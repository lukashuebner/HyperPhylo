#include <benchmark/benchmark.h>
#include <boost/dynamic_bitset.hpp>
#include <sdsl/bit_vectors.hpp>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <random>
#include <bitset>
#include <immintrin.h>
#include <emmintrin.h>

static const int SIZE = 5000000;

static void bool_vector_create(benchmark::State &state) {
	for (auto _ : state) {
		std::vector<bool> a(SIZE);
	}
}
BENCHMARK(bool_vector_create);

static void boost_dynamic_bitset_create(benchmark::State &state) {
	for (auto _ : state) {
		boost::dynamic_bitset<> a(SIZE, 0);
	}
}
BENCHMARK(boost_dynamic_bitset_create);

static void sdsl_bitvector_create(benchmark::State &state) {
	for (auto _ : state) {
		sdsl::bit_vector a(SIZE, 0);
	}
}
BENCHMARK(sdsl_bitvector_create);

static void sdsl_rrrvector_create(benchmark::State &state) {
	for (auto _ : state) {
		sdsl::bit_vector a0(SIZE, 0);
		sdsl::rrr_vector<> a(a0);
	}
}
BENCHMARK(sdsl_rrrvector_create);

static void bool_vector_or(benchmark::State &state) {
	std::vector<bool> a(SIZE);
	std::vector<bool> b(SIZE);
	std::vector<bool> c(SIZE);

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

static void memaligned_simd_or(benchmark::State &state) {
    uint64_t * a, *b, *c;
    const size_t neededInts = SIZE / 64;
    if(posix_memalign(reinterpret_cast<void**>(&a), 32, neededInts * sizeof(uint64_t)) != 0)
        assert(false);
    if(posix_memalign(reinterpret_cast<void**>(&b), 32, neededInts * sizeof(uint64_t)) != 0)
        assert(false);
    if(posix_memalign(reinterpret_cast<void**>(&c), 32, neededInts * sizeof(uint64_t)) != 0)
        assert(false);

	for (auto _ : state) {
        size_t I = 0;
        const uint32_t elementsPerIteration = 2;
        for (I = 0; I + elementsPerIteration < neededInts; I += elementsPerIteration) {
            // Load data into ymmm register
            __m128i vec_a = _mm_load_si128(reinterpret_cast<__m128i*>(&a[I]));
            __m128i vec_b = _mm_load_si128(reinterpret_cast<__m128i*>(&b[I]));

            // bitwise logical or
            __m128i vec_c = _mm_or_si128(vec_a, vec_b);

            // Store back the results
            _mm_store_si128(reinterpret_cast<__m128i*>(&c[I]), vec_c);
        }

        // Finish sequentialy
        for (; I < neededInts; I++) {
            c[I] = a[I] | b[I];
        }
	}
}
BENCHMARK(memaligned_simd_or);

static void memaligned_or(benchmark::State &state) {
    uint64_t * a, *b, *c;
    const size_t neededInts = SIZE / 64;
    if(posix_memalign(reinterpret_cast<void**>(&a), 32, neededInts * sizeof(uint64_t)) != 0)
        assert(false);
    if(posix_memalign(reinterpret_cast<void**>(&b), 32, neededInts * sizeof(uint64_t)) != 0)
        assert(false);
    if(posix_memalign(reinterpret_cast<void**>(&c), 32, neededInts * sizeof(uint64_t)) != 0)
        assert(false);

    for (auto _ : state) {
        for (size_t I = 0; I < neededInts; I++) {
            c[I] = a[I] | b[I];
        }
    }
}
BENCHMARK(memaligned_or);

static void bool_vector_or_stupid(benchmark::State &state) {
	std::vector<bool> a(SIZE);
	std::vector<bool> b(SIZE);
	std::vector<bool> c(SIZE);

	for (auto _ : state) {
		std::transform(a.begin(), a.end(),
					   b.begin(), c.begin(), std::logical_or<bool>());
	}
}
BENCHMARK(bool_vector_or_stupid);

static void boost_dynamic_bitset_or(benchmark::State &state) {
	boost::dynamic_bitset<> a(SIZE, 0);
	boost::dynamic_bitset<> b(SIZE, 0);
	boost::dynamic_bitset<> c(SIZE, 0);

	for (auto _ : state) {
		c = a | b;
	}
}
BENCHMARK(boost_dynamic_bitset_or);

static void sdsl_bitvector_or(benchmark::State &state) {
	sdsl::bit_vector a(SIZE, 0);
	sdsl::bit_vector b(SIZE, 0);
	sdsl::bit_vector c(SIZE, 0);

	for (auto _ : state) {
		size_t i = 0;
		for (; i + 64 < a.size(); i += 64) {
			c.set_int(i, (a.get_int(i) | b.get_int(i)));
		}
		auto len = static_cast<uint8_t>(a.size() - i);
		c.set_int(i, a.get_int(i, len) | b.get_int(i, len), len);
	}
}
BENCHMARK(sdsl_bitvector_or);

static void sdsl_bitvector_rrr_or(benchmark::State &state) {
	sdsl::bit_vector a0(SIZE, 0);
	sdsl::bit_vector b0(SIZE, 0);
	sdsl::bit_vector c0(SIZE, 0);

	sdsl::rrr_vector<> a(a0);
	sdsl::rrr_vector<> b(b0);
	sdsl::rrr_vector<> c(c0);

	for (auto _ : state) {
		size_t i = 0;
		for (; i + 64 < a.size(); i += 64) {
			c0.set_int(i, a.get_int(i) | b.get_int(i));
		}
		auto len = static_cast<uint8_t>(a.size() - i);
		c0.set_int(i, a.get_int(i, len) | b.get_int(i, len), len);
	}
}
BENCHMARK(sdsl_bitvector_rrr_or);

BENCHMARK_MAIN();