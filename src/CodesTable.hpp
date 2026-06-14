/**
 * @file src/CodesTable.hpp
 * @brief Huffman code table for symbols 0–255.
 */

#pragma once

#include "Bits.hpp"

#include <cstdint>
#include <array>
#include <list>
#include <stdexcept>
#include <string>

/**
 * @brief Base exception class for CodesTable errors.
 */
class CodesTableException : public std::runtime_error {
public:
    explicit CodesTableException(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief Thrown when the provided bit data cannot be parsed into a valid code table.
 */
class CodesTableIncorrectBitsData : public CodesTableException {
public:
    explicit CodesTableIncorrectBitsData(const std::string& msg) : CodesTableException(msg) {}
};

/**
 * @brief Stores a code table for symbols (0–255), each associated with a bit sequence.
 *
 * Codes are stored as Bits objects. The table can be serialised to a compact bit
 * representation and deserialised back.
 */
class CodesTable {
public:
    /**
     * @brief Default constructor. Creates an empty table (all code lengths are 0).
     */
    CodesTable();

    /**
     * @brief Deserialises the table from a bit sequence.
     *
     * Expected format:
     *   - 8 bits: number of entries N
     *   - For each entry:
     *       - 8 bits: symbol
     *       - 8 bits: code length L
     *       - L bits: code
     *
     * Entries with L=0 are skipped.
     *
     * @param bits The bit sequence containing the table.
     * @throws CodesTableIncorrectBitsData if the data is truncated or malformed.
     */
    CodesTable(const Bits& bits);

    /**
     * @brief Serialises the table into a Bits object using the compact format.
     *
     * Only non‑empty codes are written.
     *
     * @return The serialised table.
     */
    Bits toBits() const;

    /**
     * @brief Appends a bit to the end of the codes of the given symbols.
     *
     * @param bytes List of symbols whose codes will be extended.
     * @param bit   The bit to append (true = 1, false = 0).
     */
    void addBits(const std::list<uint8_t>& bytes, bool bit);

    /**
     * @brief Returns the length of the code for a given symbol.
     *
     * @param byte The symbol.
     * @return The code length in bits (0 if the symbol has no code).
     */
    uint8_t getCodeLength(uint8_t byte) const;

    /**
     * @brief Returns a constant reference to the code for a given symbol.
     *
     * @param byte The symbol.
     * @return The code as a Bits object.
     */
    const Bits& getCode(uint8_t byte) const;

    /**
     * @brief Reverses the bit order of every code that has more than one bit.
     */
    void reverseAllBits();

private:
    std::array<Bits, 256> data;   ///< Codes indexed by symbol (0–255).
};
