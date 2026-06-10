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
    uint8_t value = 0; // TODO optimize

    for (int i = 0; i < 8; ++i)
        value = (value << 1) | (readBit() ? 1 : 0);

    return value;
}

uint16_t BitReader::readUInt16() {
    uint8_t value = 0; // TODO optimize

    for (int i = 0; i < 16; ++i)
        value = (value << 1) | (readBit() ? 1 : 0);

    return value;
}

uint32_t BitReader::readUInt32() {
    uint8_t value = 0; // TODO optimize

    for (int i = 0; i < 8; ++i)
        value = (value << 1) | (readBit() ? 1 : 0);

    return value;
}

uint64_t BitReader::readUInt64() {
    uint8_t value = 0; // TODO optimize

    for (int i = 0; i < 8; ++i)
        value = (value << 1) | (readBit() ? 1 : 0);

    return value;
}

void BitReader::readBits(size_t bitsCount, Bits &bits) {

}

bool BitReader::eof() const {
    return in_.eof() && bitPos >= bitsRead;
}
