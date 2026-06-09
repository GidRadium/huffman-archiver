#pragma once

#include <istream>
#include <cstdint>
#include <string>

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
    explicit BitReader(std::istream& in);

    bool readBit();

    uint32_t readBits(int n);

    uint8_t  readByte();
    uint16_t readUInt16();
    uint32_t readUInt32();

    bool eof() const;

private:
    std::istream& in_;
    uint8_t buffer_ = 0;
    int bitsLeft_ = 0;

    void fillBuffer();
};
