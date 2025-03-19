#include "parser.hpp"

#include <iostream>

extern bool optimize;

Program::Program()
{
  nodeType = NodeType::Program;
}

DataType::DataType()
{
  nodeType = NodeType::DataType;
}

PrimitiveType::PrimitiveType()
{
  generalType = GeneralType::PrimitiveType;
}

Pointer::Pointer()
{
  generalType = GeneralType::Pointer;
}

Array::Array()
{
  generalType = GeneralType::Array;
}

Statement::Statement()
{
  nodeType = NodeType::Statement;
}

CompoundStatement::CompoundStatement()
{
  statementType = StatementType::CompoundStatement;
}

Expression::Expression()
{
  statementType = StatementType::Expression;
}

FunctionDeclaration::FunctionDeclaration()
{
  nodeType = NodeType::FunctionDeclaration;
}

Label::Label()
{
  statementType = StatementType::Label;
}

Return::Return()
{
  statementType = StatementType::Return;
}

Break::Break()
{
  statementType = StatementType::Break;
}

Continue::Continue()
{
  statementType = StatementType::Continue;
}

Goto::Goto()
{
  statementType = StatementType::Goto;
}

SwitchCase::SwitchCase()
{
  statementType = StatementType::SwitchCase;
}

SwitchDefault::SwitchDefault()
{
  statementType = StatementType::SwitchDefault;
}

VariableDeclaration::VariableDeclaration()
{
  statementType = StatementType::VariableDeclaration;
}

IfConditional::IfConditional()
{
  statementType = StatementType::IfConditional;
}

SwitchConditional::SwitchConditional()
{
  statementType = StatementType::SwitchConditional;
}

DoWhileLoop::DoWhileLoop()
{
  statementType = StatementType::DoWhileLoop;
}

WhileLoop::WhileLoop()
{
  statementType = StatementType::WhileLoop;
}

ForLoop::ForLoop()
{
  statementType = StatementType::ForLoop;
}

Constant::Constant()
{
  expressionType = ExpressionType::Constant;
}

FunctionCall::FunctionCall()
{
  expressionType = ExpressionType::FunctionCall;
}

VariableAccess::VariableAccess()
{
  expressionType = ExpressionType::VariableAccess;
}

SubExpression::SubExpression()
{
  expressionType = ExpressionType::SubExpression;
}

PreUnaryOperator::PreUnaryOperator()
{
  expressionType = ExpressionType::PreUnaryOperator;
}

PostUnaryOperator::PostUnaryOperator()
{
  expressionType = ExpressionType::PostUnaryOperator;
}

TypeCast::TypeCast()
{
  preUnaryType = PreUnaryType::TypeCast;
}

BinaryOperator::BinaryOperator()
{
  expressionType = ExpressionType::BinaryOperator;
}

TernaryOperator::TernaryOperator()
{
  expressionType = ExpressionType::TernaryOperator;
}

ASTiterator::ASTiterator(pointer ptr) : ptr(ptr)
{
  
}

ASTiterator::pointer ASTiterator::operator*() const {return ptr;}
ASTiterator::pointer ASTiterator::operator->() {return ptr;}

std::size_t ASTiterator::depth() {return path.size();}

