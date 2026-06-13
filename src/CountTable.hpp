#pragma once

#include <array>
#include <cstddef>

// Таблица количества встречаемых символов.
struct CountTable : std::array<size_t, 256> {
    // Конструктор, заполняющий нулями.
    CountTable() : std::array<size_t, 256>{} {}
};
