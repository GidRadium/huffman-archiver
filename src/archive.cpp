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
        if (countTable[byte] > 0)
            huffmanUnTree.push(std::make_pair(countTable[byte], std::list<uint8_t>(1, byte)));

    while (!huffmanUnTree.empty()) {
        std::pair<size_t, std::list<uint8_t>> smallestFirst = huffmanUnTree.top();
        huffmanUnTree.pop();
        if (huffmanUnTree.empty()) break;

        std::pair<size_t, std::list<uint8_t>> smallestSecond = huffmanUnTree.top();
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

void archive(std::istream &in, std::ostream &out, CompressMode mode)
{
    BitReader reader(in);
    BitWriter writer(out);
    CountTable countTable;
    CodesTable codesTable;

    std::vector<uint8_t> inData;

    switch (mode) {
        case CompressMode::SAVE_TO_RAM:
            inData = reader.readAllData();
            for (uint8_t byte : inData)
                ++countTable[byte];

            break;
        case CompressMode::READ_TWICE:
            while (!reader.eof())
                ++countTable[reader.readUInt8()];

            break;
        case CompressMode::SAVE_TO_DISK:
            // Not required
            break;
    }

    calculateCodesTable(countTable, codesTable);

    Bits codesTableAsBits = codesTable.toBits();

    writer.writeUInt32(codesTableAsBits.size());

    writer.writeBits(codesTableAsBits);

    size_t newDataSizeBits = 0;
    for (size_t byte = 0; byte < 256; ++byte)
        if (countTable[byte] > 0)
            newDataSizeBits += countTable[byte] * codesTable.getCodeLength(byte);

    writer.writeUInt64(newDataSizeBits);

    switch (mode) {
        case CompressMode::SAVE_TO_RAM:
            for (uint8_t byte : inData)
                writer.writeBits(codesTable.data[byte]);

            break;
        case CompressMode::READ_TWICE:
            reader.reset();
            while (!reader.eof())
                writer.writeBits(codesTable.data[reader.readUInt8()]);

            break;
        case CompressMode::SAVE_TO_DISK:
            // Not required
            break;
    }

    writer.flush();
}
