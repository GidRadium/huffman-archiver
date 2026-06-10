#pragma once

#include <array>
#include <cstddef>

struct CountTable : std::array<size_t, 256> {
    CountTable() : std::array<size_t, 256>{} {}
};
