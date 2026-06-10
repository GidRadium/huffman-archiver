#include "BitReader.hpp"

BitReader::BitReader(std::istream& in, size_t bufferSizeBytes)
    : in_(in), bufferSizeBytes_(bufferSizeBytes), bitPos(0), bitsRead(0) {
    buffer.clear();
    buffer.resize(bufferSizeBytes, 0);
}

void BitReader::readAllData(std::vector<uint8_t> &inData) {
    while (in_.read(reinterpret_cast<char*>(buffer.data()), buffer.size())
        || in_.gcount() > 0) {
        auto bytes = in_.gcount();
        inData.insert(inData.end(), buffer.data(), buffer.data() + bytes);
    }
}

void BitReader::fillBuffer() {
    in_.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    if (in_.bad())
        throw BitReaderIOError("I/O error while reading from stream");

    bitsRead = size_t(in_.gcount()) << 3;
    bitPos = 0;
}

bool BitReader::readBit() {
    if (bitPos >= bitsRead) {
        fillBuffer();
        if (bitsRead == 0) {
            throw BitReaderEOFError("EOF by readBit");
        }
    }

    bool bit = (buffer[bitPos >> 3] >> (7 - (bitPos & 7))) & 1; // TODO optimize
    ++bitPos;

    return bit;
}

uint8_t BitReader::readUInt8() {
    if ((bitPos & 7) == 0 && bitsRead - bitPos >= 8) {
        uint8_t value = buffer[bitPos >> 3];
        bitPos += 8;

        return value;
    }

    uint8_t value = 0;
    for (int i = 0; i < 8; ++i)
        value = uint8_t((value << 1) | (readBit() ? 1 : 0));

    return value;
}

uint16_t BitReader::readUInt16() {
    if ((bitPos & 7) == 0 && bitsRead - bitPos >= 16) {
        size_t bytePos = bitPos >> 3;
        uint16_t value = (uint16_t(buffer[bytePos]) << 8) | buffer[bytePos + 1];
        bitPos += 16;

        return value;
    }

    uint16_t value = 0;
    for (int i = 0; i < 16; ++i)
        value = uint16_t((value << 1) | (readBit() ? 1 : 0));

    return value;
}

uint32_t BitReader::readUInt32() {
    if ((bitPos & 7) == 0 && bitsRead - bitPos >= 32) {
        size_t bytePos = bitPos >> 3;
        uint32_t value = (uint32_t(buffer[bytePos]) << 24)
            | (uint32_t(buffer[bytePos + 1]) << 16)
            | (uint32_t(buffer[bytePos + 2]) << 8)
            | buffer[bytePos + 3];
        bitPos += 32;

        return value;
    }

    uint32_t value = 0;
    for (int i = 0; i < 32; ++i)
        value = (value << 1) | (readBit() ? 1 : 0);

    return value;
}

uint64_t BitReader::readUInt64() {
    if ((bitPos & 7) == 0 && bitsRead - bitPos >= 64) {
        size_t bytePos = bitPos >> 3;
        uint64_t value = (uint64_t(buffer[bytePos]) << 56)
            | (uint64_t(buffer[bytePos + 1]) << 48)
            | (uint64_t(buffer[bytePos + 2]) << 40)
            | (uint64_t(buffer[bytePos + 3]) << 32)
            | (uint64_t(buffer[bytePos + 4]) << 24)
            | (uint64_t(buffer[bytePos + 5]) << 16)
            | (uint64_t(buffer[bytePos + 6]) << 8)
            | buffer[bytePos + 7];
        bitPos += 64;

        return value;
    }

    uint64_t value = 0;
    for (int i = 0; i < 64; ++i)
        value = (value << 1) | (readBit() ? 1 : 0);

    return value;
}

void BitReader::readBits(size_t bitsCount, Bits &bits) {
    if (bitsCount == 0)
        return;

    size_t remaining = bitsCount;

    while (remaining > 0 && (bitPos & 7) == 0 && bitsRead - bitPos >= 8) {
        size_t bytesAvailable = (bitsRead - bitPos) >> 3;
        size_t bytesToCopy = std::min(bytesAvailable, remaining >> 3);
        if (bytesToCopy == 0)
            break;

        const uint8_t* src = buffer.data() + (bitPos >> 3);
        bits.bytes.insert(bits.bytes.end(), src, src + bytesToCopy);
        bitPos += bytesToCopy << 3;
        remaining -= bytesToCopy << 3;
    }

    uint8_t lastByte = 0;
    int bitsInLastByte = 0;
    while (remaining > 0) {
        if (bitsInLastByte == 8) {
            bits.bytes.push_back(lastByte);
            lastByte = 0;
            bitsInLastByte = 0;
        }

        lastByte = uint8_t((lastByte << 1) | (readBit() ? 1 : 0));
        ++bitsInLastByte;
        --remaining;
    }

    if (bitsInLastByte > 0) {
        lastByte <<= (8 - bitsInLastByte);
        bits.bytes.push_back(lastByte);
    }

    bits.bitsCount += bitsCount;
}

bool BitReader::eof() const {
    return in_.eof() && bitPos >= bitsRead;
}
