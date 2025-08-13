#ifndef PF_GENERATE_IR_HPP
#define PF_GENERATE_IR_HPP

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
#include "parser/function_call.hpp"
#include "parser/pre_unary_operator.hpp"
#include "parser/post_unary_operator.hpp"
#include "parser/binary_operator.hpp"
#include "parser/ternary_operator.hpp"

struct Operation
{
  struct DataType
  {
    uint16_t size = 0;
    uint8_t alignment = 0;
    uint8_t pointerDepth = 0;

    bool isSigned = false;
    bool isFloating = false;

    uint16_t arrayLength = 0;

    // Identifier for structs and the like
    std::string identifier = "";
  } type;

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


class GenerateIR
{
  public:
    
    struct IRGenError
    {

    };

    std::vector<Operation> generateIR(const Program& AST, uint8_t pointerSize);

    void optimizeIR(std::vector<Operation> &asmCode);

  private:
    struct CommonIRData
    {
      std::vector<Operation> irProgram;
      uint8_t pointerSize;
    };

    std::map<std::string, const Declaration*> declarations;

    std::map<std::string, std::pair<Operation::DataType, std::map<std::string, std::pair<Operation::DataType, uint8_t>>>> memberOffsets;

    std::vector<const CompoundStatement*> scopes;

    Operation::DataType ASTTypeToIRType(CommonIRData& data, DataType* dataType);

    std::map<std::string, const Declaration*>::iterator getIdentifier(const std::string& identifier);

    //void generateFunctionDeclaration(CommonIRData& data, const FunctionDeclaration* functionDeclaration);

    void generateStatement(CommonIRData& data, const Statement* statement);

    void generateCompoundStatement(CommonIRData& data, const CompoundStatement* compoundStatement);

    std::pair<std::string, Operation::DataType> generateExpression(CommonIRData& data, const Expression* expression);

    std::pair<std::string, Operation::DataType> generateConstant(CommonIRData& data, const Constant* constant);

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

    std::pair<std::string, Operation::DataType> generateVariableAccess(CommonIRData& data, const VariableAccess* variableAccess);

    std::pair<std::string, Operation::DataType> generateFunctionCall(CommonIRData& data, const FunctionCall* functionCall);

    std::pair<std::string, Operation::DataType> generatePreUnaryOperator(CommonIRData& data, const PreUnaryOperator* preUnary);

    std::pair<std::string, Operation::DataType> generatePostUnaryOperator(CommonIRData& data, const PostUnaryOperator* postUnary);

    std::pair<std::string, Operation::DataType> generateBinaryOperator(CommonIRData& data, const BinaryOperator* binary);

    std::pair<std::string, Operation::DataType> generateTernaryOperator(CommonIRData& data, const TernaryOperator* ternary);

    bool resolveConstantOperations(std::vector<Operation> &absCode);

    bool trimInaccessibleCode(std::vector<Operation> &absCode);
};

#endif // PF_GENERATE_IR_HPP
