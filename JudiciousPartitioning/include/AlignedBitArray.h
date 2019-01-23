#ifndef JUDICIOUSCPPOPTIMIZED_ALIGNEDBITARRAY_H
#define JUDICIOUSCPPOPTIMIZED_ALIGNEDBITARRAY_H

#include <cstdlib>
#include <bits/unique_ptr.h>
#include <boost/functional/hash.hpp>

#ifdef __AVX512F__
#define ALIGNMENT 64
#elif __AVX2__
#define ALIGNMENT 32
#else
#define ALIGNMENT 16
#endif

class AlignedBitArray {
private:
    struct Deleter {
        void operator()(uint64_t *data) const {
            free(data);
        };
    };

    using ptr_type = std::unique_ptr<uint64_t[], Deleter>;
    size_t numBits{};
    size_t numInts{};
    ptr_type bitarray;

    static ptr_type malloc_aligned(size_t numInts);

public:
    // ##### Constructors
    /**
     * Constructs a broken state, only use for local variables that get assigned over!
     */
    AlignedBitArray() = default;

    explicit AlignedBitArray(size_t numBits);
    AlignedBitArray(const AlignedBitArray &other);
    AlignedBitArray(AlignedBitArray &&other) noexcept;

    // ##### Operators
    AlignedBitArray &operator=(AlignedBitArray &&other) noexcept;
    uint64_t &operator[](size_t i);
    const uint64_t &operator[](size_t i) const;
    AlignedBitArray operator|(const AlignedBitArray &rhs) const;
    AlignedBitArray operator&(const AlignedBitArray &rhs) const;
    bool operator==(const AlignedBitArray &rhs) const;
    bool operator!=(const AlignedBitArray &rhs) const;
    bool operator<(const AlignedBitArray &rhs) const;
    bool operator>(const AlignedBitArray &rhs) const;
    bool operator<=(const AlignedBitArray &rhs) const;
    bool operator>=(const AlignedBitArray &rhs) const;
    friend std::ostream &operator<<(std::ostream &stream, const AlignedBitArray &array);

    // ##### Getters/Setters
    size_t getNumBits() const;
    size_t getNumInts() const;

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

    // ##### Functions
    size_t countOnes() const;
    bool covers(const AlignedBitArray &rhs) const;
    size_t calculateDistance(const AlignedBitArray &rhs) const;
    void setRightmost(const AlignedBitArray &rhs);
};

namespace std {
    template <> struct hash<AlignedBitArray> {
        size_t operator()(const AlignedBitArray &array) const {
            size_t result = 0;
            for (size_t i = 0; i < array.getNumInts(); i++) {
                boost::hash_combine(result, std::hash<uint64_t>{}(array[i]));
            }
            return result;
        }
    };
}


#endif //JUDICIOUSCPPOPTIMIZED_ALIGNEDBITARRAY_H
