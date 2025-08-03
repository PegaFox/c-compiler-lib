#include "label.hpp"

#include "parse_error.hpp"

Label::Label()
{
  statementType = StatementType::Label;
}

Label* Label::parse(CommonParseData& data)
{
  Label* label = new Label;

  ParseError::expect(data.code.front(), Token::Identifier);
  label->name = data.code.front().data;
  data.code.pop_front();

  ParseError::expect(data.code.front().data, ":");
  data.code.pop_front();

  return label;
}
