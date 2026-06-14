/**
 * @file src/unarchive.cpp
 * @brief Implementation of Huffman decompression.
 */

#include "unarchive.hpp"
#include "BitReader.hpp"
#include "BitWriter.hpp"
#include "CodesTable.hpp"

#include <cstdint>
#include <memory>

struct Node {
    bool isLeaf = false;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    uint8_t byte = 0;
};

std::unique_ptr<Node> buildTree(const CodesTable& codesTable)
{
    std::unique_ptr<Node> root = std::make_unique<Node>();

    for (size_t byte = 0; byte < 256; ++byte) {
        const Bits& code = codesTable.getCode(byte);
        if (code.size() == 0) continue;

        Node* current = root.get();
        for (size_t i = 0; i < code.size(); ++i) {
            bool bit = code.bitAt(i);
            if (bit == 0) {
                if (!current->left)
                    current->left = std::make_unique<Node>();
                current = current->left.get();
            } else {
                if (!current->right)
                    current->right = std::make_unique<Node>();
                current = current->right.get();
            }
        }
        current->isLeaf = true;
        current->byte = static_cast<uint8_t>(byte);
    }

    return root;
}

void unarchive(std::istream& in, std::ostream& out)
{
    BitReader reader(in);
    BitWriter writer(out);
    uint32_t codesTableBitsCount;
    Bits codesTableAsBits;
    CodesTable codesTable;

    try {
        codesTableBitsCount = reader.readUInt32();
        codesTableAsBits = reader.readBits(codesTableBitsCount);
        codesTable = CodesTable(codesTableAsBits);
    } catch (CodesTableIncorrectBitsData) {
        throw HuffmanUnArchiveBrokenDataError("unarchive(): istream CodesTable is incorrect.");
    } catch (BitReaderIOError) {
        throw HuffmanUnArchiveIStreamError("unarchive(): Read istream data error.");
    } catch (BitReaderEOFError) {
        throw HuffmanUnArchiveBrokenDataError("unarchive(): Read istream unexpected EOF.");
    }

    std::unique_ptr<Node> root = buildTree(codesTable);

    try {
        Node* currentNode = root.get();
        uint64_t inDataSizeBits = reader.readUInt64();
        uint64_t bitPos = 0;
        while (bitPos < inDataSizeBits) {
            bool currentBit = reader.readBit();
            if (!currentBit) {
                if (currentNode->left == nullptr) { throw HuffmanUnArchiveBrokenDataError(""); }
                else currentNode = currentNode->left.get();
            } else {
                if (currentNode->right == nullptr) { throw HuffmanUnArchiveBrokenDataError(""); }
                else currentNode = currentNode->right.get();
            }

            if (currentNode->isLeaf) {
                writer.writeUInt8(currentNode->byte);
                writer.flush();
                currentNode = root.get();
            }

            bitPos++;
        }

        writer.flush();
    } catch (BitReaderIOError) {
        throw HuffmanUnArchiveIStreamError("unarchive(): Read istream data error.");
    } catch (BitReaderEOFError) {
        throw HuffmanUnArchiveIStreamError("unarchive(): Read istream unexpected EOF.");
    } catch (BitWriterIOError) {
        throw HuffmanUnArchiveOStreamError("unarchive(): Write out stream error.");
    } catch (HuffmanUnArchiveBrokenDataError) {
        throw HuffmanUnArchiveBrokenDataError("unarchive(): Unexpected istream data byte code.");
    }
}
