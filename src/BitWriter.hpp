// BitWriter.hpp
#pragma once

#include <ostream>
#include <cstdint>
#include <string>
#include <stdexcept>

class BitWriterException : public std::runtime_error {
public:
    explicit BitWriterException(const std::string& msg) : std::runtime_error(msg) {}
};

class BitWriterIOError : public BitWriterException {
public:
    explicit BitWriterIOError(const std::string& msg) : BitWriterException(msg) {}
};

class BitWriter {
public:
    explicit BitWriter(std::ostream& out);
    ~BitWriter();

    void writeBit(bool bit);
    void writeBits(int n, uint32_t value);

    void writeByte(uint8_t b);
    void writeUInt16(uint16_t v);
    void writeUInt32(uint32_t v);

    void flush();

private:
    std::ostream& out_;
    uint8_t buffer_ = 0;
    int bitsFilled_ = 0;

    void flushBuffer();
};
