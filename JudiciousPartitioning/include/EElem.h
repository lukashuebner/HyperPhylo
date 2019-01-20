#ifndef JUDICIOUSCPPOPTIMIZED_EELEM_H
#define JUDICIOUSCPPOPTIMIZED_EELEM_H

#include "Definitions.h"
#include "SElem.h"
#include "AlignedBitArray.h"
#include "SparseBitVector.h"

#include <set>

/**
 * elems of the set E: sets of hyperedges for each hypernode that contain a hypernode and set S*, which
 * is just the set E of the next round
 */
class EElem {
private:
    BitRepresentation combination;
    // Elements of the original e set that are covered by this combination.
    mutable std::set<uint32_t> coveredE0Elems;

public:
    // ##### Constructors
    /**
     * Create with a specific width and empty covering
     */
    explicit EElem(size_t numBits);

    /**
     * Create new from combination and covering
     */
    EElem(BitRepresentation combination, std::set<uint32_t> coveredE0Elems);

    /**
     * Converts from SElem by dropping the coveredEElems set. This MOVES the inner datastructures, do not use original afterwards!
     */
    explicit EElem(SElem &&original);

    // ##### Operators
    friend bool operator== (const EElem &lhs, const EElem &rhs);
    friend bool operator!= (const EElem &lhs, const EElem &rhs);

    /**
     * Made for duplicate removal in generateE. Compares by the stored bitvectors number representation.
     */
    bool operator<(const EElem &rhs) const;
    bool operator>(const EElem &rhs) const;
    bool operator<=(const EElem &rhs) const;
    bool operator>=(const EElem &rhs) const;

    // ##### Getters/Setters
    const BitRepresentation &getCombination() const;
    BitRepresentation &getCombination();
    std::set<uint32_t> &getCoveredE0Elems() const;
    std::set<uint32_t> &getCoveredE0Elems();
    size_t countOnes() const;
};

namespace std {
    template <> struct hash<EElem> {
        size_t operator()(const EElem &e) const {
            return std::hash<BitRepresentation>{}(e.getCombination());
        }
    };
}

#endif //JUDICIOUSCPPOPTIMIZED_EELEM_H
