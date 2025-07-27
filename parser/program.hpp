#ifndef PF_PARSER_PROGRAM_HPP
#define PF_PARSER_PROGRAM_HPP

#include <vector>
#include <memory>

#include "AST_node.hpp"

struct Program: public ASTnode
{
  std::vector<std::unique_ptr<ASTnode>> nodes;

  Program();
};

#endif // PF_PARSER_PROGRAM_HPP
