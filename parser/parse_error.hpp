#ifndef PF_PARSER_PARSE_ERROR_HPP
#define PF_PARSER_PARSE_ERROR_HPP

#include "../lexer.hpp"

struct ParseError
{
  static int expect(const std::string& testSubject, const std::string& testCase);

  static int expect(const std::string& testSubject, const std::initializer_list<std::string>& testCases);

  static int expect(const Token& testSubject, const Token::Type& testCase);

  static int expect(const Token& testSubject, const std::initializer_list<Token::Type>& testCases);
};

#endif // PF_PARSER_PARSE_ERROR_HPP
