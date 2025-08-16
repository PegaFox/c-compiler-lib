#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <cstdio>
#include <fstream>

/*

  this compiler is currently little endian

  gcc -S -O -fno-asynchronous-unwind-tables -fcf-protection=none main.c

*/

#include <C_compiler.hpp>

#include "assembly_generation.hpp"

int main(int argc, char* argv[])
{
  Compiler compiler;
  
  compiler.includeDirs.emplace_back("./include/");

  std::vector<Operation> irCode = compiler.compileFromArgs(argc, argv);

  std::vector<std::array<std::string, 3>> asmCode = generateASM(irCode, compiler.outputFilename);

  std::ofstream asmFile(compiler.outputFilename+".s");

  for (std::array<std::string, 3>& instruction: asmCode)
  {
    asmFile << instruction[0];

    if (!instruction[1].empty())
    {
      asmFile << '\t' << instruction[1];
    }

    if (!instruction[2].empty())
    {
      asmFile << ", " << instruction[2];
    }

    asmFile << '\n';
  }

  asmFile.close();

  std::string argString = compiler.outputFilename+".s -o "+compiler.outputFilename;
  char* args[3] = {argString.data(), argString.data()+compiler.outputFilename.size()+3, argString.data()+compiler.outputFilename.size()+6};

  //std::cout << execv("gcc", args) << '\n';
  FILE* output = popen(("gcc "+argString).c_str(), "r");
  if (output)
  {
    while (!feof(output))
    {
      char buffer[128];

      std::cout << fgets(buffer, 128, output);
    }

    fclose(output);
  }

  //std::filesystem::remove(std::filesystem::path(compiler.outputFilename+".s"));

  return 0;
}
