#include "optimize_IR.hpp"

#include <map>

void optimizeIR(std::vector<Operation> &absCode)
{
  bool changed;

  do
  {
    changed = false;

    changed |= resolveConstantOperations(absCode);
    changed |= trimInaccessibleCode(absCode);
  } while (changed);
}

bool resolveConstantOperations(std::vector<Operation> &absCode)
{
  bool changed = false;

  std::map<std::string, std::string> vars;

  for (std::vector<Operation>::iterator i = absCode.begin(); i != absCode.end(); i++)
  {
    switch (i->code)
    {
      case Operation::Set:
        if (i->operands[1].find_first_not_of("0123456789") == std::string::npos)
        {
          vars[i->operands[0]] = i->operands[1];
        } else if (vars.contains(i->operands[1]))
        {
          i->operands[1] = vars[i->operands[1]];
          changed = true;
        }
        break;
      case Operation::Return:
        if (vars.contains(i->operands[0]))
        {
          i->operands[0] = vars[i->operands[0]];
          changed = true;
        }
        break;
      case Operation::Dereference:
      case Operation::SetAddition:
        if (i->operands[1].find_first_not_of("0123456789") == std::string::npos && i->operands[2].find_first_not_of("0123456789") == std::string::npos)
        {
          vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) + std::stoi(i->operands[2]));
        } else if (vars.contains(i->operands[1]))
        {
          i->operands[1] = vars[i->operands[1]];
          changed = true;
        } else if (vars.contains(i->operands[2]))
        {
          i->operands[2] = vars[i->operands[2]];
          changed = true;
        }
        break;
      case Operation::SetSubtraction:
      case Operation::SetMultiplication:
      case Operation::SetDivision:
      case Operation::SetModulo:
      case Operation::SetBitwiseAND:
      case Operation::SetBitwiseOR:
      case Operation::SetBitwiseXOR:
      case Operation::SetLeftShift:
        if (i->operands[1].find_first_not_of("0123456789") == std::string::npos && i->operands[2].find_first_not_of("0123456789") == std::string::npos)
        {
          vars[i->operands[0]] = std::to_string(std::stoi(i->operands[1]) << std::stoi(i->operands[2]));
        } else if (vars.contains(i->operands[1]))
        {
          i->operands[1] = vars[i->operands[1]];
          changed = true;
        } else if (vars.contains(i->operands[2]))
        {
          i->operands[2] = vars[i->operands[2]];
          changed = true;
        }
        break;
      case Operation::SetRightShift:
      case Operation::SetLogicalAND:
      case Operation::SetLogicalOR:
      case Operation::SetEqual:
      case Operation::SetNotEqual:
      case Operation::SetGreater:
      case Operation::SetLesser:
      case Operation::SetGreaterOrEqual:
      case Operation::SetLesserOrEqual:
      case Operation::Negate:
      case Operation::LogicalNOT:
      case Operation::BitwiseNOT:
        vars.erase(i->operands[0]);
        break;
      case Operation::Label:
        vars.clear();
        break;
    }
  }

  return changed;
}