ASTiterator& ASTiterator::operator++()
{
  bool first = true;

  topOfConditionals:
  if (!first && path.empty())
  {
    ptr = nullptr;
  }

  if (ptr == nullptr)
  {
    return *this;
  }
  
  first = false;
  switch (ptr->nodeType)
  {
    case ASTnode::NodeType::Statement: {
      Statement* statement = (Statement*)ptr;
      switch (statement->statementType)
      {
        case Statement::StatementType::CompoundStatement: {
          CompoundStatement* compoundStatement = (CompoundStatement*)statement;

          if (firstTime(compoundStatement))
          {
            if (compoundStatement->body.empty())
            {
              ptr = path.back().first;
              goto topOfConditionals;
            } else
            {
              path.push_back({compoundStatement, &(compoundStatement->body[0])});
              ptr = compoundStatement->body[0].get();
              break;
            }
          }

          for (std::vector<std::unique_ptr<Statement>>::iterator node = compoundStatement->body.begin(); node != compoundStatement->body.end(); node++)
          {
            if (&(*node) == path.back().second)
            {
              if (node+1 == compoundStatement->body.end())
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              } else
              {
                path.back().second = &(*(node+1));
                ptr = (node+1)->get();
              }
              break;
            }
          }
          break;
        } case Statement::StatementType::Expression: {
          Expression* expression = (Expression*)statement;
          switch (expression->expressionType)
          {
            case Expression::ExpressionType::Null:
              ptr = path.back().first;
              goto topOfConditionals;
              break;
            case Expression::ExpressionType::Constant: {
              Constant* constant = (Constant*)expression;
              if (firstTime(constant))
              {
                path.push_back({constant, &constant->dataType});
                ptr = constant->dataType.get();
              } else
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } case Expression::ExpressionType::FunctionCall:

              break;
            case Expression::ExpressionType::VariableAccess:
              ptr = path.back().first;
              goto topOfConditionals;
              break;
            case Expression::ExpressionType::SubExpression: {
              SubExpression* subExpression = (SubExpression*)expression;
              if (firstTime(subExpression))
              {
                path.push_back({subExpression, &subExpression->expression});
                ptr = subExpression->expression.get();
              } else
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } case Expression::ExpressionType::PreUnaryOperator: {
              PreUnaryOperator* preUnaryOperator = (PreUnaryOperator*)expression;
              if (firstTime(preUnaryOperator))
              {
                path.push_back({preUnaryOperator, &preUnaryOperator->operand});
                ptr = preUnaryOperator->operand.get();
              } else
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } case Expression::ExpressionType::PostUnaryOperator: {
              PostUnaryOperator* postUnaryOperator = (PostUnaryOperator*)expression;
              if (firstTime(postUnaryOperator))
              {
                path.push_back({postUnaryOperator, &postUnaryOperator->operand});
                ptr = postUnaryOperator->operand.get();
              } else
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } case Expression::ExpressionType::BinaryOperator: {
              BinaryOperator* binaryOperator = (BinaryOperator*)expression;
              if (firstTime(binaryOperator))
              {
                path.push_back({binaryOperator, &binaryOperator->leftOperand});
                ptr = binaryOperator->leftOperand.get();
              } else if (path.back().second == &binaryOperator->leftOperand)
              {
                path.back().second = &binaryOperator->rightOperand;
                ptr = binaryOperator->rightOperand.get();
              } else if (path.back().second == &binaryOperator->rightOperand)
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } case Expression::ExpressionType::TernaryOperator: {
              TernaryOperator* ternaryOperator = (TernaryOperator*)statement;
              if (firstTime(ternaryOperator))
              {
                path.push_back({ternaryOperator, &ternaryOperator->condition});
                ptr = ternaryOperator->condition.get();
              } else if (path.back().second == &ternaryOperator->condition)
              {
                path.back().second = &ternaryOperator->trueOperand;
                ptr = ternaryOperator->trueOperand.get();
              } else if (ternaryOperator->falseOperand && path.back().second == &ternaryOperator->trueOperand)
              {
                path.back().second = &ternaryOperator->falseOperand;
                ptr = ternaryOperator->falseOperand.get();
              } else if (!ternaryOperator->falseOperand || path.back().second == &ternaryOperator->falseOperand)
              {
                path.pop_back();
                ptr = path.back().first;
                goto topOfConditionals;
              }
              break;
            } 
          }
          break;
        } case Statement::StatementType::Label:
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        case Statement::StatementType::Return: {
          Return* returnStatement = (Return*)statement;
          if (firstTime(returnStatement))
          {
            path.push_back({returnStatement, &returnStatement->data});
            ptr = returnStatement->data.get();
          } else
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::Break:
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        case Statement::StatementType::Continue:
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        case Statement::StatementType::Goto:
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        case Statement::StatementType::VariableDeclaration: {
          VariableDeclaration* variableDeclaration = (VariableDeclaration*)statement;
          if (firstTime(variableDeclaration))
          {
            path.push_back({variableDeclaration, &variableDeclaration->dataType});
            ptr = variableDeclaration->dataType.get();
          } else if (variableDeclaration->value && path.back().second == &variableDeclaration->dataType)
          {
            path.back().second = &variableDeclaration->value;
            ptr = variableDeclaration->value.get();
          } else if (!variableDeclaration->value || path.back().second == &variableDeclaration->value)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::IfConditional: {
          IfConditional* ifConditional = (IfConditional*)statement;
          if (firstTime(ifConditional))
          {
            path.push_back({ifConditional, &ifConditional->condition});
            ptr = ifConditional->condition.get();
          } else if (path.back().second == &ifConditional->condition)
          {
            path.back().second = &ifConditional->body;
            ptr = ifConditional->body.get();
          } else if (ifConditional->elseStatement && path.back().second == &ifConditional->body)
          {
            path.back().second = &ifConditional->elseStatement;
            ptr = ifConditional->elseStatement.get();
          } else if (!ifConditional->elseStatement || path.back().second == &ifConditional->elseStatement)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::SwitchCase: {
          SwitchCase* switchCase = (SwitchCase*)statement;
          if (firstTime(switchCase))
          {
            path.push_back({switchCase, &switchCase->requirement});
            ptr = switchCase->requirement.get();
          } else
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::SwitchDefault:
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        case Statement::StatementType::SwitchConditional: {
          SwitchConditional* switchConditional = (SwitchConditional*)statement;
          if (firstTime(switchConditional))
          {
            path.push_back({switchConditional, &switchConditional->value});
            ptr = switchConditional->value.get();
          } else if (path.back().second == &switchConditional->value)
          {
            path.back().second = &switchConditional->body;
            ptr = switchConditional->body.get();
          } else if (path.back().second == &switchConditional->body)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::DoWhileLoop: {
          DoWhileLoop* doWhileLoop = (DoWhileLoop*)statement;
          if (firstTime(doWhileLoop))
          {
            path.push_back({doWhileLoop, &doWhileLoop->condition});
            ptr = doWhileLoop->condition.get();
          } else if (path.back().second == &doWhileLoop->condition)
          {
            path.back().second = &doWhileLoop->body;
            ptr = doWhileLoop->body.get();
          } else if (path.back().second == &doWhileLoop->body)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::WhileLoop: {
          WhileLoop* whileLoop = (WhileLoop*)statement;
          if (firstTime(whileLoop))
          {
            path.push_back({whileLoop, &whileLoop->condition});
            ptr = whileLoop->condition.get();
          } else if (path.back().second == &whileLoop->condition)
          {
            path.back().second = &whileLoop->body;
            ptr = whileLoop->body.get();
          } else if (path.back().second == &whileLoop->body)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case Statement::StatementType::ForLoop:
          ForLoop* forLoop = (ForLoop*)statement;
          if (firstTime(forLoop))
          {
            path.push_back({forLoop, &forLoop->initialization});
            ptr = forLoop->initialization.get();
          } else if (path.back().second == &forLoop->initialization)
          {
            path.back().second = &forLoop->condition;
            ptr = forLoop->condition.get();
          } else if (path.back().second == &forLoop->condition)
          {
            path.back().second = &forLoop->update;
            ptr = forLoop->update.get();
          } else if (path.back().second == &forLoop->update)
          {
            path.back().second = &forLoop->body;
            ptr = forLoop->body.get();
          } else if (path.back().second == &forLoop->body)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
      }
      break;
    } case ASTnode::NodeType::FunctionDeclaration: {
      FunctionDeclaration* functionDeclaration = (FunctionDeclaration*)ptr;
      if (firstTime(functionDeclaration))
      {
        path.push_back({functionDeclaration, &functionDeclaration->returnType});
        ptr = functionDeclaration->returnType.get();
      } else if (path.back().second == &functionDeclaration->returnType)
      {
        path.back().second = &functionDeclaration->body;
        ptr = functionDeclaration->body.get();
      } else if (path.back().second == &functionDeclaration->body)
      {
        path.pop_back();
        ptr = path.back().first;
        goto topOfConditionals;
      }
      break;
    } case ASTnode::NodeType::Program: {
      Program* program = (Program*)ptr;

      if (firstTime(program) && !program->nodes.empty())
      {
        path.push_back({program, &(program->nodes[0])});
        ptr = program->nodes[0].get();
        break;
      }

      for (std::vector<std::unique_ptr<ASTnode>>::iterator node = program->nodes.begin(); node != program->nodes.end(); node++)
      {
        if (&(*node) == path.back().second)
        {
          if (node+1 == program->nodes.end())
          {
            ptr = nullptr;
          } else
          {
            path.back().second = &(*(node+1));
            ptr = (node+1)->get();
          }
          break;
        }
      }
      break;
    } case ASTnode::NodeType::DataType:
      DataType* dataType = (DataType*)ptr;
      switch (dataType->generalType)
      {
        case DataType::GeneralType::PrimitiveType: {
          ptr = path.back().first;
          goto topOfConditionals;
          break;
        } case DataType::GeneralType::Pointer: {
          Pointer* pointer = (Pointer*)dataType;
          if (firstTime(pointer))
          {
            path.push_back({pointer, &pointer->dataType});
            ptr = pointer->dataType.get();
          } else
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case DataType::GeneralType::Array: {
          Array* array = (Array*)dataType;
          if (firstTime(array))
          {
            path.push_back({array, &array->dataType});
            ptr = array->dataType.get();
          } else if (path.back().second == &array->dataType)
          {
            path.back().second = &array->size;
            ptr = array->size.get();
          } else if (path.back().second == &array->size)
          {
            path.pop_back();
            ptr = path.back().first;
            goto topOfConditionals;
          }
          break;
        } case DataType::GeneralType::Struct:

          break;
      }
      break;
  }

  if (path.empty())
  {
    ptr = nullptr;
  }

  return *this;
}

ASTiterator ASTiterator::operator++(int) {ASTiterator tmp = *this; ++(*this); return tmp;}

bool ASTiterator::firstTime(pointer ptr)
{
  return path.empty() || (ptr != path.back().first);
}

int parseExpect(const std::string& testSubject, const std::string& testCase)
{
  if (testSubject != testCase)
  {
    std::cout << "Parse error: Expected \"" << testCase << "\", received \"" << testSubject << "\"\n";
    throw ParseError();
  }
  return 0;
}

int parseExpect(const std::string& testSubject, const std::initializer_list<std::string>& testCases)
{
  std::string errorMessage = "Parse error: Expected ";

  for (std::initializer_list<std::string>::const_iterator testCase = testCases.begin(); testCase < testCases.end(); testCase++)
  {
    if (testSubject == *testCase)
    {
      return testCase - testCases.begin();
    }

    if (testCase != testCases.begin() && testCase+1 == testCases.end())
    {
      errorMessage.append("or ");
    }

    errorMessage.push_back('\"');
    errorMessage.append(*testCase);
    errorMessage.append("\", ");
  }
  std::cout << errorMessage << "received \"" << testSubject << "\"\n";
  throw ParseError();
}

int parseExpect(const Token& testSubject, const Token::Type& testCase)
{
  if (testSubject.type != testCase)
  {
    std::string subjectStr;
    std::cout << "Parse error: Expected " << Token::typeStrings[testCase] << ", received " << Token::typeStrings[testSubject.type] << "\n";
    throw ParseError();
  }
  return 0;
}

int parseExpect(const Token& testSubject, const std::initializer_list<Token::Type>& testCases)
{
  std::string errorMessage = "Parse error: Expected ";

  for (std::initializer_list<Token::Type>::const_iterator testCase = testCases.begin(); testCase < testCases.end(); testCase++)
  {
    if (testSubject.type == *testCase)
    {
      return testCase - testCases.begin();
    }

    if (testCase != testCases.begin() && testCase+1 == testCases.end())
    {
      errorMessage.append("or ");
    }

    errorMessage.append(Token::typeStrings[*testCase]);
    errorMessage.append(", ");
  }
  std::cout << errorMessage << "received " << Token::typeStrings[testSubject.type] << "\n";
  throw ParseError();
}

Program parse(std::list<Token> code)
{
  Program program;

  while (!code.empty()) {
    for (std::list<Token>::iterator i = code.begin(); i != code.end(); i++)
    {
      switch (i->type)
      {
        case Token::Identifier:
          if ((++i)->data == "(")
          {
            program.nodes.emplace_back(parseFunctionDeclaration(code));
            i = code.end();
          }
          i--;
          break;
        case Token::Operator:
          if (i->data == "=")
          {
            program.nodes.emplace_back(parseVariableDeclaration(code));
            i = code.end();
          }
          break;
        case Token::Other:
          if (i->data == ";")
          {
            program.nodes.emplace_back(parseVariableDeclaration(code));
            i = code.end();
          }
          break;
        /*default:
          std::cout << "Parse error: Expected DataType, received " << Token::typeStrings[i->type] << "\n";
          break;*/
      }
    }
  }

  return program;
}

FunctionDeclaration* parseFunctionDeclaration(std::list<Token>& code)
{
  FunctionDeclaration* functionDeclaration = new FunctionDeclaration;

  functionDeclaration->returnType = std::unique_ptr<DataType>(parseDataType(code));

  parseExpect(code.front(), Token::Identifier);
  functionDeclaration->identifier = code.front().data;
  code.pop_front();

  parseExpect(code.front().data, "(");
  code.pop_front();

  if (code.front().data != ")")
  {
    parseExpect(code.front().data, "void");
    code.pop_front();
  }

  parseExpect(code.front().data, ")");
  code.pop_front();

  parseExpect(code.front().data, {"{", ";"});
  if (code.front().data == "{")
  {
    functionDeclaration->body = std::unique_ptr<CompoundStatement>(parseCompoundStatement(code));
  }

  return functionDeclaration;
}

Statement* parseStatement(std::list<Token>& code, bool canParseVariableDeclarations)
{
  Statement* statement = nullptr;

  if (code.front().type == Token::Keyword)
  {
    if (code.front().data == "return")
    {
      statement = parseReturn(code);
    } else if (code.front().data == "break")
    {
      statement = parseBreak(code);
    } else if (code.front().data == "continue")
    {
      statement = parseContinue(code);
    } else if (code.front().data == "goto")
    {
      statement = parseGoto(code);
    } else if (code.front().data == "if")
    {
      statement = parseIfConditional(code);
    } else if (code.front().data == "case")
    {
      statement = parseSwitchCase(code);
    } else if (code.front().data == "default")
    {
      statement = parseSwitchDefault(code);
    } else if (code.front().data == "switch")
    {
      statement = parseSwitchConditional(code);
    } else if (code.front().data == "do")
    {
      statement = parseDoWhileLoop(code);
    } else if (code.front().data == "while")
    {
      statement = parseWhileLoop(code);
    } else if (code.front().data == "for")
    {
      statement = parseForLoop(code);
    } else if (
      canParseVariableDeclarations && (
      code.front().data == "signed" ||
      code.front().data == "unsigned" ||
      code.front().data == "static" ||
      code.front().data == "const" ||
      code.front().data == "char" ||
      code.front().data == "short" ||
      code.front().data == "int" ||
      code.front().data == "long" ||
      code.front().data == "float" ||
      code.front().data == "double"))
    {
      statement = parseVariableDeclaration(code);
    }
  } else if (code.front().type == Token::Identifier && (++code.begin())->data == ":")
  {
    statement = parseLabel(code);
  } else if (code.front().data == "{")
  {
    statement = parseCompoundStatement(code);
  } else
  {
    statement = parseExpression(code);

    parseExpect(code.front().data, ";");
    code.pop_front();
  }

  if (statement == nullptr)
  {
    std::cout << "Parse error: Expected a statement\n";
    throw ParseError();
  }

  return statement;
}

CompoundStatement* parseCompoundStatement(std::list<Token>& code)
{
  CompoundStatement* compoundStatement = new CompoundStatement;

  parseExpect(code.front().data, "{");
  code.pop_front();

  while (code.front().data != "}")
  {
    compoundStatement->body.emplace_back(parseStatement(code, true));

    if (code.empty())
    {
      std::cout << "Parse error: Unexpected End of File\n";
      throw ParseError();
    }
  }
  code.pop_front();

  return compoundStatement;
}

Label* parseLabel(std::list<Token>& code)
{
  Label* label = new Label;

  parseExpect(code.front(), Token::Identifier);
  label->name = code.front().data;
  code.pop_front();

  parseExpect(code.front().data, ":");
  code.pop_front();

  return label;
}

Return* parseReturn(std::list<Token>& code)
{
  Return* returnVal = new Return;

  parseExpect(code.front().data, "return");
  code.pop_front();

  returnVal->data = std::unique_ptr<Expression>(parseExpression(code));

  parseExpect(code.front().data, ";");
  code.pop_front();

  return returnVal;
}

Break* parseBreak(std::list<Token>& code)
{
  Break* breakStatement = new Break;

  parseExpect(code.front().data, "break");
  code.pop_front();

  parseExpect(code.front().data, ";");
  code.pop_front();

  return breakStatement;
}

Continue* parseContinue(std::list<Token>& code)
{
  Continue* continueStatement = new Continue;

  parseExpect(code.front().data, "continue");
  code.pop_front();

  parseExpect(code.front().data, ";");
  code.pop_front();

  return continueStatement;
}

Goto* parseGoto(std::list<Token>& code)
{
  Goto* gotoStatement = new Goto;

  parseExpect(code.front().data, "goto");
  code.pop_front();

  parseExpect(code.front(), Token::Identifier);
  gotoStatement->label = code.front().data;
  code.pop_front();

  parseExpect(code.front().data, ";");
  code.pop_front();

  return gotoStatement;
}

FunctionCall* parseFunctionCall(std::list<Token>& code)
{
  FunctionCall* functionCall = new FunctionCall;

  parseExpect(code.front(), Token::Identifier);
  functionCall->identifier = code.front().data;
  code.pop_front();

  parseExpect(code.front().data, "(");
  code.pop_front();

  while (code.front().data != ")")
  {
    code.pop_front();
  }
  code.pop_front();

  parseExpect(code.front().data, ";");
  code.pop_front();

  return functionCall;
}

IfConditional* parseIfConditional(std::list<Token>& code)
{
  IfConditional* ifConditional = new IfConditional;

  parseExpect(code.front().data, "if");
  code.pop_front();

  parseExpect(code.front().data, "(");
  code.pop_front();

  ifConditional->condition = std::unique_ptr<Expression>(parseExpression(code, false));

  parseExpect(code.front().data, ")");
  code.pop_front();

  ifConditional->body = std::unique_ptr<Statement>(parseStatement(code));

  if (code.front().data == "else")
  {
    code.pop_front();
    ifConditional->elseStatement = std::unique_ptr<Statement>(parseStatement(code));
  }

  return ifConditional;
}

SwitchCase* parseSwitchCase(std::list<Token>& code)
{
  SwitchCase* switchCase = new SwitchCase;

  parseExpect(code.front().data, "case");
  code.pop_front();

  switchCase->requirement = std::unique_ptr<Expression>(parseExpression(code, false));

  parseExpect(code.front().data, ":");
  code.pop_front();

  return switchCase;
}

SwitchDefault* parseSwitchDefault(std::list<Token>& code)
{
  SwitchDefault* switchDefault = new SwitchDefault;

  parseExpect(code.front().data, "default");
  code.pop_front();

  parseExpect(code.front().data, ":");
  code.pop_front();

  return switchDefault;
}

SwitchConditional* parseSwitchConditional(std::list<Token>& code)
{
  SwitchConditional* switchConditional = new SwitchConditional;

  parseExpect(code.front().data, "switch");
  code.pop_front();

  parseExpect(code.front().data, "(");
  code.pop_front();

  switchConditional->value = std::unique_ptr<Expression>(parseExpression(code, false));

  parseExpect(code.front().data, ")");
  code.pop_front();

  switchConditional->body = std::unique_ptr<Statement>(parseStatement(code));

  return switchConditional;
}

DoWhileLoop* parseDoWhileLoop(std::list<Token>& code)
{
  DoWhileLoop* doWhileLoop = new DoWhileLoop;

  parseExpect(code.front().data, "do");
  code.pop_front();

  doWhileLoop->body = std::unique_ptr<Statement>(parseStatement(code));

  parseExpect(code.front().data, "while");
  code.pop_front();

  parseExpect(code.front().data, "(");
  code.pop_front();

  doWhileLoop->condition = std::unique_ptr<Expression>(parseExpression(code, false));

  parseExpect(code.front().data, ")");
  code.pop_front();

  parseExpect(code.front().data, ";");
  code.pop_front();

  return doWhileLoop;
}

WhileLoop* parseWhileLoop(std::list<Token>& code)
{
  WhileLoop* whileLoop = new WhileLoop;

  parseExpect(code.front().data, "while");
  code.pop_front();

  parseExpect(code.front().data, "(");
  code.pop_front();

  whileLoop->condition = std::unique_ptr<Expression>(parseExpression(code, false));

  parseExpect(code.front().data, ")");
  code.pop_front();

  whileLoop->body = std::unique_ptr<Statement>(parseStatement(code));

  return whileLoop;
}

ForLoop* parseForLoop(std::list<Token>& code)
{
  ForLoop* forLoop = new ForLoop;

  parseExpect(code.front().data, "for");
  code.pop_front();

  parseExpect(code.front().data, "(");
  code.pop_front();

  // parseVariableDeclaration() removes semicolon
  forLoop->initialization = std::unique_ptr<VariableDeclaration>(parseVariableDeclaration(code));

  forLoop->condition = std::unique_ptr<Expression>(parseExpression(code));

  parseExpect(code.front().data, ";");
  code.pop_front();

  forLoop->update = std::unique_ptr<Expression>(parseExpression(code));

  parseExpect(code.front().data, ")");
  code.pop_front();

  forLoop->body = std::unique_ptr<Statement>(parseStatement(code));

  return forLoop;
}

Expression* parseExpression(std::list<Token>& code, bool allowNullExpression)
{
  Expression* expression = nullptr;
  
  if (code.front().data == "(" && (code.begin()++)->type != Token::Keyword)
  {
    code.pop_front();

    expression = new SubExpression;
    
    ((SubExpression*)expression)->expression = std::unique_ptr<Expression>(parseExpression(code, false));

    parseExpect(code.front().data, ")");
    code.pop_front();
  } else if (code.front().type == Token::Constant)
  {
    expression = parseConstant(code);
  } else if (code.front().type == Token::Identifier)
  {
    if ((code.begin()++)->data == "(")
    {
      expression = parseFunctionCall(code);
    } else
    {
      expression = parseVariableAccess(code);
    }
  } else if (
    code.front().type == Token::Keyword ||
    code.front().data == "(" && (code.begin()++)->type != Token::Keyword ||
    code.front().data == "&" ||
    code.front().data == "*" ||
    code.front().data == "-" ||
    code.front().data == "~" ||
    code.front().data == "!" ||
    code.front().data == "++" ||
    code.front().data == "--")
  {
    expression = parsePreUnary(code);
    if (optimize && ((PreUnaryOperator*)expression)->operand.get()->expressionType == Expression::ExpressionType::Constant)
    {
      Constant* constant = (Constant*)(((PreUnaryOperator*)expression)->operand.get());
      switch (((PreUnaryOperator*)expression)->preUnaryType)
      {
        case PreUnaryOperator::PreUnaryType::MathematicNegate:
          ((PreUnaryOperator*)expression)->operand.release();
          delete expression;
          constant->value.unsignedChar = -constant->value.unsignedChar;
          expression = constant;
          break;
        case PreUnaryOperator::PreUnaryType::LogicalNegate:
          ((PreUnaryOperator*)expression)->operand.release();
          delete expression;
          constant->value.unsignedChar = !constant->value.unsignedChar;
          expression = constant;
          break;
        case PreUnaryOperator::PreUnaryType::BitwiseNOT:
          ((PreUnaryOperator*)expression)->operand.release();
          delete expression;
          constant->value.unsignedChar = ~constant->value.unsignedChar;
          expression = constant;
          break;
        default:
          break;
      }
    }
  } else if (allowNullExpression && (code.front().data == ")" || code.front().data == ";"))
  {
    expression = new Expression;
    expression->expressionType = Expression::ExpressionType::Null;
  } else
  {
    std::cout << "Parse error: Expected an expression, received \"" << code.front().data << "\"\n";
    throw ParseError();
  }

  // separate prefixes and suffixes into seperate if statements
  if (code.front().data == ")")
  {
    return expression;
  } else if (
    code.front().data == "+" ||
    code.front().data == "-" ||
    code.front().data == "*" ||
    code.front().data == "/" ||
    code.front().data == "%" ||
    code.front().data == "<<" ||
    code.front().data == ">>" ||
    code.front().data == "|" ||
    code.front().data == "&" ||
    code.front().data == "||" ||
    code.front().data == "&&" ||
    code.front().data == "[" ||
    code.front().data == "+=" ||
    code.front().data == "-=" ||
    code.front().data == "*=" ||
    code.front().data == "/=" ||
    code.front().data == "%=" ||
    code.front().data == "<<=" ||
    code.front().data == ">>=" ||
    code.front().data == "|=" ||
    code.front().data == "&=" ||
    code.front().data == "^=" ||
    code.front().data == "=" ||
    code.front().data == ">" ||
    code.front().data == "<" ||
    code.front().data == ">=" ||
    code.front().data == "<=" ||
    code.front().data == "==" ||
    code.front().data == "!=" ||
    code.front().data == "^")
  {
    expression = parseBinary(code, expression);

  } else if (
    code.front().data == "++" ||
    code.front().data == "--")
  {
    expression = parsePostUnary(code, expression);
  } else if (code.front().data == "?")
  {
    expression = parseTernary(code, expression);
  }

  return expression;
}

Expression* parsePreUnary(std::list<Token>& code)
{
  Expression* preUnary = new PreUnaryOperator;

  if (code.front().type == Token::Keyword || code.front().data == "(")
  {
    preUnary = parseTypeCast(code);
  } else if (code.front().data == "&")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Address;
  } else if (code.front().data == "*")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Dereference;
  } else if (code.front().data == "-")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::MathematicNegate;
  } else if (code.front().data == "~")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::BitwiseNOT;
  } else if (code.front().data == "!")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::LogicalNegate;
  } else if (code.front().data == "++")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Increment;
  } else if (code.front().data == "--")
  {
    ((PreUnaryOperator*)preUnary)->preUnaryType = PreUnaryOperator::PreUnaryType::Decrement;
  }

  code.pop_front();

  ((PreUnaryOperator*)preUnary)->operand = std::unique_ptr<Expression>(parseExpression(code, false));

  if (((PreUnaryOperator*)preUnary)->operand->expressionType == Expression::ExpressionType::BinaryOperator)
  {
    BinaryOperator* bottomOperand = (BinaryOperator*)((PreUnaryOperator*)preUnary)->operand.get();
    while (
      bottomOperand->leftOperand->expressionType == Expression::ExpressionType::BinaryOperator)
    {
      bottomOperand = (BinaryOperator*)bottomOperand->leftOperand.get();
    }
    Expression* operand = ((PreUnaryOperator*)preUnary)->operand.release();
    ((PreUnaryOperator*)preUnary)->operand = std::unique_ptr<Expression>(bottomOperand->leftOperand.release());
    bottomOperand->leftOperand = std::unique_ptr<Expression>(preUnary);
    preUnary = operand;
  }

  return preUnary;
}

PostUnaryOperator* parsePostUnary(std::list<Token>& code, Expression* operand)
{
  PostUnaryOperator* postUnary = new PostUnaryOperator;

  if (code.front().data == "++")
  {
    postUnary->postUnaryType = PostUnaryOperator::PostUnaryType::Increment;
  } else if (code.front().data == "--")
  {
    postUnary->postUnaryType = PostUnaryOperator::PostUnaryType::Decrement;
  }

  code.pop_front();

  postUnary->operand = std::unique_ptr<Expression>(operand);

  return postUnary;
}

TypeCast* parseTypeCast(std::list<Token>& code)
{
  TypeCast* typeCast = new TypeCast;

  if (code.front().data == "(")
  {
    code.pop_front();
  }

  typeCast->dataType = std::unique_ptr<DataType>(parseDataType(code));

  if (code.front().data == ")")
  {
    code.pop_front();
  }

  return typeCast;
}

Expression* parseBinary(std::list<Token>& code, Expression* leftOperand)
{
  Expression* binary = new BinaryOperator;

  ((BinaryOperator*)binary)->leftOperand = std::unique_ptr<Expression>(leftOperand);

  if (code.front().data == "+")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Add;
  } else if (code.front().data == "-")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Subtract;
  } else if (code.front().data == "*")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Multiply;
  } else if (code.front().data == "/")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Divide;
  } else if (code.front().data == "%")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Modulo;
  } else if (code.front().data == "<<")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::LeftShift;
  } else if (code.front().data == ">>")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::RightShift;
  } else if (code.front().data == "|")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseOR;
  } else if (code.front().data == "&")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseAND;
  } else if (code.front().data == "^")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseXOR;
  } else if (code.front().data == "||")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::LogicalOR;
  } else if (code.front().data == "&&")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::LogicalAND;
  } else if (code.front().data == "[")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Subscript;
  } else if (code.front().data == "=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::VariableAssignment;
  } else if (code.front().data == "+=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::AddEqual;
  } else if (code.front().data == "-=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::SubtractEqual;
  } else if (code.front().data == "*=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::MultiplyEqual;
  } else if (code.front().data == "/=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::DivideEqual;
  } else if (code.front().data == "%=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::ModuloEqual;
  } else if (code.front().data == "<<=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::LeftShiftEqual;
  } else if (code.front().data == ">>=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::RightShiftEqual;
  } else if (code.front().data == "|=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseOREqual;
  } else if (code.front().data == "&=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseANDEqual;
  } else if (code.front().data == "^=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::BitwiseXOREqual;
  } else if (code.front().data == "==")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Equal;
  } else if (code.front().data == "!=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::NotEqual;
  } else if (code.front().data == ">")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Greater;
  } else if (code.front().data == "<")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::Lesser;
  } else if (code.front().data == ">=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::GreaterOrEqual;
  } else if (code.front().data == "<=")
  {
    ((BinaryOperator*)binary)->binaryType = BinaryOperator::BinaryType::LesserOrEqual;
  }

  code.pop_front();

  ((BinaryOperator*)binary)->rightOperand = std::unique_ptr<Expression>(parseExpression(code, false));

  if (((BinaryOperator*)binary)->binaryType == BinaryOperator::BinaryType::Subscript)
  {
    parseExpect(code.front().data, "]");

    code.pop_front();

    return binary;
  }

  // this takes care of left-associativity and operator precedence
  if (((BinaryOperator*)binary)->rightOperand->expressionType == Expression::ExpressionType::BinaryOperator)
  {
    BinaryOperator* operand = (BinaryOperator*)((BinaryOperator*)binary)->rightOperand.get();
    if (BinaryOperator::precedence[(uint8_t)operand->binaryType] < BinaryOperator::precedence[(uint8_t)((BinaryOperator*)binary)->binaryType])
    {
      ((BinaryOperator*)binary)->rightOperand.release();
      ((BinaryOperator*)binary)->rightOperand = std::unique_ptr<Expression>(operand->leftOperand.release());
      operand->leftOperand = std::unique_ptr<Expression>(binary);
      binary = operand;
    } else if (BinaryOperator::precedence[(uint8_t)operand->binaryType] == BinaryOperator::precedence[(uint8_t)((BinaryOperator*)binary)->binaryType] && ((BinaryOperator*)binary)->binaryType != BinaryOperator::BinaryType::VariableAssignment)
    {
      BinaryOperator* bottomOperand = operand;
      while (
        bottomOperand->rightOperand->expressionType == Expression::ExpressionType::BinaryOperator &&
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)bottomOperand->rightOperand.get())->binaryType] ==
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)binary)->binaryType] ||
        bottomOperand->leftOperand->expressionType == Expression::ExpressionType::BinaryOperator &&
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)bottomOperand->leftOperand.get())->binaryType] ==
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)binary)->binaryType])
      {
        if (bottomOperand->rightOperand->expressionType == Expression::ExpressionType::BinaryOperator)
        {
          bottomOperand = (BinaryOperator*)bottomOperand->rightOperand.get();
        } else
        {
          bottomOperand = (BinaryOperator*)bottomOperand->leftOperand.get();
        }
      }
      ((BinaryOperator*)binary)->rightOperand.release();
      ((BinaryOperator*)binary)->rightOperand = std::unique_ptr<Expression>(bottomOperand->leftOperand.release());
      bottomOperand->leftOperand = std::unique_ptr<Expression>(binary);
      binary = operand;
    }
  } else if (((BinaryOperator*)binary)->binaryType != BinaryOperator::BinaryType::VariableAssignment && ((BinaryOperator*)binary)->rightOperand->expressionType == Expression::ExpressionType::TernaryOperator)
  {
    TernaryOperator* operand = (TernaryOperator*)((BinaryOperator*)binary)->rightOperand.get();
    ((BinaryOperator*)binary)->rightOperand.release();
    ((BinaryOperator*)binary)->rightOperand = std::unique_ptr<Expression>(operand->condition.release());
    operand->condition = std::unique_ptr<Expression>(binary);
    binary = operand;
  }

  return binary;
}

