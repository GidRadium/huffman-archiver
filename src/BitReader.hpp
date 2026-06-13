#pragma once

#include "Bits.hpp"

#include <istream>
#include <cstdint>
#include <string>
#include <vector>

class BitReaderException : public std::runtime_error {
public:
    explicit BitReaderException(const std::string& msg) : std::runtime_error(msg) {}
};

class BitReaderEOFError : public BitReaderException {
public:
    explicit BitReaderEOFError(const std::string& msg) : BitReaderException(msg) {}
};

class BitReaderIOError : public BitReaderException {
public:
    explicit BitReaderIOError(const std::string& msg) : BitReaderException(msg) {}
};

class BitReader {
public:
    explicit BitReader(std::istream& in, size_t bufferSizeBytes = 65536);

    bool readBit();
    uint8_t readUInt8();
    uint16_t readUInt16();
    uint32_t readUInt32();
    uint64_t readUInt64();

    void readAllData(std::vector<uint8_t> &inData);
    Bits readBits(size_t bitsToRead);

    bool eof() const;

private:
    std::istream& in_;
    std::vector<uint8_t> buffer;
    size_t bufferSizeBytes_;
    size_t bitPos = 0;
    size_t bitsRead = 0;

    void fillBuffer();
};
