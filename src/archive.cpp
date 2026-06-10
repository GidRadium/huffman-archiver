#include "archive.hpp"
#include "BitReader.hpp"
#include <cstdint>
#include <vector>

// read file and create CountTable
// Create CodesTable by CountTable
// Save to stream
void archive(std::istream &in, std::ostream, CompressMode mode) {

    BitReader reader = BitReader(in);
    std::vector<uint8_t> inData;

    switch (mode) {
        case CompressMode::SAVE_TO_RAM:
            reader.readAllData(inData);

            break;
        case CompressMode::READ_TWICE:
            break;
        case CompressMode::SAVE_TO_DISK:
            break;
    }
}
