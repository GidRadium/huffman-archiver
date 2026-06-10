#include "archive.hpp"
#include "BitReader.hpp"
#include "CountTable.hpp"
#include "CodesTable.hpp"
#include <cstdint>
#include <vector>

CountTable countBytes(const std::vector<uint8_t> &inData) {
    CountTable countTable;
    for (uint8_t byte : inData)
        countTable[byte]++;

    return countTable;
}

void calculateCodesTable(const CountTable &countTable, CodesTable &codesTable) {

}

// read file and create CountTable
// Create CodesTable by CountTable
// Save to stream
void archive(std::istream &in, std::ostream &out, CompressMode mode) {
    BitReader reader = BitReader(in);
    std::vector<uint8_t> inData;
    CountTable countTable;

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
}
