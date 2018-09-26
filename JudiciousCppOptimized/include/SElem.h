#ifndef JUDICIOUSCPPOPTIMIZED_SELEM_H
#define JUDICIOUSCPPOPTIMIZED_SELEM_H

#include <set>
#include "AlignedBitArray.h"

/**
 * Elems of the set S: maps a combination to the covered elements in E.
 */
class SElem {
private:
    AlignedBitArray combination;

    /**
     * Elements of the e set in the current iteration that are covered by this combination.
     */
    std::set<size_t> coveredEElems;

    /**
     * Elements of the original e set that are covered by this combination.
     */
    std::set<size_t> coveredE0Elems;

public:
    // ##### Constructors
    SElem() = default;

    /**
     * Construct a new SElem by merging two EElems.
     * Moves the combination. Do not use the AlignedBitArray afterwards!
     */
    explicit SElem(AlignedBitArray &&combination,
                   size_t leftElementIdx,
                   size_t rightElementIdx,
                   const std::set<size_t> &left,
                   const std::set<size_t> &right) noexcept;

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
    const AlignedBitArray &getCombination() const;
    const std::set<size_t> &getCoveredEElems() const;
    const std::set<size_t> &getCoveredE0Elems() const;
    AlignedBitArray &getCombination();
    std::set<size_t> &getCoveredEElems();
    std::set<size_t> &getCoveredE0Elems();

    // ##### Functions
    bool covers(const SElem &rhs) const;
    bool covers(const AlignedBitArray &rhs) const;
    size_t countOnes() const;
};

namespace std {
    template <> struct hash<SElem> {
        size_t operator()(const SElem &s) const {
            return std::hash<AlignedBitArray>{}(s.getCombination());
        }
    };
}


#endif //JUDICIOUSCPPOPTIMIZED_SELEM_H
