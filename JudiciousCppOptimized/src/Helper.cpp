#include <map>
#include <chrono>
#include <iostream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "Helper.h"

std::map<std::string, long> measurements;
std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> measureStarts;

void startTM(const std::string &identifier) {
	if (measureStarts.count(identifier) != 0) {
		std::cout << "The timer '" << identifier << "' is already running!" << std::endl;
		abort();
	}

	measureStarts.emplace(identifier, std::chrono::high_resolution_clock::now());
}

void endTM(const std::string &identifier) {
	if(measureStarts.count(identifier) == 0) {
		std::cout << "The timer '" << identifier << "' isn't running!" << std::endl;
		abort();
	}

	long time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - measureStarts.at(identifier)).count();
	if (!measurements.count(identifier)) {
		measurements.emplace(identifier, time);
	} else {
		measurements.at(identifier) += time;
	}

	measureStarts.erase(identifier);
}

void printAllTM() {
	if (!measureStarts.empty()) {
		std::cout << "There is still a timer running: " << measureStarts.begin()->first << std::endl;
		abort();
	}

	for (const auto &entry : measurements) {
		if (entry.second < 10'000'000) {
			std::cout << entry.first << ": " << entry.second / 1000.0 << "µs" << std::endl;
		} else {
			std::cout << entry.first << ": " << entry.second / 1000.0 / 1000.0 << "ms" << std::endl;
		}
	}
}

bool partitionsContainAllVertices(const Hypergraph &hypergraph, const std::vector<std::vector<size_t>> &partitions) {
	std::vector<bool> nodeCovered(hypergraph.getHypernodes().size(), false);
	for (auto partition : partitions) {
		for (const auto &node : partition) {
			nodeCovered[node] = true;
		}
	}

	return std::find(nodeCovered.begin(), nodeCovered.end(), false) == nodeCovered.end();
}

std::vector<std::string> splitLineAtSpaces(const std::string &line) {
	std::vector<std::string> splitLine;
	boost::split(splitLine, line, boost::is_any_of(" "));
	return splitLine;
}

uint32_t stringToUint32t(const std::string &theString) {
	uint32_t theInt;
	std::istringstream iss(theString);
	iss >> theInt;
	return theInt;
}