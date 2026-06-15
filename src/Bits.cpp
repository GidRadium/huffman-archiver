/**
 * @file src/Bits.cpp
 * @brief Implementation of the Bits class.
 */

#include "Bits.hpp"

Bits::Bits() = default;

Bits::Bits(const uint8_t* data, size_t numBytes)
    : bytes(data, data + numBytes)
    , bitsCount(numBytes * 8)
{
}

Bits::Bits(uint8_t byte)
    : bitsCount(8)
{
    bytes.push_back(byte);
}

size_t Bits::size() const
{
    return bitsCount;
}

void Bits::addBit(bool bit)
{
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

bool Bits::bitAt(size_t index) const
{
    if (index >= bitsCount)
        throw std::out_of_range("Bits::bitAt: index out of range");

    return (bytes[index >> 3] >> (7 - (index & 7))) & 1;
}

uint8_t Bits::byteAt(size_t index) const
{
    if (index >= bytes.size())
        throw std::out_of_range("Bits::byteAt: index out of range");

    return bytes[index];
}

void Bits::append(const Bits& other)
{
    for (size_t i = 0; i < other.bitsCount; ++i)
        addBit(other.bitAt(i));
}

Bits Bits::slice(size_t startBit, size_t length) const
{
    if (startBit + length > bitsCount)
        throw std::out_of_range("Bits::slice: startBit + length out of range");

    Bits result;
    for (size_t i = 0; i < length; ++i)
        result.addBit(bitAt(startBit + i));

    return result;
}

void Bits::reverse()
{
    if (bitsCount < 2)
        return;

    Bits reversed;
    for (size_t i = bitsCount; i-- > 0;)
        reversed.addBit(bitAt(i));

    bytes = std::move(reversed.bytes);
    bitsCount = reversed.bitsCount;
}

uint8_t Bits::toByte(size_t startBit) const
{
    if (startBit + 8 > bitsCount)
        throw std::out_of_range("Bits::toByte: startBit + 8 out of range");

    uint8_t byte = 0;
    for (size_t i = 0; i < 8; ++i)
        byte = (byte << 1) | (bitAt(startBit + i) ? 1 : 0);

    return byte;
}

std::ostream& operator<<(std::ostream& os, const Bits& bits)
{
    for (size_t i = 0; i < bits.bitsCount; ++i)
        os << (bits.bitAt(i) ? '1' : '0');

    return os;
}
