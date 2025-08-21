#include "lexer.hpp"

#include <iostream>
#include <algorithm>

struct LexError
{

};

// tokens array must be sorted by string length
const std::string tokens[] = {
  // keywords
  "continue",
  "register",
  "unsigned",
  "volatile",
  "default",
  "typedef",
  "static",
  "signed",
  "double",
  "sizeof",
  "struct",
  "switch",
  "extern",
  "return",
  "float",
  "short",
  "while",
  "break",
  "const",
  "union",
  "char",
  "long",
  "auto",
  "case",
  "enum",
  "else",
  "goto",
  "void",
  "int",
  "for",
  "if",
  "do",

  // operators
  ">>=",
  "<<=",
  "+=",
  "-=",
  "*=",
  "/=",
  "%=",
  "&=",
  "|=",
  "^=",
  "--",
  "==",
  "!=",
  ">=",
  "<=",
  "++",
  "&&",
  "||",
  "<<",
  ">>",
  "->",
  ">",
  "<",
  "-",
  "~",
  "!",
  "&",
  "|",
  "^",
  "+",
  "*",
  "/",
  "%",
  "=",
  ".",

  // other
  "?",
  ":",
  ";",
  ",",
  ".",
  "(",
  ")",
  "[",
  "]",
  "{",
  "}"
};

const std::string whitespace = " \t\n\v\f\r";

const std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

const std::string numbers = "0123456789";

const std::string Token::typeStrings[5] = {
  "a keyword",
  "an identifier",
  "an operator",
  "a constant",
  "a misc token"
};

std::list<Token> lex(std::string code)
{
  std::list<Token> tokenized;

  std::size_t pos = 0;
  for (std::size_t end = 0; end <= code.size(); end++)
  {
    std::string test = code.substr(pos, end - pos);

    if (test.find_last_not_of("_"+letters+numbers) != test.npos && test.find_first_of("_"+letters) < test.find_first_of(numbers+".") && test.find_first_of("_"+letters) < test.find_last_not_of("_"+letters+numbers))
    {
      if (test.find_first_not_of("_"+whitespace+letters+numbers) < test.find_first_of("_"+letters+numbers))
      {
        std::cout << "Lex error: Expected identifier, received \"" << test.substr(0, test.find_first_of("_"+letters+numbers)) << "\"\n";
        throw LexError();
      }

      std::string identifierStr = test.substr(test.find_first_of("_"+letters), test.find_last_of("_"+letters+numbers)-test.find_first_of("_"+letters)+1);
      if (std::find(std::begin(tokens), std::end(tokens), identifierStr) == std::end(tokens))
      {
        tokenized.push_back(Token{Token::Identifier, identifierStr});
        pos = end-1;
      }
    } else if (test.find_last_not_of("_"+letters+numbers+".") != test.npos && test.find_first_of(numbers) < test.find_last_not_of("_"+letters+numbers+"."))
    {
      tokenized.push_back(Token{Token::Constant, test.substr(test.find_first_of(numbers), test.find_last_not_of(numbers+".")-test.find_first_of(numbers))});
      pos = end-1;
    } else if (test.find_first_of("'") != test.npos || test.find_first_of("\"") != test.npos)
    {
      tokenized.push_back(Token{Token::Constant, ""});

      for (std::size_t c = code.find_first_of(test.find_first_of("'") < test.find_first_of("\"") ? "'" : "\"", pos); c < code.size(); c++)
      {
        switch (code[c])
        {
          case '\'':
          case '"':
            tokenized.back().data.push_back(code[c]);

            if (tokenized.back().data.size() > 1 && code[c] == tokenized.back().data.front())
            {
              pos = c+1;
              end = pos;
              c = -2;
            }
            break;
          case '\\':
            c++;
            switch (code[c])
            {
              case 'n':
                tokenized.back().data.push_back('\n');
                break;
              case 't':
                tokenized.back().data.push_back('\t');
                break;
              case 'r':
                tokenized.back().data.push_back('\r');
                break;
              case 'a':
                tokenized.back().data.push_back('\a');
                break;
              case 'b':
                tokenized.back().data.push_back('\b');
                break;
              case 'f':
                tokenized.back().data.push_back('\f');
                break;
              case 'v':
                tokenized.back().data.push_back('\v');
                break;
              case '\\':
                tokenized.back().data.push_back('\\');
                break;
              case '\'':
                tokenized.back().data.push_back('\'');
                break;
              case '\"':
                tokenized.back().data.push_back('\"');
                break;
              case '?':
                tokenized.back().data.push_back('\?');
                break;
              default:
                c--;
                break;
            }

            break;
          default:
            tokenized.back().data.push_back(code[c]);
            break;
        }
      }
    }

    // find longest token that matches
    std::size_t tokenPos = code.npos;
    Token firstToken;

    Token::Type currentType = Token::Keyword;
    for (const std::string& token : tokens)
    {
      if (token == ">>=")
      {
        currentType = Token::Operator;
      } else if (token == ":")
      {
        currentType = Token::Other;
      }
      std::size_t testPos = code.find(token, pos);
      if (testPos < tokenPos)
      {
        if (currentType == Token::Keyword && std::string("_"+letters+numbers+".").find(code[testPos+token.size()]) != code.npos)
        {
          continue;
        }
        tokenPos = code.find(token, pos);
        firstToken = Token{currentType, token};
      }
    }

    if (tokenPos == 0 || (tokenPos <= code.find_first_of("_"+letters, pos) && tokenPos < code.find_first_of(numbers+".", pos) && tokenPos < code.find_first_of("'\"", pos))) {
      tokenized.push_back(firstToken);
      pos = tokenPos + firstToken.data.size();
      end = pos;
    }
  }

  if (pos < code.size()-1 && code.substr(pos).find_first_not_of(whitespace) != std::string::npos)
  {
    std::cout << "Lex error: Encountered garbage characters at end of file\n";
    throw LexError();
  }
  return tokenized;
}
