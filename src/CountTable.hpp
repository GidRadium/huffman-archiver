/**
 * @file src/CountTable.hpp
 * @brief Frequency table for 256 symbols.
 */

#pragma once

#include <array>
#include <cstddef>

/**
 * @brief Frequency table for 256 symbols.
 *
 * Inherits from std::array<size_t, 256> and zero-initialises all counts.
 */
struct CountTable : std::array<size_t, 256> {
    /**
     * @brief Default constructor. Sets all symbol counts to zero.
     */
    CountTable()
        : std::array<size_t, 256> {}
    {
    }
};
