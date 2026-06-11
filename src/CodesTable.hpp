#pragma once

#include "Bits.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>
#include <list>
#include <stdexcept>
#include <string>

class CodesTableException : public std::runtime_error {
public:
    explicit CodesTableException(const std::string& msg) : std::runtime_error(msg) {}
};

class CodesTable {
public:
    CodesTable();
    CodesTable(Bits bits);
    Bits toBits() const;
    void addBits(const std::list<uint8_t> &bytes, bool bit);
    uint8_t getCodeLength(uint8_t code);

//private:
    std::array<Bits, 256> data;
};
