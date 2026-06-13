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
    // Инициализирует потоком данных. Устанавливает размер буфера (64 МБ по умолчанию)
    explicit BitReader(std::istream& in, size_t bufferSizeBytes = 65536);

    // Читает один бит из потока
    bool readBit();

    // Читает 8 бит как uint_8t. Возможен BitReaderEOFError.
    uint8_t readUInt8();

    // Читает 16 бит как uint_16t. Возможен BitReaderEOFError.
    uint16_t readUInt16();

    // Читает 32 бит как uint_32t. Возможен BitReaderEOFError.
    uint32_t readUInt32();

    // Читает 64 бит как uint_64t. Возможен BitReaderEOFError.
    uint64_t readUInt64();

    // Читает и возвращает все данные из потока.
    std::vector<uint8_t> readAllData();

    // Читает произвольное количество бит. Возможен BitReaderEOFError.
    Bits readBits(size_t bitsToRead);

    // Возвращает true, если достигнут конец потока.
    bool eof() const;

    // Сбрасывает буфер, начинает читать с начала потока. Возможен BitReaderIOError.
    void reset();

private:
    std::istream& in_;
    std::vector<uint8_t> buffer;
    size_t bufferSizeBytes_;
    size_t bitPos = 0;
    size_t bitsRead = 0;

    void fillBuffer();
};
