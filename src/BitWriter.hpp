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

// Обёртка над std::ostream для побитовой записи данных.
class BitWriter {
public:
    // Инициализирует потоком данных. Устанавливает размер буфера (64 МБ по умолчанию)
    explicit BitWriter(std::ostream& out, size_t bufferSizeBytes = 65536);

    // Вызывет fflush.
    ~BitWriter();

    // Пишет бит в поток данных.
    void writeBit(bool bit);

    // Пишет 8 бит в поток данных.
    void writeUInt8(uint8_t value);

    // Пишет 16 бит в поток данных.
    void writeUInt16(uint16_t value);

    // Пишет 32 бита в поток данных.
    void writeUInt32(uint32_t value);

    // Пишет 64 бита в поток данных.
    void writeUInt64(uint64_t value);

    // Пишет содержимое data в поток данных.
    void writeAllData(const std::vector<uint8_t>& data);

    // Пишет bits.size() бит в поток данных.
    void writeBits(const Bits& bits);

    // Отдаёт потоку данных информацию из буфера. Очищает буфер.
    void flush();

private:
    std::ostream& out_;
    std::vector<uint8_t> buffer;
    size_t bufferSizeBytes_;
    size_t bitPos = 0;

    void flushBuffer();
};
