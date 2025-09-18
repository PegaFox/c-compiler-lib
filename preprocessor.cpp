#include "preprocessor.hpp"

#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#include <list>

#include "compiler.hpp"

extern std::vector<std::string> includeDirs;

Preprocessor::Preprocessor()
{

}

Preprocessor::Preprocessor(std::string workingDir, std::string& code, const std::vector<std::string>& includeDirs)
{
  code = preprocess(workingDir, code, includeDirs);
}

std::string Preprocessor::preprocess(std::string workingDir, std::string code, const std::vector<std::string>& includeDirs)
{
  removeSingleLineComments(code);

  removeMultiLineComments(code);

  removeUnwantedNewlines(code);

  convertConstantTypes(code);

  handlePreprocessingDirectives(workingDir, code, includeDirs);

  resolveDefinitions(code);

  return code;
}

void Preprocessor::removeSingleLineComments(std::string& code)
{
  std::size_t pos = 0;

  while ((pos = code.find("//")) != code.npos)
  {
    std::size_t end = code.find('\n', pos);

    code.replace(pos, end-pos, " ");
  }
}

void Preprocessor::removeMultiLineComments(std::string& code)
{
  std::size_t pos = 0;

  while ((pos = code.find("/*")) != code.npos)
  {
    std::size_t end = code.find("*/", pos);

    code.replace(pos, end-pos+2, " ");
  }
}

void Preprocessor::removeUnwantedNewlines(std::string& code)
{
  std::size_t pos = 0;

  while ((pos = code.find("\\\n")) != code.npos)
  {
    code.erase(pos, 2);
  }
}

void Preprocessor::convertConstantTypes(std::string& code)
{
  std::size_t pos = 0;

  while ((pos = code.find("0x", pos)) != code.npos)
  {
    std::string hexStr = code.substr(pos, code.find_first_not_of("0123456789abcdefABCDEF", pos+2) - pos);
    code.replace(pos, hexStr.size(), std::to_string(std::stoi(hexStr, nullptr, 16)));
  }

  pos = 0;
  while ((pos = code.find("0", pos)) != code.npos)
  {
    if (std::string("b0123456789").find(code[pos-1]) != code.npos)
    {
      pos++;
      continue;
    }
    std::string octStr = code.substr(pos, code.find_first_not_of("01234567", pos) - pos);
    std::string replacementStr = std::to_string(std::stoi(octStr, nullptr, 8));
    code.replace(pos, octStr.size(), replacementStr);
    pos += replacementStr.size();
  }

  pos = 0;
  while ((pos = code.find("0b", pos)) != code.npos)
  {
    std::string binStr = code.substr(pos, code.find_first_not_of("01", pos+2) - pos);
    code.replace(pos, binStr.size(), std::to_string(std::stoi(binStr, nullptr, 2)));
  }
}

std::string Preprocessor::handleIncludeDirective(std::string& workingDir, std::string& directiveStr, const std::vector<std::string>& includeDirs)
{
  std::size_t pos;

  std::string fileStr;

  std::string filename;
  if ((pos = directiveStr.find('\"')) != directiveStr.npos)
  {
    filename = workingDir + directiveStr.substr(pos+1, directiveStr.find('\"', pos+1) - (pos+1));
    fileStr = Compiler::loadFile(filename);
  } else if ((pos = directiveStr.find('<')) != directiveStr.npos)
  {
    filename = directiveStr.substr(pos+1, directiveStr.find('>', pos+1) - (pos+1));
    for (const std::string& dir : includeDirs)
    {
      fileStr = Compiler::loadFile(dir + filename);
      if (!fileStr.empty())
      {
        break;
      }
    }
  }

  if (fileStr.empty())
  {
    std::cout << "Error: could not find file " << filename << '\n';
  }

  return preprocess(workingDir, fileStr, includeDirs);
}

