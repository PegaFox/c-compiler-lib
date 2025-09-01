#ifndef PF_PARSER_LABEL_HPP
#define PF_PARSER_LABEL_HPP

#include <string>

#include "statement.hpp"

struct Label: public Statement
{
  std::string_view name;

  Label();

  static Label* parse(CommonParseData& data);
};

#endif // PF_PARSER_LABEL_HPP
