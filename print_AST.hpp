#ifndef PF_PRINT_AST_HPP
#define PF_PRINT_AST_HPP

#include "parser.hpp"

class PrintAST
{
  public:

    PrintAST(const Program& AST);

  private:

    std::string depthPadding();

    void printFunctionDeclaration(const FunctionDeclaration* functionDeclaration);

    void printStatement(const Statement* statement);

    void printCompoundStatement(const CompoundStatement* compoundStatement);

    void printVariableDeclaration(const VariableDeclaration* var);

    void printFunctionCall(const FunctionCall* functionCall);

    void printIfConditional(const IfConditional* ifConditional);

    void printWhileLoop(const WhileLoop* whileLoop);

    void printSwitchCase(const SwitchCase* switchCase);

    void printSwitchDefault(const SwitchDefault* switchDefault);

    void printSwitchConditional(const SwitchConditional* switchConditional);

    void printDoWhileLoop(const DoWhileLoop* doWhileLoop);

    void printForLoop(const ForLoop* forLoop);

    std::string printDataType(const DataType* dataType);

    void printReturn(const Return* returnVal);

    void printBreak(const Break* breakStatement);
  
    void printContinue(const Continue* continueStatement);

    void printLabel(const Label* label);

    void printGoto(const Goto* gotoStatement);

    void printExpression(const Expression* expression);

    void printConstant(const Constant* constant);

    void printVariableAccess(const VariableAccess* variableAccess);

    void printSubExpression(const SubExpression* subExpression);

    void printPreUnaryOperator(const PreUnaryOperator* preUnary);

    void printPostUnaryOperator(const PostUnaryOperator* postUnary);

    void printBinaryOperator(const BinaryOperator* binary);

    void printTernaryOperator(const TernaryOperator* ternary);

    uint32_t depth = 0;

};

#endif // PF_PRINT_AST_HPP
