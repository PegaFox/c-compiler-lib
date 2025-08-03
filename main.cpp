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
  Compiler compiler;
  
  compiler.includeDirs.emplace_back("./include/");

  compiler.compileFromArgs(argc, argv);

  return 0;
}
