#include "print_IR.hpp"

#include <sstream>

std::string printIR(const std::vector<Operation>& asmCode)
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