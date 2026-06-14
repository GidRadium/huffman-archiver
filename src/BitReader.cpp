/**
 * @file src/BitReader.cpp
 * @brief Implementation of the BitReader class.
 */

#include "BitReader.hpp"

BitReader::BitReader(std::istream& in, size_t bufferSizeBytes)
    : in_(in), bufferSizeBytes_(bufferSizeBytes), bitPos(0), bitsRead(0)
{
    buffer.clear();
    buffer.resize(bufferSizeBytes, 0);
}

std::vector<uint8_t> BitReader::readAllData()
{
    std::vector<uint8_t> inData;
    while (in_.read(reinterpret_cast<char*>(buffer.data()), buffer.size())
        || in_.gcount() > 0) {
        auto bytes = in_.gcount();
        inData.insert(inData.end(), buffer.data(), buffer.data() + bytes);
    }

    return inData;
}

void BitReader::fillBuffer()
{
    in_.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    if (in_.bad())
        throw BitReaderIOError("BitReader: I/O error while reading from stream");

    bitsRead = size_t(in_.gcount()) << 3;
    bitPos = 0;
}

bool BitReader::readBit()
{
    if (bitPos >= bitsRead) {
        fillBuffer();
        if (bitsRead == 0)
            throw BitReaderEOFError("BitReader::readBit: EOF");
    }

    bool bit = (buffer[bitPos >> 3] >> (7 - (bitPos & 7))) & 1; // TODO optimize
    ++bitPos;

    return bit;
}

uint8_t BitReader::readUInt8()
{
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

uint16_t BitReader::readUInt16()
{
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

uint32_t BitReader::readUInt32()
{
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

uint64_t BitReader::readUInt64()
{
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

Bits BitReader::readBits(size_t bitsToRead)
{
    Bits result;
    if (bitsToRead == 0)
        return result;

    size_t remaining = bitsToRead;

    while (remaining > 0) {
        if (bitPos >= bitsRead) {
            fillBuffer();
            if (bitsRead == 0)
                throw BitReaderEOFError("BitReader::readBits: EOF");
        }

        if ((bitPos & 7) == 0 && remaining >= 8 && (bitsRead - bitPos) >= 8) {
            size_t bytesAvailable = (bitsRead - bitPos) >> 3;
            size_t bytesToCopy = std::min(bytesAvailable, remaining >> 3);
            Bits chunk(buffer.data() + (bitPos >> 3), bytesToCopy);

            if (result.size() == 0) result = chunk;
            else result.append(chunk);

            bitPos += bytesToCopy << 3;
            remaining -= bytesToCopy << 3;
        } else {
            result.addBit(readBit());
            --remaining;
        }
    }

    return result;
}

bool BitReader::eof() const
{
    return in_.eof() && bitPos >= bitsRead;
}

void BitReader::reset()
{
    in_.clear();
    if (!in_.seekg(0, std::ios::beg))
        throw BitReaderIOError("BitReader::reset: Failed to seek to beginning of stream");

    bitPos = 0;
    bitsRead = 0;
}
