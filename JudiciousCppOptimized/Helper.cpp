#include "Helper.h"

std::map<std::string, long> measurements;
std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> measureStarts;

void startTM(const std::string identifier) {
	if (measureStarts.count(identifier) != 0) {
		std::cout << "The timer '" << identifier << "' is already running!" << std::endl;
		abort();
	}

	measureStarts.emplace(identifier, std::chrono::high_resolution_clock::now());
}

void endTM(const std::string identifier) {
	if(measureStarts.count(identifier) == 0) {
		std::cout << "The timer '" << identifier << "' isn't running!" << std::endl;
		abort();
	}

	long time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - measureStarts.at(identifier)).count();
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
		if (entry.second < 10'000) {
			std::cout << entry.first << ": " << entry.second << "µs" << std::endl;
		} else {
			std::cout << entry.first << ": " << entry.second / 1000 << "ms" << std::endl;
		}
	}
}

bool partitionsContainAllVerties(Hypergraph hypergraph, std::vector<std::vector<uint32_t>> partitions) {
	std::vector<bool> nodeCovered(hypergraph.getHypernodes().size(), false);
	for (auto partition: partitions) {
		for (auto node: partition) {
			nodeCovered[node] = true;
		}
	}

	if(std::find(nodeCovered.begin(), nodeCovered.end(), false) != nodeCovered.end()) {
		return false;
	} else {
		return true;
	}
}