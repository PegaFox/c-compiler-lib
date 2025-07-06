#ifndef PF_PARSER_HPP
#define PF_PARSER_HPP

#include <cstdint>
#include <vector>
#include <list>
#include <memory>

#include "lexer.hpp"

typedef uint32_t ENUM_TYPE;
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
typedef uint8_t PFCC_SIZE_T;
typedef uint8_t PFCC_PTR;
typedef int16_t PFCC_PTRDIFF_T;

struct ASTnode
{
  enum class NodeType
  {
    Undefined, // if a node has this type, something went wrong
    Statement,
    Program,
    DataType
  } nodeType;
};

struct Program : public ASTnode
{
  std::vector<std::unique_ptr<ASTnode>> nodes;

  Program();
};

// generic type struct
struct DataType : public ASTnode
{
  enum class GeneralType
  {
    Undefined, // if a node has this type, something went wrong
    PrimitiveType,
    Pointer,
    Array,
    Struct
  } generalType;

  enum class Linkage
  {
    None,
    Internal,
    External
  } linkage;

  bool isConst = false;
  bool isVolatile = false;

  DataType();
};

struct PrimitiveType : public DataType
{
  enum class Type
  {
    Void,
    SignedChar,
    UnsignedChar,
    SignedShort,
    UnsignedShort,
    SignedInt,
    UnsignedInt,
    SignedLong,
    UnsignedLong,
    SignedLongLong,
    UnsignedLongLong,
    Float,
    Double,
    LongDouble
  } type;

  PrimitiveType();
};

struct Expression;

struct Pointer : public DataType
{
  std::unique_ptr<DataType> dataType;

  Pointer();
};

struct Array : public DataType
{
  std::unique_ptr<DataType> dataType;
  std::unique_ptr<Expression> size;

  Array();
};

struct Statement : public ASTnode
{
  enum class StatementType
  {
    Undefined, // if a node has this type, something went wrong
    CompoundStatement,
    Expression,
    Label,
    Return,
    Break,
    Continue,
    Goto,
    SwitchCase,
    SwitchDefault,
    VariableDeclaration,
    FunctionDeclaration,
    IfConditional,
    SwitchConditional,
    DoWhileLoop,
    WhileLoop,
    ForLoop
  } statementType;

  Statement();
};

struct CompoundStatement: public Statement
{
  std::vector<std::unique_ptr<Statement>> body;

  CompoundStatement();
};

struct Expression : public Statement
{
  enum class ExpressionType
  {
    Undefined, // if a node has this type, something went wrong
    Null,
    Constant,
    FunctionCall,
    VariableAccess,
    SubExpression,
    PreUnaryOperator,
    PostUnaryOperator,
    BinaryOperator,
    TernaryOperator
  } expressionType;

  Expression();
};

struct VariableDeclaration;

struct FunctionDeclaration : public Statement
{
  std::unique_ptr<DataType> returnType;

  std::string identifier;

  std::vector<std::unique_ptr<VariableDeclaration>> parameters;

  std::unique_ptr<CompoundStatement> body;

  FunctionDeclaration();
};

struct Label: public Statement
{
  std::string name;

  Label();
};

struct Return : public Statement
{
  std::unique_ptr<Expression> data;

  Return(); 
};

struct Break: public Statement
{
  Break();
};

struct Continue: public Statement
{
  Continue();
};

struct Goto: public Statement
{
  std::string label;

  Goto();
};

struct SwitchCase: public Statement
{
  std::unique_ptr<Expression> requirement;

  SwitchCase();
};

struct SwitchDefault: public Statement
{
  SwitchDefault();
};

struct VariableDeclaration : public Statement
{
  std::unique_ptr<DataType> dataType;

  std::string identifier;

  std::unique_ptr<Expression> value;

  VariableDeclaration();
};

struct IfConditional: public Statement
{
  std::unique_ptr<Expression> condition;

  std::unique_ptr<Statement> body;

  // associated else statement
  std::unique_ptr<Statement> elseStatement;

  IfConditional();
};

struct SwitchConditional: public Statement
{
  std::unique_ptr<Expression> value;

  std::unique_ptr<Statement> body;

  SwitchConditional();
};

struct DoWhileLoop: public Statement
{
  std::unique_ptr<Expression> condition;

  std::unique_ptr<Statement> body;

  DoWhileLoop();
};

struct WhileLoop: public Statement
{
  std::unique_ptr<Expression> condition;

  std::unique_ptr<Statement> body;

  WhileLoop();
};

struct ForLoop: public Statement
{
  std::unique_ptr<Statement> initialization;

  std::unique_ptr<Expression> condition;

  std::unique_ptr<Expression> update;

  std::unique_ptr<Statement> body;

  ForLoop();
};

struct Constant : public Expression
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
};

struct FunctionCall: public Expression
{
  std::string identifier;

  std::vector<std::unique_ptr<Expression>> arguments;

  FunctionCall();
};

struct VariableAccess: public Expression
{
  std::string identifier;

  VariableAccess();
};

struct SubExpression : public Expression
{
  std::unique_ptr<Expression> expression;

  SubExpression();
};

struct PreUnaryOperator : public Expression
{
  enum class PreUnaryType
  {
    Undefined, // if a node has this type, something went wrong
    MathematicNegate,
    BitwiseNOT,
    LogicalNegate,
    Increment,
    Decrement,
    Dereference,
    TypeCast,
    Address
  } preUnaryType;

  std::unique_ptr<Expression> operand;

  PreUnaryOperator();
};

struct PostUnaryOperator : public Expression
{
  enum class PostUnaryType
  {
    Undefined, // if a node has this type, something went wrong
    Increment,
    Decrement
  } postUnaryType;

