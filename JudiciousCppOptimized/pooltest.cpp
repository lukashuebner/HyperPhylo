#include <zconf.h>
#include <tbb/concurrent_vector.h>
#include "tlx/thread_pool.cpp"
#include "structures.h"

// Global Threadpool
#ifndef OMP_NUM_THREADS
tlx::ThreadPool pool;
#else
tlx::ThreadPool pool(OMP_NUM_THREADS);
#endif

int main(int argc, char **argv) {
    std::vector<AlignedBitArray> bitsets;
    const int numberOfBitsAndBitarrays = 5000;
    for (size_t i = 0; i < numberOfBitsAndBitarrays; i++) {
        bitsets.emplace_back(numberOfBitsAndBitarrays);
        bitsets[i].setBit(i);
    }

    tbb::concurrent_vector<size_t> results;

    startTM("pool");
    for (size_t i = 0; i < bitsets.size(); i++) {
        pool.enqueue([i, &results, &bitsets]() {
            for (size_t j = i + 1; j < bitsets.size(); j++) {
                results.push_back(bitsets[i].calculateDistance(bitsets[j]));
            }
        });
    }
    pool.loop_until_empty();
    endTM("pool");
    for (size_t i = 0; i < bitsets.size(); i++) {
        assert(results[i] == 2);
    }

    std::cout << "POOL DONE, results: " << results.size() << std::endl;

    results.clear();
    startTM("OMP");
    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < bitsets.size(); i++) {
        for (size_t j = i + 1; j < bitsets.size(); j++) {
            results.push_back(bitsets[i].calculateDistance(bitsets[j]));
        }
    }
    endTM("OMP");
    for (size_t i = 0; i < bitsets.size(); i++) {
        assert(results[i] == 2);
    }

    std::cout << "OMP DONE, results: " << results.size() << std::endl;
    printAllTM();
}