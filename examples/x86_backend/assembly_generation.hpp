#ifndef EXAMPLES_X86_BACKEND_ASSEMBLY_GENERATION_HPP
#define EXAMPLES_X86_BACKEND_ASSEMBLY_GENERATION_HPP

#include <array>
#include <vector>
#include <string>

#include <generate_IR.hpp>

std::string generateRegisterName(uint8_t index, uint8_t size);

constexpr std::string getSuffix(uint8_t size);

std::vector<std::array<std::string, 3>> generateASM(const IRprogram& irCode, std::string& outputFilename)
{
  std::vector<std::array<std::string, 3>> asmCode = {
    {".file", "\""+outputFilename+"\""},
    {"section", ".text"},
    {".globl", "main"}
  };

  std::map<std::string, uint16_t> variableLocations;

  for (const IRprogram::Function& function: irCode.program)
  {
    for (const Operation& irOp: function.body)
    {
      switch (irOp.code)
      {
        case Operation::Set:

          break;
        case Operation::GetAddress:

          break;
        case Operation::DereferenceLValue:

          break;
        case Operation::DereferenceRValue:

          break;
        case Operation::SetAddition:

          break;
        case Operation::SetSubtraction:

          break;
        case Operation::SetMultiplication:

          break;
        case Operation::SetDivision:

          break;
        case Operation::SetModulo:

          break;
        case Operation::SetBitwiseAND:

          break;
        case Operation::SetBitwiseOR:

          break;
        case Operation::SetBitwiseXOR:

          break;
        case Operation::SetLeftShift:

          break;
        case Operation::SetRightShift:

          break;
        case Operation::SetLogicalAND:

          break;
        case Operation::SetLogicalOR:

          break;
        case Operation::SetEqual:

          break;
        case Operation::SetNotEqual:

          break;
        case Operation::SetGreater:

          break;
        case Operation::SetLesser:

          break;
        case Operation::SetGreaterOrEqual:

          break;
        case Operation::SetLesserOrEqual:

          break;
        case Operation::Negate:

          break;
        case Operation::LogicalNOT:

          break;
        case Operation::BitwiseNOT:

          break;
        case Operation::Label:
          asmCode.emplace_back(std::array<std::string, 3>{"l"+irOp.operands[0]+":"});
          break;
        case Operation::Return:
          if (!irOp.operands[0].empty() && irOp.operands[0][0] >= '0' && irOp.operands[0][0] <= '9' && irOp.operands[0].find('_') == std::string::npos)
          {
            asmCode.insert(asmCode.end(), {
              {"mov", generateRegisterName(0, irOp.type.size), irOp.operands[0]},
              {"ret"}
            });
          }
          break;
        case Operation::AddArg:
          /*if (irOp.operands[0].find_first_not_of(".0123456789") == std::string::npos)
          {
            asmCode.emplace_back(std::array<std::string, 3>{"push", irOp.operands[0]});
          } else
          {
            asmCode.emplace_back(std::array<std::string, 3>{"push", generateRegisterName(0, irOp.type.size)});
          }*/
          break;
        case Operation::Call:

          break;
        case Operation::Jump:

          break;
        case Operation::JumpIfZero:

          break;
        case Operation::JumpIfNotZero:

          break;
      }
    }
  }

  return asmCode;
}

std::string generateRegisterName(uint8_t index, uint8_t size)
{
  switch (size)
  {
    case 1:
      //return "%"+std::string(1, 'a'+index)+"l";
    case 2:
      return std::string(1, 'a'+index)+"x";
    case 3:
    case 4:
      //return "%e"+std::string(1, 'a'+index)+"x";
    case 5:
    case 6:
    case 7:
    case 8:
      return "r"+std::string(1, 'a'+index)+"x";
  }
  return "???";
}

constexpr std::string getSuffix(uint8_t size)
{
  switch (size)
  {
    case 1:
      //return "b";
    case 2:
      return "w";
    case 3:
    case 4:
      //return "l";
    case 5:
    case 6:
    case 7:
    case 8:
      return "q";
  }
  return "";
}

#endif // EXAMPLES_X86_BACKEND_ASSEMBLY_GENERATION_HPP
