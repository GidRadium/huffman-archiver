/**
 * @file src/Bits.hpp
 * @brief Stores a sequence of bits using std::vector<uint8_t>.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <vector>

/**
 * @brief Stores a sequence of bits using std::vector<uint8_t>.
 *
 * Bits are packed into bytes with the first bit occupying the most significant
 * bit of the first byte (MSB first).
 */
class Bits {
public:
    /**
     * @brief Default constructor. Creates an empty bit sequence.
     */
    Bits();

    /**
     * @brief Constructs a bit sequence from a raw byte array.
     *
     * Reads exactly `numBytes` bytes from `data` and stores them as `numBytes*8` bits.
     *
     * @warning `data` must point to at least `numBytes` readable bytes.
     *
     * @param data     Pointer to the source bytes.
     * @param numBytes Number of bytes to read.
     */
    Bits(const uint8_t* data, size_t numBytes);

    /**
     * @brief Constructs a bit sequence from a single byte (8 bits).
     *
     * @param byte The byte to store.
     */
    explicit Bits(uint8_t byte);

    /**
     * @brief Returns the number of bits stored.
     */
    [[nodiscard]] size_t size() const;

    /**
     * @brief Appends a single bit to the end of the sequence.
     *
     * @param bit Bit value (true = 1, false = 0).
     */
    void addBit(bool bit);

    /**
     * @brief Returns the bit at position `index`.
     *
     * @param index 0-based bit position.
     * @return Bit value (true = 1, false = 0).
     * @throws std::out_of_range if `index >= size()`.
     */
    [[nodiscard]] bool bitAt(size_t index) const;

    /**
     * @brief Returns the byte that contains bits `8*index` through `8*index+7`.
     *
     * The behaviour is guaranteed only when `8*index+8 <= size()`;
     * otherwise the returned byte may contain extra bits from the underlying storage.
     *
     * @param index Byte index (0-based).
     * @return The raw byte value.
     * @throws std::out_of_range if `index >= bytes.size()`.
     */
    [[nodiscard]] uint8_t byteAt(size_t index) const;

    /**
     * @brief Appends a copy of another bit sequence.
     */
    void append(const Bits& other);

    /**
     * @brief Returns a sub-sequence of bits.
     *
     * Copies bits from `startBit` up to `startBit+length-1`.
     *
     * @param startBit Starting bit index.
     * @param length   Number of bits to copy.
     * @return A new Bits object holding the requested range.
     * @throws std::out_of_range if `startBit+length > size()`.
     */
    [[nodiscard]] Bits slice(size_t startBit, size_t length) const;

    /**
     * @brief Reverses the order of all bits in place.
     */
    void reverse();

    /**
     * @brief Builds a byte from 8 consecutive bits starting at `startBit`.
     *
     * The first extracted bit (`startBit`) becomes the most significant bit of the result.
     *
     * @param startBit Index of the first bit.
     * @return The composed byte.
     * @throws std::out_of_range if `startBit+8 > size()`.
     */
    [[nodiscard]] uint8_t toByte(size_t startBit) const;

    /**
     * @brief Outputs the bit sequence as a string of '0' and '1'.
     */
    friend std::ostream& operator<<(std::ostream& os, const Bits& bits);

private:
    size_t bitsCount = 0; ///< Number of valid bits.
    std::vector<uint8_t> bytes; ///< Underlying byte container (bits packed MSB first).
};
