#ifndef STEREOMATCHING_HELPER_H
#define STEREOMATCHING_HELPER_H

#define _USE_MATH_DEFINES
#include <chrono>
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include "Hypergraph.h"

void startTM(std::string identifier);
void endTM(std::string identifier);
void printAllTM();
bool partitionsContainAllVerties(Hypergraph, std::vector<std::vector<uint32_t>>);

#endif //STEREOMATCHING_HELPER_H
