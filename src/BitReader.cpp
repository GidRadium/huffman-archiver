#include "BitReader.hpp"

BitReader::BitReader(std::istream& in) : in_(in) {}

bool BitReader::readBit() {
    if (bitsLeft_ == 0)
        fillBuffer();
    bool bit = buffer_ & 1;
    buffer_ >>= 1;
    --bitsLeft_;
    return bit;
}

uint32_t BitReader::readBits(int n) {
    uint32_t result = 0;

    for (int i = 0; i < n; ++i) {
        if (readBit())
            result |= (uint32_t(1) << i);
    }
    return result;
}

uint8_t BitReader::readByte() {
    return static_cast<uint8_t>(readBits(8));
}

uint16_t BitReader::readUInt16() {
    return static_cast<uint16_t>(readBits(16));
}

uint32_t BitReader::readUInt32() {
    return readBits(32);
}

bool BitReader::eof() const {
    return in_.eof() && bitsLeft_ == 0;
}

void BitReader::fillBuffer() {
    char byte = 0;
    if (!in_.get(byte)) {
        if (in_.eof())
            throw BitReaderEOFError("BitReader: unexpected end of file");
        else
            throw BitReaderIOError("BitReader: stream read error");
    }

    buffer_ = static_cast<uint8_t>(byte);
    bitsLeft_ = 8;
}
