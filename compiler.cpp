#include "compiler.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

#include "preprocessor.hpp"
#include "lexer.hpp"
#include "parser/AST_iterator.hpp"
#include "parser/sub_expression.hpp"
#include "print_AST.hpp"
#include "generate_IR.hpp"
#include "optimize_IR.hpp"

Compiler::Compiler()
{

}

Compiler::Compiler(int argc, char* argv[])
{
  compileFromArgs(argc, argv);
}

void Compiler::compileFromArgs(int argc, char* argv[])
{
  handleArgs(argc, argv);

  std::string fileText = loadFile(inputFilename);

  if (doPreprocess)
  {
    // updates filetext
    Preprocessor preprocessor(fileText, includeDirs);
    std::cout << "Preprocessed:\n" << fileText << '\n';
  }

  if (doCompile)
  {
    std::list<Token> code = lex(fileText);
    std::cout << "Tokens:\n";

    for (const Token& token: code)
    {
      std::cout << Token::typeStrings[token.type] << ": \"" << token.data << "\"\n";
    }
  
    Program AST(code, typeSizes);

    optimizeAST(AST);
  
    std::cout << "AST:\n";
    PrintAST printer(AST);

    std::vector<Operation> asmCode = generateIR(AST);

    if (optimize)
    {
      optimizeIR(asmCode);
    }

    fileText = printIR(asmCode);
    std::cout << "Intermediate Representation:\n" << fileText << '\n';
  }
}

int Compiler::handleArgs(int argc, char* argv[])
{
  for (int arg = 0; arg < argc; arg++)
  {
    std::string argStr = argv[arg];

    if (argStr == "-o" && arg < argc-1)
    {
      arg++;
      outputFilename = argStr;
    } else if (argStr.substr(0, 2) == "-I" && argStr.size() > 2)
    {
      includeDirs.push_back(argStr.substr(2));
      if (includeDirs.back().back() != '/')
      {
        includeDirs.back().push_back('/');
      }
    } else if (argStr == "-S")
    {
      doAssemble = false;
    } else if (argStr == "-E")
    {
      doCompile = false;
      doAssemble = false;
    } else if (argStr == "-O")
    {
      optimize = true;
    } else if (argStr.find(".mc1") != argStr.npos)
    {
      doPreprocess = false;
      doCompile = false;
      inputFilename = argStr;
    } else if (argStr.find(".c") != argStr.npos)
    {
      doPreprocess = true;
      inputFilename = argStr;
    } else if (argStr.find(".i") != argStr.npos)
    {
      doPreprocess = false;
      inputFilename = argStr;
    }/* else if (argStr.find(".h") != argStr.npos) add precompiled headers
    {
      doPreprocess = true;
      inputFilename = argStr;
    }*/

  }

  if (inputFilename.empty())
  {
    std::clog << "Error: No input file(s), compilation terminated\n";
    return -1;
  }

  return 0;
}

std::string Compiler::loadFile(const std::string& filename)
{
  std::ifstream inputFile(filename);

  std::stringstream fileText;
  fileText << inputFile.rdbuf();

  inputFile.close();

  return fileText.str();
}

void Compiler::optimizeAST(Program& AST)
{
  bool changed;
  do
  {
    changed = false;
    uint32_t i = 0;
    for (ASTiterator node(&AST); node != ASTiterator(nullptr); node++)
    {
      if (
        node->nodeType == ASTnode::NodeType::Statement &&
        ((Statement*)(*node))->statementType == Statement::StatementType::Expression &&
        ((Expression*)(*node))->expressionType == Expression::ExpressionType::SubExpression)
      {
        node.ptr = node.path.back().first;
        changed = true;

        SubExpression* subExpression = (SubExpression*)((std::unique_ptr<Expression>*)node.path.back().second)->release();

        Expression* expression = subExpression->expression.release();

        delete subExpression;
        *((std::unique_ptr<Expression>*)node.path.back().second) = std::unique_ptr<Expression>(expression);
      }
      i++;
    }
    std::cout << i << '\n';
  } while (changed);
}

std::string Compiler::printIR(const std::vector<Operation>& asmCode)
{
  std::stringstream fileData;

  for (const Operation& operation : asmCode)
  {
    switch (operation.code)
    {
      case Operation::Set:
        fileData << operation.operands[0] << " = " << operation.operands[1] << '\n';
        break;
      case Operation::GetAddress:
        fileData << operation.operands[0] << " = &" << operation.operands[1] << '\n';
        break;
      case Operation::Dereference:
        fileData << operation.operands[0] << " = *" << operation.operands[1] << '\n';
        break;
      case Operation::SetAddition:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " + " << operation.operands[2] << '\n';
        break;
      case Operation::SetSubtraction:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " - " << operation.operands[2] << '\n';
        break;
      case Operation::SetMultiplication:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " * " << operation.operands[2] << '\n';
        break;
      case Operation::SetDivision:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " / " << operation.operands[2] << '\n';
        break;
      case Operation::SetModulo:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " % " << operation.operands[2] << '\n';
        break;
      case Operation::SetBitwiseAND:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " & " << operation.operands[2] << '\n';
        break;
      case Operation::SetBitwiseOR:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " | " << operation.operands[2] << '\n';
        break;
      case Operation::SetBitwiseXOR:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " ^ " << operation.operands[2] << '\n';
        break;
      case Operation::SetLeftShift:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " << " << operation.operands[2] << '\n';
        break;
      case Operation::SetRightShift:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " >> " << operation.operands[2] << '\n';
        break;
      case Operation::SetLogicalAND:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " && " << operation.operands[2] << '\n';
        break;
      case Operation::SetLogicalOR:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " || " << operation.operands[2] << '\n';
        break;
      case Operation::SetEqual:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " == " << operation.operands[2] << '\n';
        break;
      case Operation::SetNotEqual:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " != " << operation.operands[2] << '\n';
        break;
      case Operation::SetGreater:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " > " << operation.operands[2] << '\n';
        break;
      case Operation::SetLesser:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " < " << operation.operands[2] << '\n';
        break;
      case Operation::SetGreaterOrEqual:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " >= " << operation.operands[2] << '\n';
        break;
      case Operation::SetLesserOrEqual:
        fileData << operation.operands[0] << " = " << operation.operands[1] << " <= " << operation.operands[2] << '\n';
        break;
      case Operation::Negate:
        fileData << operation.operands[0] << " = -" << operation.operands[1] << '\n';
        break;
      case Operation::LogicalNOT:
        fileData << operation.operands[0] << " = !" << operation.operands[1] << '\n';
        break;
      case Operation::BitwiseNOT:
        fileData << operation.operands[0] << " = ~" << operation.operands[1] << '\n';
        break;
      case Operation::Label:
        fileData << operation.operands[0] << ":\n";
        break;
      case Operation::Return:
        fileData << "Return " << operation.operands[0] << '\n';
        break;
      case Operation::Call:
        fileData << "Call " << operation.operands[0] << '\n';
        break;
      case Operation::Jump:
        fileData << "Jump " << operation.operands[0] << '\n';
        break;
      case Operation::JumpIfZero:
        fileData << "Jump " << operation.operands[0] << " if " << operation.operands[1] << " == 0\n";
        break;
      case Operation::JumpIfNotZero:
        fileData << "Jump " << operation.operands[0] << " if " << operation.operands[1] << " != 0\n";
        break;
    }
  }

  return fileData.str();
}
