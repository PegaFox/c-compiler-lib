#include "compiler.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <format>

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
      IRprogram localIR = irEngine.generateIR(AST, typeSizes);

      irCode.program.insert(irCode.program.cend(), localIR.program.cbegin(), localIR.program.cend());
      for (const std::pair<std::string, std::size_t>& var: localIR.staticVariables)
      {
        irCode.staticVariables[var.first] = var.second + irCode.staticData.size();
      }
      irCode.staticData.insert(irCode.staticData.end(), localIR.staticData.begin(), localIR.staticData.end());

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

        SubExpression* subExpression = (SubExpression*)(*(Expression**)node.path.back().second);

        Expression* expression = subExpression->expression;

        *((Expression**)node.path.back().second) = expression;
      }
      i++;
    }
  } while (changed);
}

std::string Compiler::printIR(const IRprogram& irCode)
{
  std::stringstream irString;

  std::map<std::size_t, std::string> staticVars;
  for (const std::pair<std::string, std::size_t>& var: irCode.staticVariables)
  {
    staticVars[var.second] = var.first;
  }

  irString << "Static Data:\n";
  for (uint16_t b = 0; b < irCode.staticData.size(); b++)
  {
    if (b == staticVars.begin()->first)
    {
      irString << "\n" << staticVars.begin()->second << ":\n";
      staticVars.erase(b);
    }

    irString << "0x" + std::format("{:X}", irCode.staticData[b]) + " ";
  }
  irString << "\n";

  for (const IRprogram::Function& function: irCode.program)
  {
    irString << "\nFunction(";

    for (std::vector<std::pair<std::string, PrimitiveType>>::const_iterator parameter = function.parameters.cbegin(); parameter != function.parameters.cend(); parameter++)
    {
      irString << printIRType(parameter->second) << " " << parameter->first;

      if (++parameter != function.parameters.cend())
      {
        irString << ", ";
      }
      parameter--;
    }

    irString << ")\n";

    for (const Operation& operation: function.body)
    {
      irString << printIRoperation(operation) << '\n';
    }
  }

  return irString.str();
}

std::string Compiler::printIRoperation(const Operation& irOperation)
{
  std::stringstream opString;

  opString << printIRType(irOperation.type);
  
  switch (irOperation.code)
  {
    case Operation::Set:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1];
      break;
    case Operation::GetAddress:
      opString << irOperation.operands[0] << " = &" << irOperation.operands[1];
      break;
    case Operation::DereferenceRValue:
      opString << irOperation.operands[0] << " = *" << irOperation.operands[1];
      break;
    case Operation::DereferenceLValue:
      opString << "*" << irOperation.operands[0] << " = " << irOperation.operands[1];
      break;
    case Operation::SetAddition:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " + " << irOperation.operands[2];
      break;
    case Operation::SetSubtraction:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " - " << irOperation.operands[2];
      break;
    case Operation::SetMultiplication:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " * " << irOperation.operands[2];
      break;
    case Operation::SetDivision:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " / " << irOperation.operands[2];
      break;
    case Operation::SetModulo:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " % " << irOperation.operands[2];
      break;
    case Operation::SetBitwiseAND:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " & " << irOperation.operands[2];
      break;
    case Operation::SetBitwiseOR:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " | " << irOperation.operands[2];
      break;
    case Operation::SetBitwiseXOR:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " ^ " << irOperation.operands[2];
      break;
    case Operation::SetLeftShift:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " << " << irOperation.operands[2];
      break;
    case Operation::SetRightShift:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " >> " << irOperation.operands[2];
      break;
    case Operation::SetLogicalAND:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " && " << irOperation.operands[2];
      break;
    case Operation::SetLogicalOR:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " || " << irOperation.operands[2];
      break;
    case Operation::SetEqual:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " == " << irOperation.operands[2];
      break;
    case Operation::SetNotEqual:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " != " << irOperation.operands[2];
      break;
    case Operation::SetGreater:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " > " << irOperation.operands[2];
      break;
    case Operation::SetLesser:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " < " << irOperation.operands[2];
      break;
    case Operation::SetGreaterOrEqual:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " >= " << irOperation.operands[2];
      break;
    case Operation::SetLesserOrEqual:
      opString << irOperation.operands[0] << " = " << irOperation.operands[1] << " <= " << irOperation.operands[2];
      break;
    case Operation::Negate:
      opString << irOperation.operands[0] << " = -" << irOperation.operands[1];
      break;
    case Operation::LogicalNOT:
      opString << irOperation.operands[0] << " = !" << irOperation.operands[1];
      break;
    case Operation::BitwiseNOT:
      opString << irOperation.operands[0] << " = ~" << irOperation.operands[1];
      break;
    case Operation::Label:
      opString << irOperation.operands[0] << ":";
      break;
    case Operation::Return:
      opString << "Return " << irOperation.operands[0];
      break;
    case Operation::AddArg:
      opString << "AddArg " << irOperation.operands[0];
      break;
    case Operation::Call:
      opString << "Call " << irOperation.operands[0] << " -> " << irOperation.operands[1];
      break;
    case Operation::Jump:
      opString << "Jump " << irOperation.operands[0];
      break;
    case Operation::JumpIfZero:
      opString << "Jump " << irOperation.operands[0] << " if " << irOperation.operands[1] << " == 0";
      break;
    case Operation::JumpIfNotZero:
      opString << "Jump " << irOperation.operands[0] << " if " << irOperation.operands[1] << " != 0";
      break;
  }

  return opString.str();
}

std::string Compiler::printIRType(const PrimitiveType& irType)
{
  std::stringstream typeString;

  if (irType.isVolatile)
  {
    typeString << "volatile ";
  }

  if (irType.isConst)
  {
    typeString << "const ";
  }

  if (irType.isFloating)
  {
    typeString << 'f';
  } else if (irType.isSigned)
  {
    typeString << 'i';
  } else
  {
    typeString << 'u';
  }

  typeString << irType.size << ':' << (int)irType.alignment << ' ';

  return typeString.str();
}
