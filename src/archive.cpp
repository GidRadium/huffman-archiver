/**
 * @file src/archive.cpp
 * @brief Implementation of Huffman compression.
 */

#include "archive.hpp"
#include "BitReader.hpp"
#include "BitWriter.hpp"
#include "CodesTable.hpp"
#include "CountTable.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <ostream>
#include <queue>
#include <utility>
#include <vector>

void calculateCodesTable(const CountTable& countTable, CodesTable& codesTable)
{
    std::priority_queue<
        std::pair<size_t, std::list<uint8_t>>,
        std::vector<std::pair<size_t, std::list<uint8_t>>>,
        std::greater<> // прозрачный компаратор
        >
        huffmanUnTree;

    for (size_t byte = 0; byte < 256; ++byte)
        if (countTable[byte] > 0)
            huffmanUnTree.emplace(countTable[byte], std::list<uint8_t>(1, byte));

    if (huffmanUnTree.size() == 1) {
        std::pair<size_t, std::list<uint8_t>> onlyOneSymbol = huffmanUnTree.top();
        huffmanUnTree.pop();
        codesTable.addBits(onlyOneSymbol.second, false);
    }

    while (!huffmanUnTree.empty()) {
        std::pair<size_t, std::list<uint8_t>> smallestFirst = huffmanUnTree.top();
        huffmanUnTree.pop();
        if (huffmanUnTree.empty())
            break;

        std::pair<size_t, std::list<uint8_t>> smallestSecond = huffmanUnTree.top();
        huffmanUnTree.pop();

        codesTable.addBits(smallestFirst.second, false);
        codesTable.addBits(smallestSecond.second, true);

        smallestFirst.second.splice(smallestFirst.second.end(), smallestSecond.second);

        huffmanUnTree.emplace(
            smallestFirst.first + smallestSecond.first,
            smallestFirst.second);
    }

    codesTable.reverseAllBits();
}

void archive(std::istream& in, std::ostream& out, CompressMode mode)
{
    BitReader reader(in);
    BitWriter writer(out);
    CountTable countTable;
    CodesTable codesTable;

    std::vector<uint8_t> inData;

    try {
        switch (mode) {
        case CompressMode::SaveToRam:
            inData = reader.readAllData();
            for (uint8_t byte : inData)
                ++countTable[byte];

            break;
        case CompressMode::ReadTwice:
            while (!reader.eof())
                ++countTable[reader.readUInt8()];

            break;
            // case CompressMode::SAVE_TO_DISK:
            //  Not required
            // break;
        }
    } catch (const BitReaderIOError&) {
        throw HuffmanArchiveIStreamError("archive(): Read in stream data error.");
    } catch (const BitReaderEOFError&) {
        throw HuffmanArchiveIStreamError("archive(): Read in stream unexpected EOF.");
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
    case CompressMode::SaveToRam:
        try {
            for (uint8_t byte : inData)
                writer.writeBits(codesTable.getCode(byte));
        } catch (const HuffmanArchiveOStreamError&) {
            throw HuffmanArchiveOStreamError("archive(): Write out stream error.");
        }

        break;
    case CompressMode::ReadTwice:
        try {
            reader.reset();
        } catch (const BitReaderIOError&) {
            throw HuffmanArchiveIStreamError("archive(): Can't reset in stream to read twice.");
        }

        try {
            while (!reader.eof())
                writer.writeBits(codesTable.getCode(reader.readUInt8()));
        } catch (const BitReaderIOError&) {
            throw HuffmanArchiveIStreamError("archive(): Read in stream data error.");
        } catch (const BitReaderEOFError&) {
            throw HuffmanArchiveIStreamError("archive(): Read in stream unexpected EOF.");
        } catch (const BitWriterIOError&) {
            throw HuffmanArchiveOStreamError("archive(): Write out stream error.");
        }

        break;
        // case CompressMode::SAVE_TO_DISK:
        //  Not required
        // break;
    }

    try {
        writer.flush();
    } catch (const BitWriterIOError&) {
        throw HuffmanArchiveOStreamError("archive(): flush data to ostream error.");
    }
}