Expression* parseTernary(std::list<Token>& code, Expression* condition)
{
  Expression* ternary = new TernaryOperator;

  ((TernaryOperator*)ternary)->condition = std::unique_ptr<Expression>(condition);

  code.pop_front();

  ((TernaryOperator*)ternary)->trueOperand = std::unique_ptr<Expression>(parseExpression(code));

  parseExpect(code.front().data, ":");

  code.pop_front();

  ((TernaryOperator*)ternary)->falseOperand = std::unique_ptr<Expression>(parseExpression(code, false));

  if (
    ((TernaryOperator*)ternary)->falseOperand->expressionType == Expression::ExpressionType::BinaryOperator &&
    ((BinaryOperator*)((TernaryOperator*)ternary)->falseOperand.get())->binaryType == BinaryOperator::BinaryType::VariableAssignment)
  {
    std::cout << "Parse error: cannot use ternary operator as lvalue\n";
    throw ParseError();
  }

  // this takes care of left-associativity and operator precedence
  if (((TernaryOperator*)ternary)->condition->expressionType == Expression::ExpressionType::BinaryOperator)
  {
    BinaryOperator* operand = (BinaryOperator*)((TernaryOperator*)ternary)->condition.get();
    if (operand->binaryType == BinaryOperator::BinaryType::VariableAssignment)
    {
      ((TernaryOperator*)ternary)->condition.release();
      ((TernaryOperator*)ternary)->condition = std::unique_ptr<Expression>(operand->rightOperand.release());
      operand->rightOperand = std::unique_ptr<Expression>(ternary);
      ternary = operand;
    }/* else if (BinaryOperator::precedence[(uint8_t)operand->binaryType] == BinaryOperator::precedence[(uint8_t)binary->binaryType] && binary->binaryType != BinaryOperator::BinaryType::VariableAssignment)
    {
      BinaryOperator* bottomOperand = operand;
      while (
        bottomOperand->rightOperand->expressionType == Expression::ExpressionType::BinaryOperator &&
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)bottomOperand->rightOperand.get())->binaryType] ==
        BinaryOperator::precedence[(uint8_t)binary->binaryType] ||
        bottomOperand->leftOperand->expressionType == Expression::ExpressionType::BinaryOperator &&
        BinaryOperator::precedence[(uint8_t)((BinaryOperator*)bottomOperand->leftOperand.get())->binaryType] ==
        BinaryOperator::precedence[(uint8_t)binary->binaryType])
      {
        if (bottomOperand->rightOperand->expressionType == Expression::ExpressionType::BinaryOperator)
        {
          bottomOperand = (BinaryOperator*)bottomOperand->rightOperand.get();
        } else
        {
          bottomOperand = (BinaryOperator*)bottomOperand->leftOperand.get();
        }
      }
      binary->rightOperand.release();
      binary->rightOperand = std::unique_ptr<Expression>(bottomOperand->leftOperand.release());
      bottomOperand->leftOperand = std::unique_ptr<Expression>(binary);
      binary = operand;
    }*/
  }

  return ternary;
}

