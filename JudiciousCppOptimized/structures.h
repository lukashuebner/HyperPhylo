#ifndef JUDICIOUSCPPOPTIMIZED_STRUCTURES_H
#define JUDICIOUSCPPOPTIMIZED_STRUCTURES_H

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <boost/functional/hash.hpp>
#include <immintrin.h>

#include "Helper.h"

#ifdef __AVX512F__
#define ALIGNMENT 64
#elif __AVX2__
#define ALIGNMENT 32
#else
#define ALIGNMENT 16
#endif


struct AlignedBitArray {
    struct Deleter {
        void operator()(uint64_t *data) const {
            free(data);
        };
    };

    using ptr_type = std::unique_ptr<uint64_t[], Deleter>;
    size_t numBits{};
    size_t numInts{};
    ptr_type bitarray;


    /**
     * Constructs a broken state, only use for local variables that get assigned over!
     */
    AlignedBitArray() = default;

    explicit AlignedBitArray(size_t numBits);

    AlignedBitArray(const AlignedBitArray &other);

    AlignedBitArray(AlignedBitArray &&other) noexcept;

    AlignedBitArray &operator=(AlignedBitArray &&other) noexcept {
        numBits = other.numBits;
        numInts = other.numInts;
        bitarray = std::move(other.bitarray);
        return *this;
    }

    static ptr_type malloc_aligned(size_t numInts);

    size_t countOnes() const;

    /**
     * Gets the bit at index i. 0 is the LSB, numBits-1 is the MSB.
     *
     * @param i The bits index.
     * @return The bit value.
     */
    bool getBit(size_t i) const;

    /**
     * Sets the bit at index i to 1. 0 is the LSB, numBits-1 is the MSB.
     *
     * @param i The bits index.
     */
    void setBit(size_t i) const;

    bool covers(const AlignedBitArray &rhs) const;

    size_t calculateDistance(const AlignedBitArray &rhs) const;

    uint64_t &operator[](const size_t i) {
        return bitarray[i];
    }

    const uint64_t &operator[](const size_t i) const {
        return bitarray[i];
    }

    AlignedBitArray operator|(const AlignedBitArray &rhs) const;

    bool operator==(const AlignedBitArray &rhs) const;

    bool operator!=(const AlignedBitArray &rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const AlignedBitArray &rhs) const;

    bool operator>(const AlignedBitArray &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const AlignedBitArray &rhs) const {
        return !(*this > rhs);
    }

    bool operator>=(const AlignedBitArray &rhs) const {
        return !(*this < rhs);
    }
};

std::ostream &operator<<(std::ostream &stream, const AlignedBitArray &array);

namespace std {
    template <> struct hash<AlignedBitArray> {
        size_t operator()(const AlignedBitArray &array) const {
            size_t result = 0;
            for (size_t i = 0; i < array.numInts; i++) {
                boost::hash_combine(result, std::hash<uint64_t>{}(array.bitarray[i]));
            }
            return result;
        }
    };
}

// elems of the set S: maps a combination to the covered elements in E
struct sElem {
    AlignedBitArray combination;
    // Elements of the e set in the current iteration that are covered by this combination.
    mutable std::set<size_t> coveredEElems;
    // Elements of the original e set that are covered by this combination.
    mutable std::set<size_t> coveredE0Elems;

    sElem() = default;

    // Construct from AlignedBitArray
    explicit sElem(AlignedBitArray &&combination) : combination(std::move(combination)) {
    }

    bool operator==(const sElem &rhs) const {
        return this->combination == rhs.combination;
    }

    bool operator!=(const sElem &rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const sElem &rhs) const {
        if (this->coveredEElems.size() == rhs.coveredEElems.size()) {
            return this->combination < rhs.combination;
        }

        return this->coveredEElems.size() < rhs.coveredEElems.size();
    }

    bool operator>(const sElem &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const sElem &rhs) const {
        return !(*this > rhs);
    }

    bool operator>=(const sElem &rhs) const {
        return !(*this < rhs);
    }
};


// elems of the set E: sets of hyperedges for each hypernode that contain a hypernode and set S*, which
// is just the set E of the next round
struct eElem {
    AlignedBitArray combination;
    // Elements of the original e set that are covered by this combination.
    std::set<size_t> coveredE0Elems;

    // Create new from combination and covering
    explicit eElem(AlignedBitArray combination, std::set<size_t> coveredE0Elems) :
            combination(std::move(combination)),
            coveredE0Elems(std::move(coveredE0Elems)) {
    }

    // Convert from sElem
    explicit eElem(sElem &&original) : combination(std::move(original.combination)), coveredE0Elems(std::move(original.coveredE0Elems)) {
    }

    friend bool operator== (const eElem &lhs, const eElem &rhs) {
        return lhs.combination == rhs.combination;
    }

    friend bool operator!= (const eElem &lhs, const eElem &rhs) {
        return !(lhs == rhs);
    }
};

namespace std {
    template <> struct hash<sElem> {
        size_t operator()(const sElem &s) const {
            return std::hash<AlignedBitArray>{}(s.combination);
        }
    };
}

#endif //JUDICIOUSCPPOPTIMIZED_STRUCTURES_H
