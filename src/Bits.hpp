#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <ostream>

class Bits {
public:
    size_t bitsCount = 0;
    std::vector<uint8_t> bytes;

    void addBit(bool bit);
    bool bitAt(size_t index) const;
    void append(const Bits& other);
    Bits slice(size_t startBit, size_t length) const;
    void reverse();
    uint8_t toByte(size_t startBit, size_t length) const;
    static Bits fromByte(uint8_t val);

    friend std::ostream& operator<<(std::ostream& os, const Bits& bits);
};
