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

    for (size_t byte = 0; byte < 256; ++byte) {
        if (countTable[byte] > 0) {
            huffmanUnTree.push(std::make_pair(countTable[byte], std::list<uint8_t>(1, byte)));
            //std::cout << (char)byte << " " << countTable[byte] << std::endl;
        }
    }

    while (!huffmanUnTree.empty()) {
        std::pair<size_t, std::list<uint8_t>> smallestFirst = huffmanUnTree.top();
        huffmanUnTree.pop();

        if (huffmanUnTree.empty()) break;

        std::pair<size_t, std::list<uint8_t>> smallestSecond = huffmanUnTree.top();

        //std::cout << smallestFirst.first << ":" << smallestFirst.second.size() << " ";
        //std::cout << smallestSecond.first << ":" << smallestSecond.second.size() << std::endl;

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

    codesTable.reverseAllBits();
}

void archive(std::istream &in, std::ostream &out, CompressMode mode) {
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

    calculateCodesTable(countTable, codesTable);

    Bits codesTableAsBits = codesTable.toBits();

    writer.writeUInt32(codesTableAsBits.bitsCount);

    writer.writeBits(codesTableAsBits);

    size_t newDataSizeBits = 0;
    for (size_t byte = 0; byte < 256; ++byte) {
        if (countTable[byte] > 0){
            newDataSizeBits += countTable[byte] * codesTable.getCodeLength(byte);
            std::cout << (char)byte << " : " << codesTable.data[byte] << std::endl;
        }
    }

    writer.writeUInt64(newDataSizeBits);

    switch (mode) {
        case CompressMode::SAVE_TO_RAM:
            for (uint8_t byte : inData) {
                writer.writeBits(codesTable.data[byte]);
                //std::cout << (char)byte << " : " << codesTable.data[byte] << std::endl;
            }

            writer.flush();

            break;
        case CompressMode::READ_TWICE:
            break;
        case CompressMode::SAVE_TO_DISK:
            break;
    }
}
