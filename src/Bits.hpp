#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <ostream>

// Хранит последовательность битов как std::vector<uint8_t>.
class Bits {
public:
    // Конструктор по умолчанию.
    Bits();

    // Конструтор, инициализирующий numBytes * 8 битами.
    // Возможен выход за выделенные данные.
    Bits(const uint8_t* data, size_t numBytes);

    // Инициализирует Bits 8 битами
    explicit Bits(uint8_t byte);

    // Возвращает количество хранимых битов.
    size_t size() const;

    // Добавляет бит в конец последовательности.
    void addBit(bool bit);

    // Возвращает бит на позиции index от начала.
    // Если index >= size() выбрасывает std::out_of_range.
    bool bitAt(size_t index) const;

    // Возвращает 8 бит как uint8_t начиная от 8 * index.
    uint8_t byteAt(size_t index) const;

    // Копирует последовательность битов в конец.
    void append(const Bits& other);

    // Копирует и возвращает подпоследовательность битов от startBit до startBit + length - 1 включительно.
    // Если startBit + length > size() выбрасывает std::out_of_range.
    Bits slice(size_t startBit, size_t length) const;

    // Меняет порядок битов на обратный.
    void reverse();

    // Возвращает подпоследовательность битов длины 8 как uint8_t, начиная с startBit.
    // Если startBit + 8 > size() выбрасывает std::out_of_range.
    uint8_t toByte(size_t startBit) const;

    // Выводит в поток содержимое как последовательность '0' и '1'.
    friend std::ostream& operator<<(std::ostream& os, const Bits& bits);

private:
    size_t bitsCount = 0;
    std::vector<uint8_t> bytes;
};
