#include "handle_args.hpp"

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

extern std::string inputFilename;
extern std::string outputFilename;
extern std::vector<std::string> includeDirs;
extern bool doPreprocess;
extern bool doCompile;
extern bool doAssemble;
extern bool optimize;

int handleArgs(int argc, char* argv[])
{
  for (uint16_t arg = 0; arg < argc; arg++)
  {
    std::string argStr = argv[arg];

    if (argStr == "-o" && arg < argc-1)
    {
      arg++;
      outputFilename = argv[arg];
    } else if (argStr.substr(0, 2) == "-I")
    {
      includeDirs.push_back(argStr.substr(2));
      if (includeDirs.back().back() != '/')
      {
        includeDirs.back().push_back('/');
      }
    } else if (argStr == "-S")
    {
      doAssemble = false;
    } else if (argStr == "-E")
    {
      doCompile = false;
      doAssemble = false;
    } else if (argStr == "-O")
    {
      optimize = true;
    } else if (argStr.find(".mc1") != argStr.npos)
    {
      doPreprocess = false;
      doCompile = false;
      inputFilename = argStr;
    } else if (argStr.find(".c") != argStr.npos)
    {
      doPreprocess = true;
      inputFilename = argStr;
    } else if (argStr.find(".i") != argStr.npos)
    {
      doPreprocess = false;
      inputFilename = argStr;
    }/* else if (argStr.find(".h") != argStr.npos) add precompiled headers
    {
      doPreprocess = true;
      inputFilename = argStr;
    }*/

  }

  if (inputFilename.empty())
  {
    std::clog << "Error: No input file(s), compilation terminated\n";
    return -1;
  }

  return 0;
}