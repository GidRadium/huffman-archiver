#pragma once

#include "Bits.hpp"

#include <ostream>
#include <cstdint>
#include <string>
#include <vector>

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
    explicit BitWriter(std::ostream& out, size_t bufferSizeBytes = 65536);
    ~BitWriter();

    void writeBit(bool bit);
    void writeUInt8(uint8_t value);
    void writeUInt16(uint16_t value);
    void writeUInt32(uint32_t value);
    void writeUInt64(uint64_t value);

    void writeAllData(const std::vector<uint8_t>& data);
    void writeBits(const Bits& bits);

    void flush();

private:
    std::ostream& out_;
    std::vector<uint8_t> buffer;
    size_t bufferSizeBytes_;
    size_t bitPos = 0;

    void flushBuffer();
};
