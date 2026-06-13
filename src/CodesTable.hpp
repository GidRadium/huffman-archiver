#pragma once

#include "Bits.hpp"

#include <cstdint>
#include <array>
#include <list>
#include <stdexcept>
#include <string>

class CodesTableException : public std::runtime_error {
public:
    explicit CodesTableException(const std::string& msg) : std::runtime_error(msg) {}
};

// Таблица кодов символов.
class CodesTable {
public:
    // Конструктор по умолчанию.
    CodesTable();

    // Десеариализирует и инициализирует полученными кодами.
    CodesTable(const Bits& bits);

    // Сериализирует в последовательность бит.
    Bits toBits() const;

    // Добавляет бит в конец каждого из кодов из списка.
    void addBits(const std::list<uint8_t> &bytes, bool bit);

    // Отдаёт длину кода байта.
    uint8_t getCodeLength(uint8_t byte) const;

    // Отдаёт константную ссылку на код байта.
    const Bits& getCode(uint8_t byte) const;

    // Ставит биты кодов всех байт в обратном порядке.
    void reverseAllBits();

private:
    std::array<Bits, 256> data;
};
