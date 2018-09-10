#include <benchmark/benchmark.h>
#include <boost/dynamic_bitset.hpp>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <random>
#include <bitset>

static void bool_vector_create(benchmark::State &state) {
	for (auto _ : state) {
		std::vector<bool>(50000);
	}
}
BENCHMARK(bool_vector_create);

static void boost_dynamic_bitset_create(benchmark::State &state) {
	for (auto _ : state) {
		boost::dynamic_bitset<> a(50000, 0);
	}
}
BENCHMARK(boost_dynamic_bitset_create);

static void std_bitset_create(benchmark::State &state) {
	for (auto _ : state) {
		std::bitset<50000> a;
	}
}
BENCHMARK(std_bitset_create);

static void bool_vector_or(benchmark::State &state) {
	std::vector<bool> a(50000);
	std::vector<bool> b(50000);
	std::vector<bool> c(50000);

	for (auto _ : state) {
		std::vector<bool>::iterator itC = c.begin();
		std::vector<bool>::const_iterator itA = a.begin();
		std::vector<bool>::const_iterator itB = b.begin();

		// c++ implementation-specific
		while (itA < a.end())
			*(itC._M_p ++) = *(itA._M_p ++) | *(itB._M_p ++); // word-at-a-time bitwise operation
		}
}
BENCHMARK(bool_vector_or);

static void bool_vector_or_stupid(benchmark::State &state) {
	std::vector<bool> a(50000);
	std::vector<bool> b(50000);
	std::vector<bool> c(a.size());

	for (auto _ : state) {
		std::transform(a.begin(), a.end(),
					   b.begin(), c.begin(), std::logical_or<bool>());
	}
}
BENCHMARK(bool_vector_or_stupid);

static void boost_dynamic_bitset_or(benchmark::State &state) {
	boost::dynamic_bitset<> a(50000, 0);
	boost::dynamic_bitset<> b(50000, 0);
	boost::dynamic_bitset<> c(50000, 0);

	for (auto _ : state) {
		c = a | b;
	}
}
BENCHMARK(boost_dynamic_bitset_or);

static void std_bitset_or(benchmark::State &state) {
	std::bitset<50000> a;
	std::bitset<50000> b;
	std::bitset<50000> c;

	b[10] = true;
	a[1000] = true;

	for (auto _ : state) {
		c = a | b;
	}
}
BENCHMARK(std_bitset_or);

BENCHMARK_MAIN();