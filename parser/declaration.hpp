#ifndef PF_PARSER_DECLARATION_HPP
#define PF_PARSER_DECLARATION_HPP

#include <memory>

#include "data_type.hpp"
#include "statement.hpp"

struct Declaration: public Statement
{
  enum class Linkage
  {
    None,
    Internal,
    External
  } linkage;

  bool isTypedef = false;
  bool isInline = false;

  DataType* dataType = nullptr;

  std::string_view identifier;

  Statement* value = nullptr;

  Declaration();

  static Declaration* parse(CommonParseData& data, Linkage defaultLinkage = Linkage::External);
};

#endif // PF_PARSER_DECLARATION_HPP
