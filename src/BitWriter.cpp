// BitWriter.cpp
#include "BitWriter.hpp"

BitWriter::BitWriter(std::ostream& out) : out_(out) {}

BitWriter::~BitWriter() {
    try {
        flush();
    } catch (...) {}
}

void BitWriter::writeBit(bool bit) {
    if (bitsFilled_ == 8) {
        flushBuffer();
    }

    if (bit) {
        buffer_ |= (static_cast<uint8_t>(bit) << bitsFilled_);
    }

    ++bitsFilled_;
}

void BitWriter::writeBits(int n, uint32_t value) {
    for (int i = 0; i < n; ++i) {
        writeBit((value >> i) & 1);
    }
}

void BitWriter::writeByte(uint8_t b) {
    writeBits(8, b);
}

void BitWriter::writeUInt16(uint16_t v) {
    writeBits(16, v);
}

void BitWriter::writeUInt32(uint32_t v) {
    writeBits(32, v);
}

void BitWriter::flush() {
    if (bitsFilled_ > 0) {
        flushBuffer();
    }
    out_.flush();
    if (out_.fail()) {
        throw BitWriterIOError("BitWriter: flush failed");
    }
}

void BitWriter::flushBuffer() {
    if (bitsFilled_ == 0) return;
    char byte = static_cast<char>(buffer_);
    if (!out_.write(&byte, 1)) {
        throw BitWriterIOError("BitWriter: write error");
    }
    buffer_ = 0;
    bitsFilled_ = 0;
}