Constant* parseConstant(std::list<Token>& code)
{
  Constant* constant = new Constant;
  PrimitiveType* constantType = new PrimitiveType;

  if (code.front().data.front() == '\'' && code.front().data.back() == '\'')
  { // char constant
    constantType->type = PrimitiveType::Type::SignedChar;
    constant->value.signedChar = code.front().data[1];
    code.pop_front();
  } else if (code.front().data.front() == '\"' && code.front().data.back() == '\"')
  { // str constant

  } else if (code.front().data.front() >= '0' && code.front().data.front() <= '9')
  { // int constant
    if (code.front().data.back() == 'L')
    { // long int
      if (code.front().data[code.front().data.size()-2] == 'U')
      { // unsigned long int

      } else
      { // long int

      }
    } else if (code.front().data.back() == 'U')
    { // unsigned int
      int constVal = std::stoi(code.front().data);
      code.pop_front();

      if (constVal == (constVal & 0xFF))
      {
        constantType->type = PrimitiveType::Type::UnsignedChar;
        constant->value.unsignedChar = constVal;
      } else if (constVal == (constVal & 0xFFFF))
      {
        constantType->type = PrimitiveType::Type::UnsignedShort;
        constant->value.unsignedShort = constVal;
      } else if (constVal == (constVal & 0xFFFFFFFF))
      {
        constantType->type = PrimitiveType::Type::UnsignedLong;
        constant->value.unsignedLong = constVal;
      } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
      {
        constantType->type = PrimitiveType::Type::UnsignedLongLong;
        constant->value.unsignedLongLong = constVal;
      }
    } else
    { // signed int
      int constVal = std::stoi(code.front().data);
      code.pop_front();

      if (constVal < 0)
      {
        if (-constVal == -(constVal & 0xFF))
        {
          constantType->type = PrimitiveType::Type::SignedChar;
          constant->value.signedChar = constVal;
        } else if (-constVal == -(constVal & 0xFFFF))
        {
          constantType->type = PrimitiveType::Type::SignedShort;
          constant->value.signedShort = constVal;
        } else if (-constVal == -(constVal & 0xFFFFFFFF))
        {
          constantType->type = PrimitiveType::Type::SignedLong;
          constant->value.signedLong = constVal;
        } else if (-constVal == -(constVal & 0xFFFFFFFFFFFFFFFF))
        {
          constantType->type = PrimitiveType::Type::SignedLongLong;
          constant->value.signedLongLong = constVal;
        }
      } else
      {
        if (constVal == (constVal & 0xFF))
        {
          constantType->type = PrimitiveType::Type::UnsignedChar;
          constant->value.unsignedChar = constVal;
        } else if (constVal == (constVal & 0xFFFF))
        {
          constantType->type = PrimitiveType::Type::UnsignedShort;
          constant->value.unsignedShort = constVal;
        } else if (constVal == (constVal & 0xFFFFFFFF))
        {
          constantType->type = PrimitiveType::Type::UnsignedLong;
          constant->value.unsignedLong = constVal;
        } else if (constVal == (constVal & 0xFFFFFFFFFFFFFFFF))
        {
          constantType->type = PrimitiveType::Type::UnsignedLongLong;
          constant->value.unsignedLongLong = constVal;
        }
      }
    }
  }

  constant->dataType = std::unique_ptr<DataType>(constantType);

  return constant;
}

