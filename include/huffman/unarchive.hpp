/**
 * @file /include/huffman/unarchive.hpp
 * @brief Huffman decompression entry point.
 */

#pragma once

#include <istream>
#include <ostream>

/**
 * @brief Base exception for unarchive errors.
 */
class HuffmanUnArchiveException : public std::runtime_error {
public:
    explicit HuffmanUnArchiveException(const std::string& msg)
        : std::runtime_error(msg)
    {
    }
};

/**
 * @brief Thrown when a read error occurs on the input stream.
 */
class HuffmanUnArchiveIStreamError : public HuffmanUnArchiveException {
public:
    explicit HuffmanUnArchiveIStreamError(const std::string& msg)
        : HuffmanUnArchiveException(msg)
    {
    }
};

/**
 * @brief Thrown when a write error occurs on the output stream.
 */
class HuffmanUnArchiveOStreamError : public HuffmanUnArchiveException {
public:
    explicit HuffmanUnArchiveOStreamError(const std::string& msg)
        : HuffmanUnArchiveException(msg)
    {
    }
};

/**
 * @brief Thrown when the compressed data is malformed or truncated.
 */
class HuffmanUnArchiveBrokenDataError : public HuffmanUnArchiveException {
public:
    explicit HuffmanUnArchiveBrokenDataError(const std::string& msg)
        : HuffmanUnArchiveException(msg)
    {
    }
};

/**
 * @brief Decompresses a Huffman‑coded archive.
 *
 * Reads the code table and the encoded bit stream from the input, reconstructs
 * the Huffman tree, and writes the decoded bytes to the output stream.
 *
 * @param in  Input stream containing the archive.
 * @param out Output stream for the decompressed data.
 *
 * @throws HuffmanUnArchiveIStreamError   if an I/O error occurs while reading.
 * @throws HuffmanUnArchiveOStreamError   if an I/O error occurs while writing or flushing.
 * @throws HuffmanUnArchiveBrokenDataError if the archive structure is invalid.
 */
void unarchive(std::istream& in, std::ostream& out);
