/**
 * @file /include/huffman/archive.hpp
 * @brief Huffman compression entry point.
 */

#pragma once

#include <istream>
#include <ostream>

/**
 * @brief Compression mode for archive().
 */
enum class CompressMode {
    SaveToRam, ///< Read entire input into memory before compressing.
    ReadTwice ///< Read input twice (resets stream between passes).
};

/**
 * @brief Base exception for archive errors.
 */
class HuffmanArchiveException : public std::runtime_error {
public:
    explicit HuffmanArchiveException(const std::string& msg)
        : std::runtime_error(msg)
    {
    }
};

/**
 * @brief Thrown when a read error occurs on the input stream.
 */
class HuffmanArchiveIStreamError : public HuffmanArchiveException {
public:
    explicit HuffmanArchiveIStreamError(const std::string& msg)
        : HuffmanArchiveException(msg)
    {
    }
};

/**
 * @brief Thrown when a write error occurs on the output stream.
 */
class HuffmanArchiveOStreamError : public HuffmanArchiveException {
public:
    explicit HuffmanArchiveOStreamError(const std::string& msg)
        : HuffmanArchiveException(msg)
    {
    }
};

/**
 * @brief Compresses data from an input stream using Huffman coding.
 *
 * Reads the entire input, constructs a frequency table, builds a Huffman code
 * table, and writes the serialised code table followed by the encoded data.
 *
 * @param in   Input stream containing the original data.
 * @param out  Output stream where the compressed archive is written.
 * @param mode Compression mode (SAVE_TO_RAM or READ_TWICE).
 *
 * @throws HuffmanArchiveIStreamError  if an I/O error occurs while reading.
 * @throws HuffmanArchiveOStreamError  if an I/O error occurs while writing or flushing.
 */
void archive(std::istream& in, std::ostream& out, CompressMode mode);
