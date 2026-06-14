/**
 * @file src/CodesTable.cpp
 * @brief Implementation of the CodesTable class.
 */

#include "CodesTable.hpp"

CodesTable::CodesTable() = default;

CodesTable::CodesTable(const Bits& bits)
{
    try {
        size_t pos = 0;
        uint8_t n = bits.toByte(pos);
        pos += 8;

        for (uint8_t i = 0; i < n; ++i) {
            uint8_t symbol = bits.toByte(pos);
            pos += 8;
            uint8_t length = bits.toByte(pos);
            pos += 8;

            if (length == 0)
                continue;

            data[symbol] = bits.slice(pos, length);
            pos += length;
        }
    } catch (const std::out_of_range& e) {
        throw CodesTableIncorrectBitsData(std::string("CodesTable::CodesTable: Bits data is incorrect. ") + e.what());
    }
}

Bits CodesTable::toBits() const
{
    size_t n = 0;
    for (const auto& entry : data)
        if (entry.size() > 0)
            ++n;

    Bits result = Bits(n);

    for (size_t sym = 0; sym < 256; ++sym) {
        const auto& entry = data[sym];
        if (entry.size() == 0)
            continue;

        result.append(Bits(sym));
        result.append(Bits(entry.size()));
        result.append(entry);
    }

    return result;
}

void CodesTable::addBits(const std::list<uint8_t>& bytes, bool bit)
{
    for (uint8_t sym : bytes) {
        Bits& entry = data[sym];
        entry.addBit(bit);
    }
}

uint8_t CodesTable::getCodeLength(uint8_t byte) const
{
    return data[byte].size();
}

const Bits& CodesTable::getCode(uint8_t byte) const
{
    return data[byte];
}

void CodesTable::reverseAllBits()
{
    for (size_t i = 0; i < 256; ++i)
        if (data[i].size() > 1)
            data[i].reverse();
}
