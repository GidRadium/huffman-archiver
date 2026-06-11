#include "archive.hpp"
#include "BitReader.hpp"
#include "BitWriter.hpp"
#include "CountTable.hpp"
#include "CodesTable.hpp"

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <utility>
#include <vector>
#include <queue>
#include <functional>

#include <iostream> // debug

CountTable countBytes(const std::vector<uint8_t> &inData)
{
    CountTable countTable;
    for (uint8_t byte : inData)
        countTable[byte]++;

    return countTable;
}

void calculateCodesTable(const CountTable &countTable, CodesTable &codesTable)
{
    std::priority_queue<
        std::pair<size_t, std::list<uint8_t>>,
        std::vector<std::pair<size_t, std::list<uint8_t>>>,
        std::greater<std::pair<size_t, std::list<uint8_t>>>
    > huffmanUnTree;
    for (size_t byte = 0; byte < 256; ++byte)
        if (countTable[byte] > 0) {
            huffmanUnTree.push(std::make_pair(countTable[byte], std::list<uint8_t>(1, byte)));
            std::cout << (char)byte << " " << countTable[byte] << std::endl;
        }

    while (!huffmanUnTree.empty()) {
        std::pair<size_t, std::list<uint8_t>> smallestFirst = huffmanUnTree.top();
        huffmanUnTree.pop();

        if (huffmanUnTree.empty()) break;

        std::pair<size_t, std::list<uint8_t>> smallestSecond = huffmanUnTree.top();

        std::cout << smallestFirst.first << ":" << smallestFirst.second.size() << " ";
        std::cout << smallestSecond.first << ":" << smallestSecond.second.size() << std::endl;

        huffmanUnTree.pop();

        codesTable.addBits(smallestFirst.second, 0);
        codesTable.addBits(smallestSecond.second, 1);

        smallestFirst.second.splice(smallestFirst.second.end(), smallestSecond.second);

        huffmanUnTree.push(
            std::make_pair(
                smallestFirst.first + smallestSecond.first,
                smallestFirst.second
            )
        );
    }
}

void archive(std::istream &in, std::ostream &out, CompressMode mode) {

    std::cout << "1" << std::endl;

    BitReader reader(in);
    BitWriter writer(out);
    CountTable countTable;
    CodesTable codesTable;

    std::vector<uint8_t> inData;

    switch (mode) {
        case CompressMode::SAVE_TO_RAM:
            reader.readAllData(inData);
            countTable = countBytes(inData);
            break;
        case CompressMode::READ_TWICE:
            break;
        case CompressMode::SAVE_TO_DISK:
            break;
    }

    std::cout << "2" << std::endl;

    calculateCodesTable(countTable, codesTable);

    std::cout << "3" << std::endl;

    Bits codesTableAsBits = codesTable.toBits();

    std::cout << "4" << std::endl;

    writer.writeUInt32(codesTableAsBits.bitsCount);

    std::cout << "5" << std::endl;

    writer.writeBits(codesTableAsBits);

    std::cout << "6" << std::endl;

    size_t newDataSizeBits = 0;
    for (size_t i = 0; i < 256; ++i)
        newDataSizeBits += countTable[i] * codesTable.getCodeLength(i);

    std::cout << "7" << std::endl;

    writer.writeUInt64(newDataSizeBits);

    std::cout << "8" << std::endl;

    switch (mode) {
        case CompressMode::SAVE_TO_RAM:
            for (uint8_t byte : inData) {
                writer.writeBits(codesTable.data[byte]);
                std::cout << (char)byte << " : " << codesTable.data[byte].bitsCount << std::endl;
            }

            writer.flush();

            break;
        case CompressMode::READ_TWICE:
            break;
        case CompressMode::SAVE_TO_DISK:
            break;
    }
}