VariableAccess* parseVariableAccess(std::list<Token>& code)
{
  VariableAccess* variableAccess = new VariableAccess;

  variableAccess->identifier = code.front().data;
  code.pop_front();

  return variableAccess;
}

DataType* parseDataType(std::list<Token>& code)
{
  DataType* dataType = nullptr;

  PrimitiveType* primitiveType = new PrimitiveType;

  if (code.front().data == "volatile")
  {
    primitiveType->isVolatile = true;
    code.pop_front();
  }

  switch (parseExpect(code.front().data, {"void", "signed", "unsigned", "short", "long", "char", "int", "float", "double"})) {
    case 0:
      primitiveType->type = PrimitiveType::Type::Void;
      code.pop_front();
      break;
    case 1:
      code.pop_front();
      switch (parseExpect(code.front().data, {"short", "long", "char", "int"})) {
        case 0:
          primitiveType->type = PrimitiveType::Type::SignedShort;
          code.pop_front();
          if (code.front().data == "int")
          {
            code.pop_front();
          }
          break;
        case 1:
          code.pop_front();
          if (code.front().data == "long")
          {
            primitiveType->type = PrimitiveType::Type::SignedLongLong;
            code.pop_front();
          } else
          {
            primitiveType->type = PrimitiveType::Type::SignedLong;
          }
          if (code.front().data == "int")
          {
            code.pop_front();
          }
          break;
        case 2:
          primitiveType->type = PrimitiveType::Type::SignedChar;
          code.pop_front();
          break;
        case 3:
          primitiveType->type = PrimitiveType::Type::SignedInt;
          code.pop_front();
          break;
        default:

          break;
      }
      break;
    case 2:
      code.pop_front();
      switch (parseExpect(code.front().data, {"short", "long", "char", "int"}))
      {
        case 0:
          primitiveType->type = PrimitiveType::Type::UnsignedShort;
          code.pop_front();
          if (code.front().data == "int")
          {
            code.pop_front();
          }
          break;
        case 1:
          code.pop_front();
          if (code.front().data == "long")
          {
            primitiveType->type = PrimitiveType::Type::UnsignedLongLong;
            code.pop_front();
          } else {
            primitiveType->type = PrimitiveType::Type::UnsignedLong;
          }
          if (code.front().data == "int")
          {
            code.pop_front();
          }
          break;
        case 2:
          primitiveType->type = PrimitiveType::Type::UnsignedChar;
          code.pop_front();
          break;
        case 3:
          primitiveType->type = PrimitiveType::Type::UnsignedInt;
          code.pop_front();
          break;
        default:

          break;
      }
      break;
    case 3:
      primitiveType->type = PrimitiveType::Type::SignedShort;
      code.pop_front();
      if (code.front().data == "int")
      {
        code.pop_front();
      }
      break;
    case 4:
      code.pop_front();
      if (code.front().data == "long")
      {
        primitiveType->type = PrimitiveType::Type::SignedLongLong;
        code.pop_front();
      } else
      {
        primitiveType->type = PrimitiveType::Type::SignedLong;
      }
      if (code.front().data == "int")
      {
        code.pop_front();
      }
      break;
    case 5:
      primitiveType->type = PrimitiveType::Type::SignedChar;
      code.pop_front();
      break;
    case 6:
      primitiveType->type = PrimitiveType::Type::SignedInt;
      code.pop_front();
      break;
    case 7:
      primitiveType->type = PrimitiveType::Type::Float;
      code.pop_front();
      break;
    case 8:
      primitiveType->type = PrimitiveType::Type::Double;
      code.pop_front();
      break;
    default:

      break;
  }
  dataType = primitiveType;

  while (code.front().data == "*")
  {
    Pointer* pointer = new Pointer;
    pointer->dataType = std::unique_ptr<DataType>(dataType);
    dataType = pointer;
    code.pop_front();
  }

  Token variableIdentifier;
  if ((code.front().type == Token::Identifier && (++code.begin())->data == "["))
  {
    variableIdentifier = code.front();
    code.pop_front();
  }

  while (code.front().data == "[")
  {
    Array* array = new Array;
    array->dataType = std::unique_ptr<DataType>(dataType);
    dataType = array;

    code.pop_front();
    array->size = std::unique_ptr<Expression>(parseExpression(code));

    parseExpect(code.front().data, "]");
    code.pop_front();
  }

  if (variableIdentifier.type == Token::Identifier)
  {
    code.push_front(variableIdentifier);
  }

  return dataType;
}

VariableDeclaration* parseVariableDeclaration(std::list<Token>& code)
{
  VariableDeclaration* variableDeclaration = new VariableDeclaration;

  variableDeclaration->dataType = std::unique_ptr<DataType>(parseDataType(code));

  parseExpect(code.front(), Token::Identifier);
  variableDeclaration->identifier = code.front().data;
  code.pop_front();

  if (code.front().data == "=")
  {
    code.pop_front();
    variableDeclaration->value = std::unique_ptr<Expression>(parseExpression(code, false));
  }

  parseExpect(code.front().data, ";");
  code.pop_front();

  return variableDeclaration;
}
