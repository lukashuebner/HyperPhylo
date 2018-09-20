#ifndef JUDICIOUSCPPOPTIMIZED_HELPER_H
#define JUDICIOUSCPPOPTIMIZED_HELPER_H

#include <chrono>
#include <string>
#include <map>
#include <iostream>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "Hypergraph.h"

void startTM(const std::string &identifier);
void endTM(const std::string &identifier);
void printAllTM();
bool partitionsContainAllVertices(const Hypergraph &hypergraph, const std::vector<std::vector<size_t>> &line);
std::vector<std::string> splitLineAtSpaces(const std::string &line);
uint32_t stringToUint32t(const std::string &theString);

#endif //JUDICIOUSCPPOPTIMIZED_HELPER_H
