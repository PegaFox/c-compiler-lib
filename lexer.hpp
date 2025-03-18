#ifndef PF_LEXER_HPP
#define PF_LEXER_HPP

#include <string>
#include <list>

struct Token
{
  enum Type
  {
    Keyword,
    Identifier,
    Operator,
    Constant,
    Other
  } type = Keyword;

  static const std::string typeStrings[5];

  std::string data = "";
};

std::list<Token> lex(std::string code);

#endif // PF_LEXER_HPP