  std::unique_ptr<Expression> operand;

  PostUnaryOperator();
};

struct TypeCast: public PreUnaryOperator
{
  std::unique_ptr<DataType> dataType;

  TypeCast();
};

struct BinaryOperator : public Expression
{
  enum class BinaryType
  {
    Undefined, // if a node has this type, something went wrong
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    LeftShift,
    RightShift,
    BitwiseOR,
    BitwiseAND,
    BitwiseXOR,
    LogicalOR,
    LogicalAND,
    Subscript,
    VariableAssignment,
    AddEqual,
    SubtractEqual,
    MultiplyEqual,
    DivideEqual,
    ModuloEqual,
    LeftShiftEqual,
    RightShiftEqual,
    BitwiseOREqual,
    BitwiseANDEqual,
    BitwiseXOREqual,
    Equal,
    NotEqual,
    Greater,
    Lesser,
    GreaterOrEqual,
    LesserOrEqual
  } binaryType;

  static const constexpr uint8_t precedence[(uint8_t)BinaryType::LesserOrEqual+1] = {
    0, // Undefined
    11, // Add
    11, // Subtract
    12, // Multiply
    12, // Divide
    12, // Modulo
    10, // LeftShift
    10, // RightShift
    5, // BitwiseOR
    7, // BitwiseAND
    6, // BitwiseXOR
    3, // LogicalOR
    4, // LogicalAND
    13, // Subscript
    2, // VariableAssignment
    2, // AddEqual
    2, // SubtractEqual
    2, // MultiplyEqual
    2, // DivideEqual
    2, // ModuloEqual
    2, // LeftShiftEqual
    2, // RightShiftEqual
    2, // BitwiseOREqual
    2, // BitwiseANDEqual
    2, // BitwiseXOREqual
    8, // Equal
    8, // NotEqual
    9, // Greater
    9, // Lesser
    9, // GreaterOrEqual
    9 // LesserOrEqual
  };

  std::unique_ptr<Expression> leftOperand;

  std::unique_ptr<Expression> rightOperand;

  BinaryOperator();
};

struct TernaryOperator: public Expression
{
  std::unique_ptr<Expression> condition;

  std::unique_ptr<Expression> trueOperand;

  std::unique_ptr<Expression> falseOperand;

  TernaryOperator();
};

struct ASTiterator
{
  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = ASTnode;
    using pointer = ASTnode*;
    using reference = ASTnode&;

    ASTiterator(pointer ptr);

    pointer operator*() const;
    pointer operator->();

    std::size_t depth();

    ASTiterator& operator++();

    ASTiterator operator++(int);

    friend bool operator== (const ASTiterator& a, const ASTiterator& b) {return a.ptr == b.ptr;};
    friend bool operator!= (const ASTiterator& a, const ASTiterator& b) {return a.ptr != b.ptr;};

    // elements in path are arrays of 2 pointers, first is the intersection node, second is the path taken (represented by a pointer to a std::unique_ptr)
    std::vector<std::pair<pointer, void*>> path;

    pointer ptr;

  private:
    bool firstTime(pointer ptr);
};

struct ParseError
{

};

int parseExpect(const std::string& testSubject, const std::string& testCase);

int parseExpect(const std::string& testSubject, const std::initializer_list<std::string>& testCases);

int parseExpect(const Token& testSubject, const Token::Type& testCase);

int parseExpect(const Token& testSubject, const std::initializer_list<Token::Type>& testCases);

Program parse(std::list<Token> code);

FunctionDeclaration* parseFunctionDeclaration(std::list<Token>& code);

Statement* parseStatement(std::list<Token>& code, bool canParseVariableDeclarations = false);

CompoundStatement* parseCompoundStatement(std::list<Token>& code);

void parseEnum(std::list<Token>& code);

Label* parseLabel(std::list<Token>& code);

Return* parseReturn(std::list<Token>& code);

Break* parseBreak(std::list<Token>& code);

Continue* parseContinue(std::list<Token>& code);

Goto* parseGoto(std::list<Token>& code);

FunctionCall* parseFunctionCall(std::list<Token>& code);

IfConditional* parseIfConditional(std::list<Token>& code);

SwitchCase* parseSwitchCase(std::list<Token>& code);

SwitchDefault* parseSwitchDefault(std::list<Token>& code);

SwitchConditional* parseSwitchConditional(std::list<Token>& code);

DoWhileLoop* parseDoWhileLoop(std::list<Token>& code);

WhileLoop* parseWhileLoop(std::list<Token>& code);

ForLoop* parseForLoop(std::list<Token>& code);

Expression* parseExpression(std::list<Token>& code, bool allowNullExpression = true);

Expression* parsePreUnary(std::list<Token>& code);

PostUnaryOperator* parsePostUnary(std::list<Token>& code, Expression* operand);

TypeCast* parseTypeCast(std::list<Token>& code);

Expression* parseBinary(std::list<Token>& code, Expression* leftOperand);

Expression* parseTernary(std::list<Token>& code, Expression* condition);

Constant* parseConstant(std::list<Token>& code);

VariableAccess* parseVariableAccess(std::list<Token>& code);

DataType* parseDataType(std::list<Token>& code, DataType::Linkage defaultLinkage = DataType::Linkage::External);

PrimitiveType* parsePrimitiveType(std::list<Token>& code);

Pointer* parsePointer(std::list<Token>& code);

Array* parseArray(std::list<Token>& code);

VariableDeclaration* parseVariableDeclaration(std::list<Token>& code);

#endif // PF_PARSER_HPP
