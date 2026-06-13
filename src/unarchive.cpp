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
        const Bits& code = codesTable.data[byte];
        if (code.size() == 0) continue;
        std::cout << (char)byte << " : " << codesTable.data[byte] << std::endl;

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

    uint32_t codesTableBitsCount = reader.readUInt32();
    Bits codesTableAsBits = reader.readBits(codesTableBitsCount);
    CodesTable codesTable(codesTableAsBits);
    std::unique_ptr<Node> root = buildTree(codesTable);

    Node* currentNode = root.get();
    uint64_t inDataSizeBits = reader.readUInt64();
    uint64_t bitPos = 0;
    while (bitPos < inDataSizeBits) {
        bool currentBit = reader.readBit();
        if (!currentBit) {
            if (currentNode->left == nullptr) { /* Error */ }
            else currentNode = currentNode->left.get();
        } else {
            if (currentNode->right == nullptr) { /* Error */ }
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
}
