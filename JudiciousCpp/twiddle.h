#ifndef JUDICIOUSCPP_TWIDDLE_H
#define JUDICIOUSCPP_TWIDDLE_H

#include <vector>
#include "Hypergraph.h"

void setBaseSet(const std::set<hElem> &baseSet);
void initCombinationGenerator(int k);
std::set<hElem> getNextCombination();

#endif //JUDICIOUSCPP_TWIDDLE_H
