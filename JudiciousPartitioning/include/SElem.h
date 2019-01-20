#ifndef JUDICIOUSCPPOPTIMIZED_SELEM_H
#define JUDICIOUSCPPOPTIMIZED_SELEM_H

#include "Definitions.h"
#include "AlignedBitArray.h"
#include "SparseBitVector.h"

#include <set>

/**
 * Elems of the set S: maps a combination to the covered elements in E.
 */
class SElem {
private:
    BitRepresentation combination;

    /**
     * Elements of the e set in the current iteration that are covered by this combination.
     */
    std::set<uint32_t> coveredEElems;

    /**
     * Elements of the original e set that are covered by this combination.
     */
    std::set<uint32_t> coveredE0Elems;

public:
    // ##### Constructors
    SElem() = default;

    /**
     * Construct a new SElem by merging two EElems.
     * Moves the combination. Do not use the BitRepresentation afterwards!
     */
    explicit SElem(BitRepresentation &&combination,
                   uint32_t leftElementIdx,
                   uint32_t rightElementIdx,
                   const std::set<uint32_t> &left,
                   const std::set<uint32_t> &right) noexcept;

    // ##### Operators
    bool operator==(const SElem &rhs) const;
    bool operator!=(const SElem &rhs) const;

    /**
     * Made for determinism sort in findMinimalSubset. Sorts by numerical representation of the bitvector,
     * then by number of elements of E covered.
     */
    bool operator<(const SElem &rhs) const;
    bool operator>(const SElem &rhs) const;
    bool operator<=(const SElem &rhs) const;
    bool operator>=(const SElem &rhs) const;

    // ##### Getters/Setters
    const BitRepresentation &getCombination() const;
    const std::set<uint32_t> &getCoveredEElems() const;
    const std::set<uint32_t> &getCoveredE0Elems() const;
    BitRepresentation &getCombination();
    std::set<uint32_t> &getCoveredEElems();
    std::set<uint32_t> &getCoveredE0Elems();

    // ##### Functions
    bool covers(const SElem &rhs) const;
    bool covers(const BitRepresentation &rhs) const;
    size_t countOnes() const;
};

namespace std {
    template <> struct hash<SElem> {
        size_t operator()(const SElem &s) const {
            return std::hash<BitRepresentation>{}(s.getCombination());
        }
    };
}


#endif //JUDICIOUSCPPOPTIMIZED_SELEM_H
