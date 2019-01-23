#include <algorithm>
#include <SparseBitVector.h>

#include "SparseBitVector.h"

// ##### Constructors
SparseBitVector::SparseBitVector(size_t numBits) : numBits(numBits) {
}

SparseBitVector::SparseBitVector(const SparseBitVector &other) = default;

SparseBitVector::SparseBitVector(SparseBitVector &&other) noexcept : numBits(other.numBits), bitarray(std::move(other.bitarray)) {
}

// ##### Operators
SparseBitVector &SparseBitVector::operator=(SparseBitVector &&other) noexcept {
    numBits = other.numBits;
    bitarray = std::move(other.bitarray);
    return *this;
}

SparseBitVector SparseBitVector::operator|(const SparseBitVector &rhs) const {
    assert(numBits == rhs.numBits);
    SparseBitVector result(numBits);
    size_t i = 0;
    size_t j = 0;
    while (i < bitarray.size() && j < bitarray.size())
    {
        if (bitarray[i] < rhs.bitarray[j]) {
            result.bitarray.push_back(bitarray[i]);
            i++;
        } else if (rhs.bitarray[j] < bitarray[i]) {
            result.bitarray.push_back(rhs.bitarray[j]);
            j++;
        } else {
            result.bitarray.push_back(bitarray[i]);
            i++;
            j++;
        }
    }

    if (i != bitarray.size()) {
        for (; i < bitarray.size(); i++) {
            result.bitarray.push_back(bitarray[i]);
        }
    } else if (j != rhs.bitarray.size()) {
        for (; j < rhs.bitarray.size(); j++) {
            result.bitarray.push_back(rhs.bitarray[j]);
        }
    }

    return result;
}

bool SparseBitVector::operator==(const SparseBitVector &rhs) const {
    assert(numBits == rhs.numBits);
    if (bitarray.size() != rhs.bitarray.size()) {
        return false;
    }

    for (size_t i = 0; i < bitarray.size(); i++) {
        if (bitarray[i] != rhs.bitarray[i]) {
            return false;
        }
    }
    return true;
}

bool SparseBitVector::operator!=(const SparseBitVector &rhs) const {
    return !(*this == rhs);
}

bool SparseBitVector::operator<(const SparseBitVector &rhs) const {
    assert(numBits == rhs.numBits);
    int i = static_cast<int>(bitarray.size() - 1);
    int j = static_cast<int>(rhs.bitarray.size() - 1);
    for (; i >= 0 && j >= 0; i--, j--) {
        if (bitarray[i] < rhs.bitarray[i]) {
            return true;
        } else if (bitarray[i] > rhs.bitarray[i]) {
            return false;
        }
    }

    if (i + j == -2) {
      return false;
    } else {
        return i == -1;
    }
}

bool SparseBitVector::operator>(const SparseBitVector &rhs) const {
    return rhs < *this;
}

bool SparseBitVector::operator<=(const SparseBitVector &rhs) const {
    return !(*this > rhs);
}

bool SparseBitVector::operator>=(const SparseBitVector &rhs) const {
    return !(*this < rhs);
}

std::ostream &operator<<(std::ostream &stream, const SparseBitVector &array) {
    size_t posInVector = array.bitarray.size() - 1;
    for (size_t i = array.numBits; i > 0; i--) {
        if (array.bitarray[posInVector] == i) {
            stream << 1;
            posInVector--;
        } else {
            stream << 0;
        }
    }
    return stream;
}

// ##### Getters/Setters
size_t SparseBitVector::getNumBits() const {
    return numBits;
}

const std::vector<uint32_t> &SparseBitVector::getBitarray() const {
    return bitarray;
}

void SparseBitVector::setBit(size_t i) {
    assert(i < numBits);
    bitarray.insert(std::upper_bound(bitarray.begin(), bitarray.end(), i), static_cast<uint32_t>(i));
}

// ##### Functions
size_t SparseBitVector::countOnes() const {
    return bitarray.size();
}

bool SparseBitVector::covers(const SparseBitVector &rhs) const {
    assert(numBits == rhs.numBits);
    if (bitarray.size() < rhs.bitarray.size()) {
        return false;
    }

    size_t i = 0;
    size_t j = 0;
    while (i < bitarray.size() && j < rhs.bitarray.size()) {
        if (bitarray[i] < rhs.bitarray[j]) {
            i++;
        } else if (rhs.bitarray[j] < bitarray[i]) {
            return false;
        } else {
            i++;
            j++;
        }
    }

    return j >= rhs.bitarray.size();
}

size_t SparseBitVector::calculateDistance(const SparseBitVector &rhs) const {
    assert(numBits == rhs.numBits);
    size_t result = 0;
    size_t i = 0;
    size_t j = 0;
    while (i < bitarray.size() && j < rhs.bitarray.size()) {
        if (bitarray[i] < rhs.bitarray[j]) {
            result++;
            i++;
        } else if (rhs.bitarray[j] < bitarray[i]) {
            result++;
            j++;
        } else {
            i++;
            j++;
        }
    }

    if (i != bitarray.size()) {
        result += bitarray.size() - i;
    } else if (j != rhs.bitarray.size()) {
        result += rhs.bitarray.size() - j;
    }
    return result;
}

void SparseBitVector::setRightmost(const SparseBitVector &rhs) {
    assert(numBits == rhs.numBits);
    size_t i = 0;
    size_t j = 0;
    while (i < bitarray.size() && j < rhs.bitarray.size()) {
        if (bitarray[i] < rhs.bitarray[j]) {
            i++;
        } else if (rhs.bitarray[j] < bitarray[i]) {
            setBit(rhs.bitarray[j]);
            return;
        } else {
            i++;
            j++;
        }
    }

    assert(i == bitarray.size());
    if (j != rhs.bitarray.size()) {
        setBit(rhs.bitarray[j]);
    }
}
