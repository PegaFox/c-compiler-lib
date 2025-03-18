#ifndef PF_OPTIMIZE_IR_HPP
#define PF_OPTIMIZE_IR_HPP

#include "generate_IR.hpp"

void optimizeIR(std::vector<Operation> &asmCode);

bool resolveConstantOperations(std::vector<Operation> &absCode);

bool trimInaccessibleCode(std::vector<Operation> &absCode);

#endif // PF_OPTIMIZE_IR_HPP