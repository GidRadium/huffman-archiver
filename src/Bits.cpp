#include "Bits.hpp"

void Bits::addBit(bool bit) {
    size_t bitIndex = bitsCount;
    if ((bitIndex & 7) == 0) {
        bytes.push_back(bit ? (1 << 7) : 0);
    } else {
        size_t byteIdx = bitIndex >> 3;
        size_t shift = 7 - (bitIndex & 7);
        if (bit)
            bytes[byteIdx] |= (1 << shift);
        else
            bytes[byteIdx] &= ~(1 << shift);
    }

    ++bitsCount;
}

bool Bits::bitAt(size_t index) const {
    return (bytes[index >> 3] >> (7 - (index & 7))) & 1;
}

void Bits::append(const Bits& other) {
    for (size_t i = 0; i < other.bitsCount; ++i)
        addBit(other.bitAt(i));
}

Bits Bits::slice(size_t startBit, size_t length) const {
    if (startBit + length > bitsCount)
        throw std::runtime_error("slice out of range");
    Bits result;
    for (size_t i = 0; i < length; ++i)
        result.addBit(bitAt(startBit + i));
    return result;
}

void Bits::reverse() {
    if (bitsCount <= 1)
        return;

    Bits reversed;
    for (size_t i = bitsCount; i-- > 0; )
        reversed.addBit(bitAt(i));

    bytes = std::move(reversed.bytes);
    bitsCount = reversed.bitsCount;
}

uint8_t Bits::toByte(size_t startBit, size_t length) const {
    if (length > 8 || startBit + length > bitsCount)
        throw std::runtime_error("toByte out of range");
    uint8_t val = 0;
    for (size_t i = 0; i < length; ++i)
        val = (val << 1) | (bitAt(startBit + i) ? 1 : 0);
    return val;
}

Bits Bits::fromByte(uint8_t val) {
    Bits b;
    for (int i = 7; i >= 0; --i)
        b.addBit((val >> i) & 1);
    return b;
}

std::ostream& operator<<(std::ostream& os, const Bits& bits) {
    for (size_t i = 0; i < bits.bitsCount; ++i)
        os << (bits.bitAt(i) ? '1' : '0');

    return os;
}
