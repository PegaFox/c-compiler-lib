#include "parse_error.hpp"

#include <iostream>

int ParseError::expect(const std::string& testSubject, const std::string& testCase)
{
  if (testSubject != testCase)
  {
    std::cout << "Parse error: Expected \"" << testCase << "\", received \"" << testSubject << "\"\n";
    throw ParseError();
  }
  return 0;
}

int ParseError::expect(const std::string& testSubject, const std::initializer_list<std::string>& testCases)
{
  std::string errorMessage = "Parse error: Expected ";

  for (std::initializer_list<std::string>::const_iterator testCase = testCases.begin(); testCase < testCases.end(); testCase++)
  {
    if (testSubject == *testCase)
    {
      return testCase - testCases.begin();
    }

    if (testCase != testCases.begin() && testCase+1 == testCases.end())
    {
      errorMessage.append("or ");
    }

    errorMessage.push_back('\"');
    errorMessage.append(*testCase);
    errorMessage.append("\", ");
  }
  std::cout << errorMessage << "received \"" << testSubject << "\"\n";
  throw ParseError();
}

int ParseError::expect(const Token& testSubject, const Token::Type& testCase)
{
  if (testSubject.type != testCase)
  {
    std::string subjectStr;
    std::cout << "Parse error: Expected " << Token::typeStrings[testCase] << ", received " << Token::typeStrings[testSubject.type] << "\n";
    throw ParseError();
  }
  return 0;
}

int ParseError::expect(const Token& testSubject, const std::initializer_list<Token::Type>& testCases)
{
  std::string errorMessage = "Parse error: Expected ";

  for (std::initializer_list<Token::Type>::const_iterator testCase = testCases.begin(); testCase < testCases.end(); testCase++)
  {
    if (testSubject.type == *testCase)
    {
      return testCase - testCases.begin();
    }

    if (testCase != testCases.begin() && testCase+1 == testCases.end())
    {
      errorMessage.append("or ");
    }

    errorMessage.append(Token::typeStrings[*testCase]);
    errorMessage.append(", ");
  }
  std::cout << errorMessage << "received " << Token::typeStrings[testSubject.type] << "\n";
  throw ParseError();
}

