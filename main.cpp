#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <list>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>
#include <array>
#include <set>

/*

  this compiler is currently little endian

  gcc -S -O -fno-asynchronous-unwind-tables -fcf-protection=none main.c

*/

#include "C_compiler.hpp"

int main(int argc, char* argv[])
{
  #ifndef NDEBUG
  {
    argc = 4;

    char* args[4] = {
      "./C_compiler",
      "../tests/counting.c",
      "-O",
      "-I../include"
    };
    argv = args;
  }
  #endif // NDEBUG

  if (handleArgs(argc, argv))
  {
    return -1;
  }

  outputFilename = doAssemble ? "out.mcfunction" : "out.mc1";

  std::string fileText = loadFile(inputFilename);

  if (doPreprocess)
  {
    fileText = preprocess(fileText);
  }

  if (doCompile)
  {
    std::list<Token> code = lex(fileText);
  
    Program AST = parse(code);

    /*if (optimize)
    {
      optimizeAST(AST);
    }*/

    removeSubExpressions(AST);
  
    PrintAST printer(AST);

    std::vector<Operation> asmCode = generateIR(AST);

    if (optimize)
    {
      optimizeIR(asmCode);
    }

    fileText = printIR(asmCode);
    std::cout << fileText << '\n';
  }

  return 0;
}
