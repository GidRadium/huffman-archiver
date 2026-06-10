#pragma once

#include "Bits.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>
#include <list>

class CodesTable {
public:
    CodesTable();
    CodesTable(Bits bits);

    Bits toBits() const;
    void addBits(const std::list<uint8_t> &bytes, bool bit);

private:
    // data[byte] = <bitsCount, bitsAsUInts64>
    std::array<std::pair<size_t, std::vector<uint64_t>>, 256> data;
};
