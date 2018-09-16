#ifndef JUDICIOUSCPPOPTIMIZED_STRUCTURES_H
#define JUDICIOUSCPPOPTIMIZED_STRUCTURES_H

#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <boost/functional/hash.hpp>
#include <immintrin.h>

#ifdef __AVX512F__
#define ALIGNMENT 64
#elif __AVX2__
#define ALIGNMENT 32
#else
#define ALIGNMENT 16
#endif

struct AlignedBitArray {
    std::shared_ptr<uint64_t[]> bitarray;
    size_t numBits;
    size_t numInts;

    /**
     * Constructs a broken state, only use for local variables that get assigned over!
     */
    AlignedBitArray() = default;

    explicit AlignedBitArray(size_t numBits) : numBits(numBits), numInts(numBits / 64 + 1) {
        uint64_t *raw = nullptr;
        if(posix_memalign(reinterpret_cast<void**>(&raw), ALIGNMENT, numInts * sizeof(uint64_t)) != 0) {
            std::cerr << "Aligned malloc failed!" << std::endl;
            throw std::bad_alloc();
        }
        bitarray = std::shared_ptr<uint64_t[]>(raw, [=](uint64_t *data) {
            free(data);
        });

        for (size_t i = 0; i < numInts; i++) {
            bitarray[i] = 0;
        }
    }

    size_t countOnes() const {
        size_t result = 0;
        for (size_t i = 0; i < numInts; i++) {
            result += __builtin_popcountll(bitarray[i]);
        }
        return result;
    }

    /**
     * Gets the bit at index i. 0 is the LSB, numBits-1 is the MSB.
     *
     * @param i The bits index.
     * @return The bit value.
     */
    bool getBit(size_t i) const {
        assert(i < numBits);
        size_t diff = numBits - i - 1;
        return static_cast<bool>((bitarray[diff / 64] << diff % 64) & 0x8000000000000000);
    }

    /**
     * Sets the bit at index i to 1. 0 is the LSB, numBits-1 is the MSB.
     *
     * @param i The bits index.
     */
    void setBit(size_t i) const {
        assert(i < numBits);
        size_t diff = numBits - i - 1;
        bitarray[diff / 64] |= 0x8000000000000000 >> diff % 64;
    }

    bool covers(const AlignedBitArray &rhs) const {
        assert(numInts == rhs.numInts && numBits == rhs.numBits);
        for (size_t i = 0; i < numInts; i++) {
            if ((bitarray[i] & rhs[i]) != rhs[i]) {
                return false;
            }
        }
        return true;
    }

    size_t calculateDistance(const AlignedBitArray &rhs) const {
        assert(numInts == rhs.numInts && numBits == rhs.numBits);
        size_t result = 0;
        for (size_t i = 0; i < numInts; i++) {
            result += __builtin_popcountll(bitarray[i] ^ rhs[i]);
        }
        return result;
    }

    uint64_t &operator[](const size_t i) {
        return bitarray[i];
    }

    const uint64_t &operator[](const size_t i) const {
        return bitarray[i];
    }

    AlignedBitArray operator|(const AlignedBitArray &rhs) const {
        assert(numInts == rhs.numInts && numBits == rhs.numBits);
        AlignedBitArray result(numBits);
        for (size_t i = 0; i < numInts; i++) {
            result[i] = bitarray[i] | rhs[i];
        }
        return result;
    }

    AlignedBitArray operator^(const AlignedBitArray &rhs) const {
        assert(numInts == rhs.numInts && numBits == rhs.numBits);
        AlignedBitArray result(numBits);
        for (size_t i = 0; i < numInts; i++) {
            result[i] = bitarray[i] ^ rhs[i];
        }
        return result;
    }

    AlignedBitArray operator&(const AlignedBitArray &rhs) const {
        assert(numInts == rhs.numInts && numBits == rhs.numBits);
        AlignedBitArray result(numBits);
        for (size_t i = 0; i < numInts; i++) {
            result[i] = bitarray[i] & rhs[i];
        }
        return result;
    }

    bool operator==(const AlignedBitArray &rhs) const {
        assert(numInts == rhs.numInts && numBits == rhs.numBits);
        for (size_t i = 0; i < numInts; i++) {
            if (bitarray[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const AlignedBitArray &rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const AlignedBitArray &rhs) const {
        assert(numInts == rhs.numInts && numBits == rhs.numBits);
        for (size_t i = 0; i < numInts; i++) {
            if (bitarray[i] < rhs[i]) {
                return true;
            } else if (bitarray[i] > rhs[i]) {
                return false;
            }
        }
        return false;
    }

    bool operator>(const AlignedBitArray &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const AlignedBitArray &rhs) const {
        return !(*this > rhs);
    }

    bool operator>=(const AlignedBitArray &rhs) const {
        return !(*this < rhs);
    }

    friend std::ostream &operator<<(std::ostream &stream, const AlignedBitArray &array) {
        for (size_t i = array.numBits; i > 0; i--) {
            stream << array.getBit(i - 1);
        }
        stream << std::endl;
        return stream;
    }
};

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

#endif //JUDICIOUSCPPOPTIMIZED_STRUCTURES_H
