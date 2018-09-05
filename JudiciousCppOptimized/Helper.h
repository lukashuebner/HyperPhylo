#ifndef STEREOMATCHING_HELPER_H
#define STEREOMATCHING_HELPER_H

#define _USE_MATH_DEFINES
#include <chrono>
#include <string>
#include <map>
#include <iostream>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "Hypergraph.h"

void startTM(std::string identifier);
void endTM(std::string identifier);
void printAllTM();
bool partitionsContainAllVertices(Hypergraph, std::vector<std::vector<size_t>>);
std::vector<std::string> splitLineAtSpaces(std::string line);
uint32_t stringToUint32t(const std::string &theString);

#endif //STEREOMATCHING_HELPER_H
