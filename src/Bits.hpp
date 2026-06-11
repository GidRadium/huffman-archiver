#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <stdexcept>

class Bits {
public:
    size_t bitsCount = 0;
    std::vector<uint8_t> bytes;

    void addBit(bool bit);
    bool bitAt(size_t index) const;
    void append(const Bits& other);
    Bits slice(size_t startBit, size_t length) const;
    uint8_t toByte(size_t startBit, size_t length) const;
    static Bits fromByte(uint8_t val);
};
