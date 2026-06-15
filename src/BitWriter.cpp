/**
 * @file src/BitWriter.cpp
 * @brief Implementation of the BitWriter class.
 */

#include "BitWriter.hpp"

BitWriter::BitWriter(std::ostream& out, size_t bufferSizeBytes)
    : out_(out)
    , bufferSizeBytes_(bufferSizeBytes)
    , bitPos(0)
{
    buffer.clear();
    buffer.resize(bufferSizeBytes, 0);
}

BitWriter::~BitWriter()
{
    try {
        flush();
    } catch (...) {
    }
}

void BitWriter::flushBuffer()
{
    size_t bytesToWrite = (bitPos + 7) >> 3;
    if (bytesToWrite == 0)
        return;

    out_.write(reinterpret_cast<const char*>(buffer.data()), static_cast<std::streamsize>(bytesToWrite));

    if (out_.bad())
        throw BitWriterIOError("BitWriter: I/O error while writing to stream");

    std::fill(buffer.begin(), buffer.begin() + static_cast<std::streamsize>(bytesToWrite), 0);
    bitPos = 0;
}

void BitWriter::writeBit(bool bit)
{
    if (bitPos >= bufferSizeBytes_ << 3)
        flushBuffer();

    if (bit)
        buffer[bitPos >> 3] |= uint8_t(1u << (7 - (bitPos & 7)));

    ++bitPos;
}

void BitWriter::writeUInt8(uint8_t value)
{
    if ((bitPos & 7) == 0 && bitPos + 8 <= (bufferSizeBytes_ << 3)) {
        buffer[bitPos >> 3] = value;
        bitPos += 8;

        return;
    }

    for (int i = 7; i >= 0; --i)
        writeBit((value >> i) & 1);
}

void BitWriter::writeUInt16(uint16_t value)
{
    if ((bitPos & 7) == 0 && bitPos + 16 <= (bufferSizeBytes_ << 3)) {
        size_t bytePos = bitPos >> 3;

        buffer[bytePos] = uint8_t(value >> 8);
        buffer[bytePos + 1] = uint8_t(value);

        bitPos += 16;
        return;
    }

    for (int i = 15; i >= 0; --i)
        writeBit((value >> i) & 1);
}

void BitWriter::writeUInt32(uint32_t value)
{
    if ((bitPos & 7) == 0 && bitPos + 32 <= (bufferSizeBytes_ << 3)) {
        size_t bytePos = bitPos >> 3;

        buffer[bytePos] = uint8_t(value >> 24);
        buffer[bytePos + 1] = uint8_t(value >> 16);
        buffer[bytePos + 2] = uint8_t(value >> 8);
        buffer[bytePos + 3] = uint8_t(value);

        bitPos += 32;
        return;
    }

    for (int i = 31; i >= 0; --i)
        writeBit((value >> i) & 1);
}

void BitWriter::writeUInt64(uint64_t value)
{
    if ((bitPos & 7) == 0 && bitPos + 64 <= (bufferSizeBytes_ << 3)) {
        size_t bytePos = bitPos >> 3;

        buffer[bytePos] = uint8_t(value >> 56);
        buffer[bytePos + 1] = uint8_t(value >> 48);
        buffer[bytePos + 2] = uint8_t(value >> 40);
        buffer[bytePos + 3] = uint8_t(value >> 32);
        buffer[bytePos + 4] = uint8_t(value >> 24);
        buffer[bytePos + 5] = uint8_t(value >> 16);
        buffer[bytePos + 6] = uint8_t(value >> 8);
        buffer[bytePos + 7] = uint8_t(value);

        bitPos += 64;
        return;
    }

    for (int i = 63; i >= 0; --i)
        writeBit((value >> i) & 1);
}

void BitWriter::writeAllData(const std::vector<uint8_t>& data)
{
    if (data.empty())
        return;

    if (bitPos != 0)
        flush();

    out_.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));

    if (out_.bad())
        throw BitWriterIOError("I/O error while writing to stream");
}

void BitWriter::writeBits(const Bits& bits)
{
    size_t remaining = bits.size();

    size_t bytePos = 0;
    while (remaining >= 8) {
        writeUInt8(bits.byteAt(bytePos));
        ++bytePos;
        remaining -= 8;
    }

    if (remaining > 0) {
        uint8_t lastByte = bits.byteAt(bytePos);

        for (size_t i = 0; i < remaining; ++i)
            writeBit((lastByte >> (7 - i)) & 1);
    }
}

void BitWriter::flush()
{
    flushBuffer();
    out_.flush();

    if (out_.bad())
        throw BitWriterIOError("BitWriter::flush: I/O error while flushing stream");
}
