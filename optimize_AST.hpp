#ifndef PF_OPTIMIZE_AST_HPP
#define PF_OPTIMIZE_AST_HPP

#include "parser/program.hpp"

void optimizeAST(Program& AST);

bool optimizeConstants(Program& AST);

#endif // OPTIMIZE_AST_HPP
