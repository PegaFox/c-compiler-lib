#include <fstream>

/*

  this compiler is currently little endian

  gcc -S -O -fno-asynchronous-unwind-tables -fcf-protection=none main.c

*/

#include <C_compiler.hpp>
#include <parser/pointer.hpp>

int main(int argc, char* argv[])
{
  Compiler compiler;
  
  compiler.includeDirs.emplace_back("./include/");

  std::variant<IRprogram, std::string> irCode = compiler.compileFromArgs(argc, argv);

  std::ofstream outFile(compiler.outputFilename);

  if (IRprogram* ir = std::get_if<IRprogram>(&irCode))
  {
    outFile << compiler.printIR(*ir);
  } else if (std::string* text = std::get_if<std::string>(&irCode))
  {
    outFile << *text;
  }

  outFile.close();

  return 0;
}