bool Preprocessor::handleConditionalDirective(std::string& directiveStr)
{
  std::size_t pos = -1UL;

  if ((pos = directiveStr.find("elifdef")) != directiveStr.npos)
  {
    directiveStr.replace(0, pos+sizeof("elifdef"), "elif defined(");
  } else if ((pos = directiveStr.find("elifndef")) != directiveStr.npos)
  {
    directiveStr.replace(0, pos+sizeof("elifndef"), "elif !defined(");
  } else if ((pos = directiveStr.find("ifdef")) != directiveStr.npos)
  {
    directiveStr.replace(0, pos+sizeof("ifdef"), "if defined(");
  } else if ((pos = directiveStr.find("ifndef")) != directiveStr.npos)
  {
    directiveStr.replace(0, pos+sizeof("ifndef"), "if !defined(");
  }

  if (pos != -1UL)
  {
    directiveStr.push_back(')');
  }

  for (const std::string& name : {"endif", "else", "elif", "if"})
  {
    if ((pos = directiveStr.find(name)) != directiveStr.npos)
    {
      directiveStr.erase(0, pos + name.size());
      break;
    }
  }

  bool invert = false;

  while (directiveStr.size() != 1)
  {
    //erase leading whitespace
    directiveStr.erase(0, directiveStr.find_first_not_of(' '));

    if (directiveStr.front() == '!')
    {
      directiveStr.erase(0, 1);
      invert = !invert;
    } else if (directiveStr.substr(0, sizeof("defined(")-1) == "defined(")
    {
      directiveStr.erase(0, sizeof("defined("));

      std::string varName = directiveStr.substr(0, directiveStr.find(')'));

      if (std::find(definitions.begin(), definitions.end(), std::array<std::string, 2>{varName, ""}) != definitions.end())
      {
        directiveStr.replace(0, varName.size()+1, "1");
      } else
      {
        directiveStr.replace(0, varName.size()+1, "0");
      }
    } else
    {
      std::cout << "Error: invalid conditional directive \"" << directiveStr << "\"\n";
      break;
    }
  }

  if (invert)
  {
    return directiveStr.front() == '0';
  } else
  {
    return directiveStr.front() == '1';
  }

}

void Preprocessor::handlePreprocessingDirectives(std::string& workingDir, std::string& code, const std::vector<std::string>& includeDirs)
{
  std::size_t pos = 0;

  while ((pos = code.find('#')) != code.npos)
  {
    std::size_t end = code.find('\n', pos);

    std::string directiveStr = code.substr(pos+1, end-pos-1);

    // erase directive now so that pos can be reused
    code.erase(pos, end-pos);

    if (directiveStr.find("include") != directiveStr.npos)
    {
      code.insert(pos, handleIncludeDirective(workingDir, directiveStr, includeDirs));
    } else if (directiveStr.find("define") != directiveStr.npos)
    {
      definitions.push_front(std::array<std::string, 2>());

      directiveStr.erase(0, directiveStr.find_first_not_of(' ', sizeof("define")));

      std::size_t nameEnd = directiveStr.find_first_of(' ');

      definitions.front()[0] = directiveStr.substr(0, nameEnd);

      directiveStr.erase(0, nameEnd);

      if (directiveStr.find_first_not_of(' ') != directiveStr.npos)
      {
        definitions.front()[1] = directiveStr.substr(directiveStr.find_first_not_of(' '));
      }
      
    } else if (directiveStr.find("else") != directiveStr.npos)
    {
      if (conditionalDirectives.back() == -1UL)
      {
        conditionalDirectives.back() = pos;
      } else
      {
        code.erase(conditionalDirectives.back(), pos - conditionalDirectives.back());
        pos = conditionalDirectives.back();

        conditionalDirectives.back() = -1UL;
      }

    } else if (directiveStr.find("elif") != directiveStr.npos)
    {
      if (conditionalDirectives.back() == -1UL)
      {
        conditionalDirectives.back() = pos;
      } else if (!handleConditionalDirective(directiveStr))
      {
        code.erase(conditionalDirectives.back(), pos - conditionalDirectives.back());
        pos = conditionalDirectives.back();

        conditionalDirectives.back() = -1UL;
      }

    } else if (directiveStr.find("endif") != directiveStr.npos)
    {
      if (conditionalDirectives.back() != -1UL)
      {
        code.erase(conditionalDirectives.back(), pos - conditionalDirectives.back());
        pos = conditionalDirectives.back();
      }

      conditionalDirectives.pop_back();
    } else if (directiveStr.find("if") < 3)
    {
      if (handleConditionalDirective(directiveStr))
      {
        conditionalDirectives.push_back(-1UL);
      } else
      {
        conditionalDirectives.push_back(pos);
      }
    }
  }
}

void Preprocessor::resolveDefinitions(std::string& code)
{
  // make sure definitions are ordered by longest to shortest
  definitions.sort([](const std::array<std::string, 2>& a, const std::array<std::string, 2>& b)
  {
    return a[0].size() > b[0].size();
  });

  bool changed = true;

  while (changed)
  {
    changed = false;
    for (const std::array<std::string, 2>& definition: definitions)
    {
      std::size_t pos = code.find(definition[0]);
      while (pos != code.npos)
      {
        code.replace(pos, definition[0].size(), definition[1]);
        changed = true;
        pos = code.find(definition[0]);
      }
    }
  }
}

