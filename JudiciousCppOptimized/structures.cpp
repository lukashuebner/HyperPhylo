#include "structures.h"

size_t AlignedBitArray::countOnes() const {
    size_t result = 0;
    for (size_t i = 0; i < numInts; i++) {
        result += __builtin_popcountll(bitarray[i]);
    }
    return result;
}

AlignedBitArray::AlignedBitArray(size_t numBits) : numBits(numBits), numInts(numBits / 64 + 1), bitarray(malloc_aligned(numInts)) {
    memset(bitarray.get(), 0, sizeof(uint64_t) * numInts);
}

AlignedBitArray::AlignedBitArray(const AlignedBitArray &other) : numBits(other.numBits), numInts(other.numInts), bitarray(malloc_aligned(numInts))  {
    memcpy(bitarray.get(), other.bitarray.get(), sizeof(uint64_t) * numInts);
}

AlignedBitArray::AlignedBitArray(AlignedBitArray &&other) noexcept : numBits(other.numBits), numInts(other.numInts), bitarray(std::move(other.bitarray)) {
}

// STATIC
AlignedBitArray::ptr_type AlignedBitArray::malloc_aligned(size_t numInts) {
    uint64_t *raw = nullptr;
    if(posix_memalign(reinterpret_cast<void**>(&raw), ALIGNMENT, numInts * sizeof(uint64_t)) != 0) {
        std::cerr << "Aligned malloc failed!" << std::endl;
        throw std::bad_alloc();
    }
    return ptr_type(raw);
}

bool AlignedBitArray::getBit(size_t i) const {
    assert(i < numBits);
    size_t diff = numBits - i - 1;
    return static_cast<bool>((bitarray[diff / 64] << diff % 64) & 0x8000000000000000);
}

void AlignedBitArray::setBit(size_t i) const {
    assert(i < numBits);
    size_t diff = numBits - i - 1;
    bitarray[diff / 64] |= 0x8000000000000000 >> diff % 64;
}

bool AlignedBitArray::covers(const AlignedBitArray &rhs) const {
    assert(numInts == rhs.numInts && numBits == rhs.numBits);
    for (size_t i = 0; i < numInts; i++) {
        if ((bitarray[i] & rhs[i]) != rhs[i]) {
            return false;
        }
    }
    return true;
}

size_t AlignedBitArray::calculateDistance(const AlignedBitArray &rhs) const {
    assert(numInts == rhs.numInts && numBits == rhs.numBits);
    size_t result = 0;
    for (size_t i = 0; i < numInts; i++) {
        result += __builtin_popcountll(bitarray[i] ^ rhs[i]);
    }
    return result;
}

AlignedBitArray AlignedBitArray::operator|(const AlignedBitArray &rhs) const {
    assert(numInts == rhs.numInts && numBits == rhs.numBits);
    AlignedBitArray result(numBits);
    for (size_t i = 0; i < numInts; i++) {
        result[i] = bitarray[i] | rhs[i];
    }
    return result;
}

bool AlignedBitArray::operator==(const AlignedBitArray &rhs) const {
    assert(numInts == rhs.numInts && numBits == rhs.numBits);
    for (size_t i = 0; i < numInts; i++) {
        if (bitarray[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

bool AlignedBitArray::operator<(const AlignedBitArray &rhs) const {
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

std::ostream &operator<<(std::ostream &stream, const AlignedBitArray &array) {
    for (size_t i = array.numBits; i > 0; i--) {
        stream << array.getBit(i - 1);
    }
    return stream;
}

//bool AlignedBitArray::covers(const AlignedBitArray &rhs) const {
//    assert(numInts == rhs.numInts && numBits == rhs.numBits);
//    if (average_length == 0) {
//        average_length = numInts;
//    }
//    assert(average_length == numInts);
//    startTM("COVERS_LOOP");
//
//    size_t i = 0;
//    for (i = 0; i + 2 < numInts; i += 2) {
//        // Load data into ymmm register
//        __m128i vec_a = _mm_load_si128(reinterpret_cast<__m128i*>(&bitarray[i]));
//        __m128i vec_b = _mm_load_si128(reinterpret_cast<__m128i*>(&rhs.bitarray[i]));
//
//        // bitwise logical or
//        __m128i result = _mm_or_si128(vec_a, vec_b);
//
//        // compare for equality
//        result = _mm_cmpeq_epi64(result, vec_b);
//
//        // check if all values are one, if not return false
//        if (!_mm_test_all_ones(result)) {
//            endTM("COVERS_LOOP");
//            average_iterations = ((average_iterations * values_in_average) + i + 1) / (values_in_average + 1);
//            values_in_average++;
//            return false;
//        }
//    }
//
//    // Finish sequentialy
//    for (; i < numInts; i++) {
//        if ((bitarray[i] & rhs[i]) != rhs[i]) {
//            endTM("COVERS_LOOP");
//            average_iterations = ((average_iterations * values_in_average) + i + 1) / (values_in_average + 1);
//            values_in_average++;
//            return false;
//        }
//    }
//
//    endTM("COVERS_LOOP");
//    average_iterations = ((average_iterations * values_in_average) + numInts) / (values_in_average + 1);
//    values_in_average++;
//    return true;
//}
