#include "archive.hpp"
#include "BitReader.hpp"
#include "BitWriter.hpp"
#include "CountTable.hpp"
#include "CodesTable.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <queue>


CountTable countBytes(const std::vector<uint8_t> &inData)
{
    CountTable countTable;
    for (uint8_t byte : inData)
        countTable[byte]++;

    return countTable;
}

void calculateCodesTable(const CountTable &countTable, CodesTable &codesTable)
{
    std::priority_queue<std::pair<size_t, std::list<uint8_t>>> huffmanUnTree;
    for (size_t byte = 0; byte < 256; ++byte)
        if (countTable[byte] > 0)
            huffmanUnTree.push(std::make_pair(countTable[byte], std::list<uint8_t>(1, byte)));

    while (!huffmanUnTree.empty()) {
        std::pair<size_t, std::list<uint8_t>> smallestFirst = huffmanUnTree.top();
        huffmanUnTree.pop();
        std::pair<size_t, std::list<uint8_t>> smallestSecond = huffmanUnTree.top();
        huffmanUnTree.pop();

        codesTable.addBits(smallestFirst.second, 0);
        codesTable.addBits(smallestFirst.second, 1);

        smallestFirst.second.splice(smallestFirst.second.end(), smallestSecond.second);

        huffmanUnTree.push(
            std::make_pair(
                smallestFirst.first + smallestSecond.first,
                smallestFirst.second
            )
        );
    }
}

// read file and create CountTable
// Create CodesTable by CountTable
// Save to stream
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
}
