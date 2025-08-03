#ifndef PF_COMPILER_HPP
#define PF_COMPILER_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "parser/program.hpp"

class Compiler
{
  public:
    std::string inputFilename;

    std::string outputFilename = "a.out";

    std::vector<std::string> includeDirs;

    struct TypeSizes
    {
      uint8_t charSize = 1;
      uint8_t shortSize = 2;
      uint8_t intSize = 4;
      uint8_t longSize = 8;
      uint8_t longLongSize = 8;
      uint8_t floatSize = 4;
      uint8_t doubleSize = 8;
      uint8_t longDoubleSize = 10;
    } typeSizes;

    bool doPreprocess = true;

    bool doCompile = true;

    bool doAssemble = true;

    bool optimize = false;

    Compiler();

    template <typename IIter>
    Compiler(IIter argsBegin, IIter argsEnd);

    template <typename IIter>
    void compileFromArgs(IIter argsBegin, IIter argsEnd);

    template <typename IIter>
    int handleArgs(IIter argsBegin, IIter argsEnd);

    std::string loadFile(const std::string& filename);

    void optimizeAST(Program& AST);
};

#endif // PF_COMPILER_HPP
