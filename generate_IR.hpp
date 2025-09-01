#ifndef PF_GENERATE_IR_HPP
#define PF_GENERATE_IR_HPP

#include <array>
#include <map>

#include "parser/data_type.hpp"
#include "parser/declaration.hpp"
#include "parser/primitive_type.hpp"
#include "parser/program.hpp"
#include "parser/compound_statement.hpp"
#include "parser/function.hpp"
#include "parser/constant.hpp"
#include "parser/return.hpp"
#include "parser/break.hpp"
#include "parser/continue.hpp"
#include "parser/label.hpp"
#include "parser/goto.hpp"
#include "parser/if_conditional.hpp"
#include "parser/switch_case.hpp"
#include "parser/switch_default.hpp"
#include "parser/switch_conditional.hpp"
#include "parser/do_while_loop.hpp"
#include "parser/while_loop.hpp"
#include "parser/for_loop.hpp"
#include "parser/variable_access.hpp"
#include "parser/string_literal.hpp"
#include "parser/function_call.hpp"
#include "parser/pre_unary_operator.hpp"
#include "parser/post_unary_operator.hpp"
#include "parser/binary_operator.hpp"
#include "parser/ternary_operator.hpp"

struct Operation
{
  /*struct DataType
  {
    uint16_t size = 0;
    uint8_t alignment = 0;

    bool isSigned = false;
    bool isFloating = false;
    bool isVolatile = false;
  }*/ PrimitiveType type;

  enum Opcode
  {
    Set,
    GetAddress,
    DereferenceLValue,
    DereferenceRValue,
    SetAddition,
    SetSubtraction,
    SetMultiplication,
    SetDivision,
    SetModulo,
    SetBitwiseAND,
    SetBitwiseOR,
    SetBitwiseXOR,
    SetLeftShift,
    SetRightShift,
    SetLogicalAND,
    SetLogicalOR,
    SetEqual,
    SetNotEqual,
    SetGreater,
    SetLesser,
    SetGreaterOrEqual,
    SetLesserOrEqual,
    Negate,
    LogicalNOT,
    BitwiseNOT,
    Label,
    Return,
    AddArg,
    Call,
    Jump,
    JumpIfZero,
    JumpIfNotZero,
  } code;
  std::string operands[3];
};

struct IRprogram
{
  struct Function
  {
    // value.first is the parameter index, value.second is the parameter type
    std::vector<std::pair<std::string, PrimitiveType>> parameters;
    std::vector<Operation> body;
  };

  std::vector<uint8_t> staticData;

  // map of indices into the staticData array
  std::map<std::string, std::size_t> staticVariables;

  std::vector<Function> program;
};

class GenerateIR
{
  public:
    struct IRGenError
    {

    };

    IRprogram generateIR(const Program& AST, const Compiler::TypeSizes& typeSizes);

    void optimizeIR(IRprogram& asmCode);

  private:
    struct CommonIRData
    {
      // Pointer to the body of the function currently being filled
      std::vector<Operation>* instrArray = nullptr;

      IRprogram irProgram;

      Compiler::TypeSizes typeSizes;
    };

    std::pair<std::unique_ptr<uint8_t[]>, std::size_t> dynamicData;

    //std::map<std::string, const Declaration*> declarations;
    std::map<std::string, DataType*> declarations;

    std::map<std::string, std::pair<DataType*, std::map<std::string, std::pair<PrimitiveType, uint8_t>>>> memberOffsets;

    std::vector<const CompoundStatement*> scopes;

    template<typename ObjType>
    ObjType* arenaAlloc(ObjType* object)
    {
      object = (ObjType*)&dynamicData.first[dynamicData.second];
      dynamicData.second += sizeof(ObjType);

      *object = ObjType();

      return object;
    }

    PrimitiveType ASTTypeToIRType(CommonIRData& data, const DataType* dataType);

    std::map<std::string, DataType*>::iterator getIdentifier(const std::string& identifier);

    //void generateFunctionDeclaration(CommonIRData& data, const FunctionDeclaration* functionDeclaration);

    void generateStatement(CommonIRData& data, const Statement* statement);

    void generateCompoundStatement(CommonIRData& data, const CompoundStatement* compoundStatement);

    std::pair<std::string, PrimitiveType> generateExpression(CommonIRData& data, const Expression* expression);

    std::pair<std::string, PrimitiveType> generateConstant(CommonIRData& data, Constant* constant);

    void generateReturn(CommonIRData& data, const Return* returnVal);

    void generateBreak(CommonIRData& data, const Break* breakStatement);

    void generateContinue(CommonIRData& data, const Continue* continueStatement);

    void generateLabel(CommonIRData& data, const Label* label);

    void generateGoto(CommonIRData& data, const Goto* gotoStatement);

    void generateDeclaration(CommonIRData& data, const Declaration* declaration, bool allowInitialization = true);

    void generateIfConditional(CommonIRData& data, const IfConditional* ifConditional);

    void generateSwitchCase(CommonIRData& data, const SwitchCase* switchCase);

    void generateSwitchDefault(CommonIRData& data, const SwitchDefault* switchDefault);

    void generateSwitchConditional(CommonIRData& data, const SwitchConditional* switchConditional);

    void generateDoWhileLoop(CommonIRData& data, const DoWhileLoop* doWhileLoop);

    void generateWhileLoop(CommonIRData& data, const WhileLoop* whileLoop);

    void generateForLoop(CommonIRData& data, const ForLoop* forLoop);

    std::pair<std::string, PrimitiveType> generateVariableAccess(CommonIRData& data, const VariableAccess* variableAccess);

    std::pair<std::string, PrimitiveType> generateStringLiteral(CommonIRData& data, const StringLiteral* stringLiteral);

    std::pair<std::string, PrimitiveType> generateFunctionCall(CommonIRData& data, const FunctionCall* functionCall);

    std::pair<std::string, PrimitiveType> generatePreUnaryOperator(CommonIRData& data, const PreUnaryOperator* preUnary);

    std::pair<std::string, PrimitiveType> generatePostUnaryOperator(CommonIRData& data, const PostUnaryOperator* postUnary);

    std::pair<std::string, PrimitiveType> generateBinaryOperator(CommonIRData& data, const BinaryOperator* binary);

    std::pair<std::string, PrimitiveType> generateTernaryOperator(CommonIRData& data, const TernaryOperator* ternary);

    bool resolveConstantOperations(IRprogram& irProgram);

    bool trimInaccessibleCode(IRprogram& irProgram);
};

#endif // PF_GENERATE_IR_HPP
