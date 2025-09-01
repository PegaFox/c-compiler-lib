#ifndef PF_PARSER_POINTER_HPP
#define PF_PARSER_POINTER_HPP

#include <memory>

#include "data_type.hpp"

struct Pointer: public DataType
{
  DataType* dataType = nullptr;

  Pointer();

  static Pointer* parse(std::list<Token>& code);
};

#endif // PF_PARSER_POINTER_HPP
