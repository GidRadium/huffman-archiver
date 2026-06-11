#include "CodesTable.hpp"

CodesTable::CodesTable() = default;

CodesTable::CodesTable(Bits bits) {
    if (bits.bitsCount < 8)
        throw CodesTableException("Invalid Bits: too short for N");

    size_t pos = 0;
    uint8_t n = bits.toByte(pos, 8);
    pos += 8;

    for (uint8_t i = 0; i < n; ++i) {
        uint8_t symbol = bits.toByte(pos, 8);
        pos += 8;
        uint8_t length = bits.toByte(pos, 8);
        pos += 8;

        if (length == 0) continue;

        data[symbol] = bits.slice(pos, length);
        pos += length;
    }
}

Bits CodesTable::toBits() const {
    size_t n = 0;
    for (const auto& entry : data)
        if (entry.bitsCount > 0) ++n;

    Bits result = Bits::fromByte(static_cast<uint8_t>(n));

    for (size_t sym = 0; sym < 256; ++sym) {
        const auto& entry = data[sym];
        if (entry.bitsCount == 0) continue;

        result.append(Bits::fromByte(static_cast<uint8_t>(sym)));
        result.append(Bits::fromByte(static_cast<uint8_t>(entry.bitsCount)));
        result.append(entry);
    }

    return result;
}

void CodesTable::addBits(const std::list<uint8_t>& bytes, bool bit) {
    for (uint8_t sym : bytes) {
        Bits& entry = data[sym];
        // if (entry.bitsCount == 0) continue;
        entry.addBit(bit);
    }
}

uint8_t CodesTable::getCodeLength(uint8_t code) {
    return data[code].bitsCount;
}

void CodesTable::reverseAllBits() {
    for (size_t i = 0; i < 256; ++i)
        if (data[i].bitsCount > 1)
            data[i].reverse();
}
