#ifndef PF_COMPILER_HPP
#define PF_COMPILER_HPP

#include <cstdint>
#include <string>
#include <vector>

struct Operation;

class Program;

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
      uint8_t pointerSize = 8;
    } typeSizes;

    bool doPreprocess = true;

    bool doCompile = true;

    bool doAssemble = true;

    bool optimize = false;

    Compiler();

    Compiler(int argc, char* argv[]);

    void compileFromArgs(int argc, char* argv[]);

    int handleArgs(int argc, char* argv[]);

    static std::string loadFile(const std::string& filename);

    static void optimizeAST(Program& AST);
  
    static std::string printIR(const std::vector<Operation>& asmCode);
};

#endif // PF_COMPILER_HPP
