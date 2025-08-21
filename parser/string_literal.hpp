#ifndef PF_PARSER_STRING_LITERAL_HPP
#define PF_PARSER_STRING_LITERAL_HPP

#include <string>

#include "expression.hpp"

struct StringLiteral: public Expression
{
  std::string value;

  StringLiteral();

  static StringLiteral* parse(CommonParseData& data);
};

#endif // PF_PARSER_STRING_LITERAL_HPP
