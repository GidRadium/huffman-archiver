#include <gtest/gtest.h>
#include "BitReader.hpp"
#include "Bits.hpp"
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>


TEST(BitReaderTest, ReadBitBasic) {
    std::istringstream in(std::string(1, static_cast<char>(0b10000000)));
    BitReader reader(in);
    EXPECT_TRUE(reader.readBit());
    EXPECT_FALSE(reader.eof());
}

TEST(BitReaderTest, ReadByte) {
    std::istringstream in(std::string(1, static_cast<char>(0b10101011)));
    BitReader reader(in);
    EXPECT_EQ(reader.readUInt8(), 0xAB);
}

TEST(BitReaderTest, ReadUInt16) {
    std::istringstream in(std::string{static_cast<char>(0b00010010),
                                      static_cast<char>(0b00110100)});
    BitReader reader(in);
    EXPECT_EQ(reader.readUInt16(), 0x1234);
}

TEST(BitReaderTest, ReadBeyondEOF) {
    std::istringstream in(std::string(1, static_cast<char>(0b11111111)));
    BitReader reader(in);
    EXPECT_THROW(reader.readUInt16(), BitReaderEOFError);
}

TEST(BitReaderTest, MixedReadAll) {
    std::istringstream in(std::string{static_cast<char>(0b00010010),
                                      static_cast<char>(0b00110100),
                                      static_cast<char>(0b01010110)});
    BitReader reader(in, 1);
    reader.readBits(4);
    auto rest = reader.readAllData();
    std::vector<uint8_t> expected = {0b00110100, 0b01010110};
    EXPECT_EQ(rest, expected);
}

TEST(BitReaderTest, ResetWorks) {
    std::istringstream in(std::string(1, static_cast<char>(0b10101011)));
    BitReader reader(in);
    uint8_t first = reader.readUInt8();
    reader.reset();
    uint8_t second = reader.readUInt8();
    EXPECT_EQ(first, second);
}

TEST(BitReaderTest, EofAfterRead) {
    std::istringstream in(std::string(1, static_cast<char>(0b00000000)));
    BitReader reader(in);
    reader.readUInt8();
    EXPECT_TRUE(reader.eof());
}

TEST(BitReaderTest, ReadZeroBits) {
    std::istringstream in(std::string(1, static_cast<char>(0b11111111)));
    BitReader reader(in);
    Bits bits = reader.readBits(0);
    EXPECT_EQ(bits.size(), 0u);
    EXPECT_FALSE(reader.eof());
}

TEST(BitReaderTest, ReadBeyondBufferSize) {
    std::istringstream in(std::string{static_cast<char>(0b10101010),
                                      static_cast<char>(0b10111011),
                                      static_cast<char>(0b11001100)});
    BitReader reader(in, 2);
    EXPECT_EQ(reader.readUInt8(), 0b10101010);
    EXPECT_EQ(reader.readUInt8(), 0b10111011);
    EXPECT_EQ(reader.readUInt8(), 0b11001100);
}
