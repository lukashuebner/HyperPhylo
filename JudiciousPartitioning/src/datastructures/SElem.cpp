#include "SElem.h"

// ##### Constructors
SElem::SElem(AlignedBitArray &&combination,
        uint32_t leftElementIdx,
        uint32_t rightElementIdx,
        const std::set<uint32_t> &left,
        const std::set<uint32_t> &right) noexcept : combination(std::move(combination)), coveredEElems({ leftElementIdx, rightElementIdx }) {
    coveredE0Elems.insert(left.begin(), left.end());
    coveredE0Elems.insert(right.begin(), right.end());
}

// ##### Operators
bool SElem::operator==(const SElem &rhs) const {
    return this->combination == rhs.combination;
}

bool SElem::operator!=(const SElem &rhs) const {
    return !(*this == rhs);
}

bool SElem::operator<(const SElem &rhs) const {
    if (this->coveredEElems.size() == rhs.coveredEElems.size()) {
        return this->combination < rhs.combination;
    }

    return this->coveredEElems.size() < rhs.coveredEElems.size();
}

bool SElem::operator>(const SElem &rhs) const {
    return rhs < *this;
}

bool SElem::operator<=(const SElem &rhs) const {
    return !(*this > rhs);
}

bool SElem::operator>=(const SElem &rhs) const {
    return !(*this < rhs);
}

// ##### Getters/Setters
const AlignedBitArray &SElem::getCombination() const {
    return combination;
}

const std::set<uint32_t> &SElem::getCoveredEElems() const {
    return coveredEElems;
}

const std::set<uint32_t> &SElem::getCoveredE0Elems() const {
    return coveredE0Elems;
}

AlignedBitArray &SElem::getCombination() {
    return combination;
}

std::set<uint32_t> &SElem::getCoveredEElems() {
    return coveredEElems;
}

std::set<uint32_t> &SElem::getCoveredE0Elems() {
    return coveredE0Elems;
}

// ##### Functions
bool SElem::covers(const SElem &rhs) const {
    return combination.covers(rhs.combination);
}

bool SElem::covers(const AlignedBitArray &rhs) const {
    return combination.covers(rhs);
}

size_t SElem::countOnes() const {
    return combination.countOnes();
}