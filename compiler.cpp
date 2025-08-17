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

Compiler::Compiler()
{

}

Compiler::Compiler(int argc, char* argv[], IRprogram& irCode)
{
  irCode = compileFromArgs(argc, argv);
}

IRprogram Compiler::compileFromArgs(int argc, char* argv[])
{
  handleArgs(argc, argv);

  IRprogram irCode;

  for (std::string& inputFilename: inputFilenames)
  {
    std::string fileText = loadFile(inputFilename);

    if (doPreprocess && inputFilename.find(".s") == std::string::npos && inputFilename.find(".i") == std::string::npos)
    {
      // updates filetext
      Preprocessor preprocessor(inputFilename.substr(0, inputFilename.find_last_of('/')+1), fileText, includeDirs);
      //std::cout << "Preprocessed:\n" << fileText << '\n';
    }

    if (doCompile && inputFilename.find(".s") == std::string::npos)
    {
      std::list<Token> code = lex(fileText);
      /*std::cout << "Tokens:\n";

      for (const Token& token: code)
      {
        std::cout << Token::typeStrings[token.type] << ": \"" << token.data << "\"\n";
      }*/
    
      Program AST(code, typeSizes);

      optimizeAST(AST);
    
      //std::cout << "AST:\n";
      //PrintAST printer(AST);

      GenerateIR irEngine;
      IRprogram localIR = irEngine.generateIR(AST, typeSizes.pointerSize);
      irCode.program.insert(irCode.program.cend(), localIR.program.cbegin(), localIR.program.cend());

      //fileText = printIR(irCode);
      //std::cout << "Intermediate Representation:\n" << fileText << '\n';
    }
  }

  if (optimize)
  {
    GenerateIR irEngine;
    irEngine.optimizeIR(irCode);
  }

  return irCode;
}

int Compiler::handleArgs(int argc, char* argv[])
{
  for (int arg = 0; arg < argc; arg++)
  {
    std::string argStr = argv[arg];

    if (argStr == "-o" && arg < argc-1)
    {
      arg++;
      outputFilename = argv[arg];
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
    } else if (argStr == "-O0")
    {
      optimize = false;
    } else if (argStr == "-O" || argStr == "-O1" || argStr == "-O2" || argStr == "-O3")
    {
      optimize = true;
    } else if (argStr.find(".s") != argStr.npos || argStr.find(".c") != argStr.npos || argStr.find(".i") != argStr.npos)
    {
      inputFilenames.emplace_back("./"+argStr);
    }/* else if (argStr.find(".h") != argStr.npos) add precompiled headers
    {
      doPreprocess = true;
      inputFilename = argStr;
    }*/

  }

  if (inputFilenames.empty())
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
  } while (changed);
}

std::string Compiler::printIR(const IRprogram& irCode)
{
  std::stringstream fileData;

  for (const IRprogram::Function& function: irCode.program)
  {
    fileData << "\nFunction:\n";
    for (const Operation& operation: function.body)
    {
      if (operation.type.isFloating)
      {
        fileData << 'f';
      } else if (operation.type.isSigned)
      {
        fileData << 'i';
      } else if (!operation.type.identifier.empty())
      {
        fileData << "struct " << operation.type.identifier << ':';
      } else
      {
        fileData << 'u';
      }

      if (operation.type.arrayLength > 0)
      {
        fileData << operation.type.size << ':' << (int)operation.type.alignment << std::string(operation.type.pointerDepth-1, '*');
        fileData << '[' << operation.type.arrayLength << ']';
      } else
      {
        fileData << operation.type.size << ':' << (int)operation.type.alignment << std::string(operation.type.pointerDepth, '*');
      }

      fileData << ' ';

      switch (operation.code)
      {
        case Operation::Set:
          fileData << operation.operands[0] << " = " << operation.operands[1] << '\n';
          break;
        case Operation::GetAddress:
          fileData << operation.operands[0] << " = &" << operation.operands[1] << '\n';
          break;
        case Operation::DereferenceRValue:
          fileData << operation.operands[0] << " = *" << operation.operands[1] << '\n';
          break;
        case Operation::DereferenceLValue:
          fileData << "*" << operation.operands[0] << " = " << operation.operands[1] << '\n';
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
        case Operation::AddArg:
          fileData << "AddArg " << operation.operands[0] << '\n';
          break;
        case Operation::Call:
          fileData << "Call " << operation.operands[0] << " -> " << operation.operands[1] << '\n';
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
  }

  return fileData.str();
}
