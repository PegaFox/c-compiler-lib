#ifndef PF_PARSER_PROGRAM_HPP
#define PF_PARSER_PROGRAM_HPP

#include <vector>
#include <list>
#include <memory>

#include "../lexer.hpp"
#include "AST_node.hpp"

struct Program: public ASTnode
{
  std::vector<std::unique_ptr<ASTnode>> nodes;

  Program();

  static Program parse(std::list<Token> code);
};

#endif // PF_PARSER_PROGRAM_HPP
