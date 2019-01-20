#ifndef JUDICIOUSCPPOPTIMIZED_SPARSEBITVECTOR_H
#define JUDICIOUSCPPOPTIMIZED_SPARSEBITVECTOR_H

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

class SparseBitVector {
private:
    size_t numBits{};
    std::vector<uint32_t> bitarray;

public:
    // ##### Constructors
    /**
     * Constructs a broken state, only use for local variables that get assigned over!
     */
    SparseBitVector() = default;

    explicit SparseBitVector(size_t numBits);
    SparseBitVector(const SparseBitVector &other);
    SparseBitVector(SparseBitVector &&other) noexcept;

    // ##### Operators
    SparseBitVector &operator=(SparseBitVector &&other) noexcept;
    SparseBitVector operator|(const SparseBitVector &rhs) const;
    bool operator==(const SparseBitVector &rhs) const;
    bool operator!=(const SparseBitVector &rhs) const;
    bool operator<(const SparseBitVector &rhs) const;
    bool operator>(const SparseBitVector &rhs) const;
    bool operator<=(const SparseBitVector &rhs) const;
    bool operator>=(const SparseBitVector &rhs) const;
    friend std::ostream &operator<<(std::ostream &stream, const SparseBitVector &array);


    // ##### Getters/Setters
    size_t getNumBits() const;
    const std::vector<uint32_t> &getBitarray() const;

    /**
     * Sets the bit at index i to 1. 0 is the LSB, numBits-1 is the MSB.
     *
     * @param i The bits index.
     */
    void setBit(size_t i);

    // ##### Functions
    size_t countOnes() const;
    bool covers(const SparseBitVector &rhs) const;
    size_t calculateDistance(const SparseBitVector &rhs) const;
    void setRightmost(const SparseBitVector &rhs);
};

namespace std {
    template <> struct hash<SparseBitVector> {
        size_t operator()(const SparseBitVector &array) const {
            size_t result = 0;
            for (size_t i = 0; i < array.getBitarray().size(); i++) {
                boost::hash_combine(result, std::hash<uint32_t>{}(array.getBitarray()[i]));
            }
            return result;
        }
    };
}


#endif //JUDICIOUSCPPOPTIMIZED_SPARSEBITVECTOR_H
