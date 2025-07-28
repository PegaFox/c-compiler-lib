#include "label.hpp"

#include "parse_error.hpp"

Label::Label()
{
  statementType = StatementType::Label;
}

Label* Label::parse(std::list<Token>& code)
{
  Label* label = new Label;

  ParseError::expect(code.front(), Token::Identifier);
  label->name = code.front().data;
  code.pop_front();

  ParseError::expect(code.front().data, ":");
  code.pop_front();

  return label;
}
