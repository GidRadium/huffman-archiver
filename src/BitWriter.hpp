/**
 * @file src/BitWriter.hpp
 * @brief Bit-level output stream wrapper.
 */

#pragma once

#include "Bits.hpp"

#include <ostream>
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Base exception class for BitWriter errors.
 */
class BitWriterException : public std::runtime_error {
public:
    explicit BitWriterException(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief Thrown when an I/O error occurs during writing or flushing.
 */
class BitWriterIOError : public BitWriterException {
public:
    explicit BitWriterIOError(const std::string& msg) : BitWriterException(msg) {}
};

/**
 * @brief Wrapper around std::ostream for bit-level writing.
 *
 * Bits are written most significant bit first (MSB first) within each byte.
 * Uses an internal buffer to minimise stream operations.
 */
class BitWriter {
public:
    /**
     * @brief Constructs a BitWriter that writes to the given output stream.
     *
     * @param out             The output stream to write to.
     * @param bufferSizeBytes Internal buffer size in bytes (default 64 KiB).
     */
    explicit BitWriter(std::ostream& out, size_t bufferSizeBytes = 65536);

    /**
     * @brief Destructor. Flushes buffered data; exceptions are caught and ignored.
     */
    ~BitWriter();

    /**
     * @brief Writes a single bit.
     *
     * @param bit Bit value (true = 1, false = 0).
     */
    void writeBit(bool bit);

    /**
     * @brief Writes an 8-bit unsigned integer (MSB first).
     *
     * @param value The value to write.
     */
    void writeUInt8(uint8_t value);

    /**
     * @brief Writes a 16-bit unsigned integer (MSB first).
     *
     * @param value The value to write.
     */
    void writeUInt16(uint16_t value);

    /**
     * @brief Writes a 32-bit unsigned integer (MSB first).
     *
     * @param value The value to write.
     */
    void writeUInt32(uint32_t value);

    /**
     * @brief Writes a 64-bit unsigned integer (MSB first).
     *
     * @param value The value to write.
     */
    void writeUInt64(uint64_t value);

    /**
     * @brief Writes raw bytes directly to the stream.
     *
     * Any buffered partial bits are flushed before the raw bytes are written.
     *
     * @param data The bytes to write.
     */
    void writeAllData(const std::vector<uint8_t>& data);

    /**
     * @brief Writes all bits from a Bits object.
     *
     * @param bits The bits to write.
     */
    void writeBits(const Bits& bits);

    /**
     * @brief Flushes the internal bit buffer and the underlying stream.
     *
     * @throws BitWriterIOError if a stream error occurs.
     */
    void flush();

private:
    std::ostream& out_;              ///< The output stream.
    std::vector<uint8_t> buffer;     ///< Internal buffer for accumulating bits.
    size_t bufferSizeBytes_;         ///< Size of the internal buffer in bytes.
    size_t bitPos = 0;               ///< Current write position within the buffer (in bits).

    /**
     * @brief Writes full bytes from the buffer to the output stream and resets the buffer.
     */
    void flushBuffer();
};