bool trimInaccessibleCode(std::vector<Operation> &absCode)
{
  bool changed = false;
  bool accessible = true;

  std::map<std::string, uint8_t> identifiers;

  for (std::vector<Operation>::iterator i = absCode.begin(); i != absCode.end(); i++)
  {
    if (!accessible)
    {
      absCode.erase(i--);
      changed = true;
    }

    if (i->code == Operation::Label)
    {
      identifiers[i->operands[0]] |= 2;
      accessible = true;
    } else if (i->code == Operation::Jump || i->code == Operation::Return)
    {
      accessible = false;
    }

    if (i->code == Operation::Jump || i->code == Operation::JumpIfZero || i->code == Operation::JumpIfNotZero || i->code == Operation::Call)
    {
      identifiers[i->operands[0]] |= 1;
    }

    if (i->code == Operation::Set || 
      i->code == Operation::Dereference || 
      i->code == Operation::SetAddition || 
      i->code == Operation::SetSubtraction || 
      i->code == Operation::SetMultiplication || 
      i->code == Operation::SetDivision || 
      i->code == Operation::SetModulo || 
      i->code == Operation::SetBitwiseAND || 
      i->code == Operation::SetBitwiseOR || 
      i->code == Operation::SetBitwiseXOR || 
      i->code == Operation::SetLeftShift || 
      i->code == Operation::SetRightShift || 
      i->code == Operation::SetLogicalAND || 
      i->code == Operation::SetLogicalOR || 
      i->code == Operation::SetEqual || 
      i->code == Operation::SetNotEqual || 
      i->code == Operation::SetGreater || 
      i->code == Operation::SetLesser || 
      i->code == Operation::SetGreaterOrEqual || 
      i->code == Operation::SetLesserOrEqual || 
      i->code == Operation::Negate || 
      i->code == Operation::LogicalNOT || 
      i->code == Operation::BitwiseNOT)
    {
      if (i->operands[0].find("_Volatile") == std::string::npos)
      {
        identifiers[i->operands[0]] |= 2;
      }
      if (!i->operands[1].empty() && i->operands[1].find_first_not_of("0123456789") != std::string::npos)
      {
        identifiers[i->operands[1]] |= 1;
      }
      if (!i->operands[2].empty() && i->operands[2].find_first_not_of("0123456789") != std::string::npos)
      {
        identifiers[i->operands[2]] |= 1;
      }
    } else if (i->code != Operation::Label && 
      i->code != Operation::Jump)
    {
      if (!i->operands[0].empty() && i->operands[0].find_first_not_of("0123456789") != std::string::npos)
      {
        identifiers[i->operands[0]] |= 1;
      }
      if (!i->operands[1].empty() && i->operands[1].find_first_not_of("0123456789") != std::string::npos)
      {
        identifiers[i->operands[1]] |= 1;
      }
      if (!i->operands[2].empty() && i->operands[2].find_first_not_of("0123456789") != std::string::npos)
      {
        identifiers[i->operands[2]] |= 1;
      }
    }
  }

  while (!identifiers.empty() && (identifiers.begin()->second != 2 || identifiers.begin()->first == "main_Function"))
  {
    identifiers.erase(identifiers.begin());
  }

  for (std::map<std::string, uint8_t>::iterator l = identifiers.begin(); l != identifiers.end(); l++)
  {
    if (l->second != 2 || l->first == "main_Function")
    {
      identifiers.erase(l--);
    }
  }

  if (identifiers.empty())
  {
    return changed;
  }
  /*
93824992619968_Constant = 0
i = 0
93824992635248_DoWhileLoopBegin:
93824992635616_Constant = 248
93824992635408_Dereference = *93824992635616_Constant
93824992635408_Dereference = i
93824992635696_Assigned = 93824992635408_Dereference
i = i + 1
93824992635776_Incremented = i
Jump 93824992635248_DoWhileLoopBegin if i != 0
93824992635872_Constant = 0
Return 93824992635872_Constant
  */
  for (std::vector<Operation>::iterator i = absCode.begin(); i != absCode.end(); i++)
  {
    if (i->code == Operation::Set || 
      i->code == Operation::Dereference || 
      i->code == Operation::SetAddition || 
      i->code == Operation::SetSubtraction || 
      i->code == Operation::SetMultiplication || 
      i->code == Operation::SetDivision || 
      i->code == Operation::SetModulo || 
      i->code == Operation::SetBitwiseAND || 
      i->code == Operation::SetBitwiseOR || 
      i->code == Operation::SetBitwiseXOR || 
      i->code == Operation::SetLeftShift || 
      i->code == Operation::SetRightShift || 
      i->code == Operation::SetLogicalAND || 
      i->code == Operation::SetLogicalOR || 
      i->code == Operation::SetEqual || 
      i->code == Operation::SetNotEqual || 
      i->code == Operation::SetGreater || 
      i->code == Operation::SetLesser || 
      i->code == Operation::SetGreaterOrEqual || 
      i->code == Operation::SetLesserOrEqual || 
      i->code == Operation::Negate || 
      i->code == Operation::LogicalNOT || 
      i->code == Operation::BitwiseNOT || 
      i->code == Operation::Label)
    {
      for (std::map<std::string, uint8_t>::iterator l = identifiers.begin(); l != identifiers.end(); l++)
      {
        if (i->operands[0] == l->first)
        {
          absCode.erase(i--);
          changed = true;
        }
      }
    }
  }

  return changed;
}
