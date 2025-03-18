#ifndef PF_C_COMPILER_HPP
#define PF_C_COMPILER_HPP

#include "handle_args.hpp"

#include "load_file.hpp"

#include "preprocessor.hpp"

#include "lexer.hpp"

#include "parser.hpp"

#include "optimize_AST.hpp"

#include "remove_sub_expressions.hpp"

#include "print_AST.hpp"

#include "generate_IR.hpp"

#include "optimize_IR.hpp"

#include "print_IR.hpp"

#include "save_file.hpp"

extern std::string inputFilename;
extern std::string outputFilename;
extern std::vector<std::string> includeDirs;
extern bool doPreprocess;
extern bool doCompile;
extern bool doAssemble;
extern bool optimize;

#endif // PF_C_COMPILER_HPP