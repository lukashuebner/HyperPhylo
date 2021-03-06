#include "EElem.h"

// ##### Constructors
EElem::EElem(size_t numBits) : combination(BitRepresentation(numBits)) {
}

EElem::EElem(BitRepresentation combination, std::set <uint32_t> coveredE0Elems) :
        combination(std::move(combination)),
        coveredE0Elems(std::move(coveredE0Elems)) {
}

EElem::EElem(SElem &&original) : combination(std::move(original.getCombination())), coveredE0Elems(std::move(original.getCoveredE0Elems())) {
}

// ##### Operators
bool operator==(const EElem &lhs, const EElem &rhs) {
    return lhs.combination == rhs.combination;
}

bool operator!=(const EElem &lhs, const EElem &rhs) {
    return !(lhs == rhs);
}

bool EElem::operator<(const EElem &rhs) const {
    return this->combination < rhs.combination;
}

bool EElem::operator>(const EElem &rhs) const {
    return rhs < *this;
}

bool EElem::operator<=(const EElem &rhs) const {
    return !(*this > rhs);
}

bool EElem::operator>=(const EElem &rhs) const {
    return !(*this < rhs);
}

// ##### Getters/Setters
const BitRepresentation &EElem::getCombination() const {
    return combination;
}

BitRepresentation &EElem::getCombination() {
    return combination;
}

std::set<uint32_t> &EElem::getCoveredE0Elems() const {
    return coveredE0Elems;
}

std::set<uint32_t> &EElem::getCoveredE0Elems() {
    return coveredE0Elems;
}

size_t EElem::countOnes() const {
    return combination.countOnes();
}
