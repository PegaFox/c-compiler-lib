#ifndef PF_GENERATE_IR_HPP
#define PF_GENERATE_IR_HPP

#include <map>

#include "parser.hpp"

struct Operation
{
  enum Opcode
  {
    Set,
    GetAddress,
    Dereference,
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

DataType* copyDataType(const DataType* data);

std::map<std::string, const VariableDeclaration*>::iterator getIdentifier(const std::string& identifier);

std::vector<Operation> generateIR(const Program& AST);

void generateFunctionDeclaration(std::vector<Operation>& absProgram, const FunctionDeclaration* functionDeclaration);

void generateStatement(std::vector<Operation>& absProgram, const Statement* statement);

void generateCompoundStatement(std::vector<Operation>& absProgram, const CompoundStatement* compoundStatement);

std::string generateExpression(std::vector<Operation>& absProgram, const Expression* expression);

std::string generateConstant(std::vector<Operation>& absProgram, const Constant* constant);

void generateReturn(std::vector<Operation>& absProgram, const Return* returnVal);

void generateBreak(std::vector<Operation>& absProgram, const Break* breakStatement);

void generateContinue(std::vector<Operation>& absProgram, const Continue* continueStatement);

void generateLabel(std::vector<Operation>& absProgram, const Label* label);

void generateGoto(std::vector<Operation>& absProgram, const Goto* gotoStatement);

void generateVariableDeclaration(std::vector<Operation>& absProgram, const VariableDeclaration* variableDeclaration, bool allowInitialization = true);

void generateIfConditional(std::vector<Operation>& absProgram, const IfConditional* ifConditional);

void generateSwitchCase(std::vector<Operation>& absProgram, const SwitchCase* switchCase);

void generateSwitchDefault(std::vector<Operation>& absProgram, const SwitchDefault* switchDefault);

void generateSwitchConditional(std::vector<Operation>& absProgram, const SwitchConditional* switchConditional);

void generateDoWhileLoop(std::vector<Operation>& absProgram, const DoWhileLoop* doWhileLoop);

void generateWhileLoop(std::vector<Operation>& absProgram, const WhileLoop* whileLoop);

void generateForLoop(std::vector<Operation>& absProgram, const ForLoop* forLoop);

std::string generateVariableAccess(std::vector<Operation>& absProgram, const VariableAccess* variableAccess);

std::string generateFunctionCall(std::vector<Operation>& absProgram, const FunctionCall* functionCall);

std::string generatePreUnaryOperator(std::vector<Operation>& absProgram, const PreUnaryOperator* preUnary);

std::string generatePostUnaryOperator(std::vector<Operation>& absProgram, const PostUnaryOperator* postUnary);

std::string generateBinaryOperator(std::vector<Operation>& absProgram, const BinaryOperator* binary);

std::string generateTernaryOperator(std::vector<Operation>& absProgram, const TernaryOperator* ternary);

#endif // PF_GENERATE_IR_HPP
