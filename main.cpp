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

#include "compiler.hpp"

int main(int argc, char* argv[])
{
  /*#ifndef NDEBUG
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
  #endif // NDEBUG*/

  Compiler compiler;
  
  compiler.includeDirs.emplace_back("./include/");

  if (compiler.handleArgs(argv, argv+argc))
  {
    return -1;
  }

  compiler.outputFilename = compiler.doAssemble ? "out.a" : "out.s";

  std::string fileText = loadFile(compiler.inputFilename);

  if (metadata.doPreprocess)
  {
    fileText = preprocess(fileText);
    std::cout << "Preprocessed:\n" << fileText << '\n';
  }

  if (metadata.doCompile)
  {
    std::list<Token> code = lex(fileText);
    std::cout << "Tokens:\n";

    for (const Token& token: code)
    {
      std::cout << Token::typeStrings[token.type] << ": \"" << token.data << "\"\n";
    }
  
    Program AST(code);

    /*if (optimize)
    {
      optimizeAST(AST);
    }*/

    removeSubExpressions(AST);
  
    std::cout << "AST:\n";
    PrintAST printer(AST);

    std::vector<Operation> asmCode = generateIR(AST);

    if (metadata.optimize)
    {
      optimizeIR(asmCode);
    }

    fileText = printIR(asmCode);
    std::cout << "Intermediate Representation:\n" << fileText << '\n';
  }

  return 0;
}
