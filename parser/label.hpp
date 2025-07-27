#ifndef PF_PARSER_LABEL_HPP
#define PF_PARSER_LABEL_HPP

#include <string>

#include "statement.hpp"

struct Label: public Statement
{
  std::string name;

  Label();

  static Label* parse(std::list<Token>& code);
};

#endif // PF_PARSER_LABEL_HPP
