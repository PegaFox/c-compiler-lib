#ifndef PF_PARSER_CONSTANT_HPP
#define PF_PARSER_CONSTANT_HPP

#include <memory>

#include "data_type.hpp"
#include "expression.hpp"

typedef int8_t PFCC_SIGNED_CHAR;
typedef uint8_t PFCC_UNSIGNED_CHAR;
typedef int16_t PFCC_SIGNED_SHORT;
typedef uint16_t PFCC_UNSIGNED_SHORT;
typedef int16_t PFCC_SIGNED_INT;
typedef uint16_t PFCC_UNSIGNED_INT;
typedef int32_t PFCC_SIGNED_LONG;
typedef uint32_t PFCC_UNSIGNED_LONG;
typedef int64_t PFCC_SIGNED_LONG_LONG;
typedef uint64_t PFCC_UNSIGNED_LONG_LONG;

struct Constant: public Expression
{
  std::unique_ptr<DataType> dataType;

  union Value
  {
    PFCC_SIGNED_CHAR signedChar;
    PFCC_UNSIGNED_CHAR unsignedChar;
    PFCC_SIGNED_SHORT signedShort;
    PFCC_UNSIGNED_SHORT unsignedShort;
    PFCC_SIGNED_INT signedInt;
    PFCC_UNSIGNED_INT unsignedInt;
    PFCC_SIGNED_LONG signedLong;
    PFCC_UNSIGNED_LONG unsignedLong;
    PFCC_SIGNED_LONG_LONG signedLongLong;
    PFCC_UNSIGNED_LONG_LONG unsignedLongLong;
  } value;

  Constant();

  static Constant* parse(std::list<Token>& code);
};

#endif // PF_PARSER_CONSTANT_HPP
