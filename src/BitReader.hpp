/**
 * @file src/BitReader.hpp
 * @brief Bit-level input stream wrapper.
 */

#pragma once

#include "Bits.hpp"

#include <istream>
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Base exception class for BitReader errors.
 */
class BitReaderException : public std::runtime_error {
public:
    explicit BitReaderException(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief Thrown when attempting to read past the end of the stream.
 */
class BitReaderEOFError : public BitReaderException {
public:
    explicit BitReaderEOFError(const std::string& msg) : BitReaderException(msg) {}
};

/**
 * @brief Thrown when an I/O error occurs during reading or seeking.
 */
class BitReaderIOError : public BitReaderException {
public:
    explicit BitReaderIOError(const std::string& msg) : BitReaderException(msg) {}
};

/**
 * @brief Wrapper around std::istream for bit-level reading.
 *
 * Bits are read most significant bit first (MSB first) within each byte.
 * Uses an internal buffer to reduce stream access.
 */
class BitReader {
public:
    /**
     * @brief Constructs a BitReader that reads from the given input stream.
     *
     * @param in              The input stream to read from.
     * @param bufferSizeBytes Internal buffer size in bytes (default 64 KiB).
     */
    explicit BitReader(std::istream& in, size_t bufferSizeBytes = 65536);

    /**
     * @brief Reads a single bit.
     *
     * @return The bit value (true = 1, false = 0).
     * @throws BitReaderEOFError if the end of the stream is reached.
     */
    bool readBit();

    /**
     * @brief Reads 8 bits as an unsigned 8-bit integer (MSB first).
     *
     * @return The decoded uint8_t value.
     * @throws BitReaderEOFError if not enough bits remain.
     */
    uint8_t readUInt8();

    /**
     * @brief Reads 16 bits as an unsigned 16-bit integer (MSB first).
     *
     * @return The decoded uint16_t value.
     * @throws BitReaderEOFError if not enough bits remain.
     */
    uint16_t readUInt16();

    /**
     * @brief Reads 32 bits as an unsigned 32-bit integer (MSB first).
     *
     * @return The decoded uint32_t value.
     * @throws BitReaderEOFError if not enough bits remain.
     */
    uint32_t readUInt32();

    /**
     * @brief Reads 64 bits as an unsigned 64-bit integer (MSB first).
     *
     * @return The decoded uint64_t value.
     * @throws BitReaderEOFError if not enough bits remain.
     */
    uint64_t readUInt64();

    /**
     * @brief Reads all remaining raw bytes from the stream.
     *
     * This bypasses bit-level buffering; the returned vector contains the
     * exact bytes that follow the current stream position.
     *
     * @return A vector of all remaining bytes.
     */
    std::vector<uint8_t> readAllData();

    /**
     * @brief Reads an arbitrary number of bits.
     *
     * @param bitsToRead Number of bits to read (0 returns an empty Bits object).
     * @return A Bits object containing the requested bits.
     * @throws BitReaderEOFError if EOF is reached before all bits are read.
     */
    Bits readBits(size_t bitsToRead);

    /**
     * @brief Checks whether the end of the stream has been reached.
     *
     * @return true if the underlying stream is at EOF and all buffered bits
     *         have been consumed.
     */
    bool eof() const;

    /**
     * @brief Resets the reader to the beginning of the stream.
     *
     * Clears error flags, seeks to position 0, and discards any buffered data.
     *
     * @throws BitReaderIOError if the seek operation fails.
     */
    void reset();

private:
    std::istream& in_;              ///< The input stream.
    std::vector<uint8_t> buffer;    ///< Internal buffer for raw bytes.
    size_t bufferSizeBytes_;        ///< Size of the internal buffer in bytes.
    size_t bitPos = 0;              ///< Current read position within the buffer (in bits).
    size_t bitsRead = 0;            ///< Number of valid bits currently in the buffer.

    /**
     * @brief Fills the internal buffer with fresh data from the stream.
     *
     * Updates bitsRead and resets bitPos. Throws BitReaderIOError if a stream
     * error occurs.
     */
    void fillBuffer();
};
