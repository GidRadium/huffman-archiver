#include "BitWriter.hpp"
#include "Bits.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>

TEST(BitWriterTest, WriteBitFlush)
{
    std::ostringstream out;
    BitWriter writer(out);
    writer.writeBit(true);
    writer.flush();
    EXPECT_EQ(out.str(), std::string(1, static_cast<char>(0b10000000)));
}

TEST(BitWriterTest, WriteUInt8)
{
    std::ostringstream out;
    BitWriter writer(out);
    writer.writeUInt8(0b10101011);
    writer.flush();
    EXPECT_EQ(out.str(), std::string(1, static_cast<char>(0b10101011)));
}

TEST(BitWriterTest, WriteUInt16)
{
    std::ostringstream out;
    BitWriter writer(out);
    writer.writeUInt16(0x1234);
    writer.flush();
    std::string expected = std::string { static_cast<char>(0b00010010),
        static_cast<char>(0b00110100) };
    EXPECT_EQ(out.str(), expected);
}

TEST(BitWriterTest, WriteBeyondBuffer)
{
    std::ostringstream out;
    BitWriter writer(out, 1);
    writer.writeUInt8(0b10101010);
    writer.writeUInt8(0b10111011);
    writer.flush();
    std::string expected = std::string { static_cast<char>(0b10101010),
        static_cast<char>(0b10111011) };
    EXPECT_EQ(out.str(), expected);
}

TEST(BitWriterTest, MixedBitsAndRaw)
{
    std::ostringstream out;
    BitWriter writer(out);
    writer.writeBit(true);
    writer.writeAllData({ 0b00110100 });
    writer.flush();
    std::string expected = std::string { static_cast<char>(0b10000000),
        static_cast<char>(0b00110100) };
    EXPECT_EQ(out.str(), expected);
}

TEST(BitWriterTest, WriteBits)
{
    std::ostringstream out;
    BitWriter writer(out);
    Bits bits;
    bits.addBit(true);
    bits.addBit(false);
    bits.addBit(true);
    bits.addBit(false);
    writer.writeBits(bits);
    writer.flush();
    EXPECT_EQ(out.str(), std::string(1, static_cast<char>(0b10100000)));
}

TEST(BitWriterTest, WriteZeroBits)
{
    std::ostringstream out;
    BitWriter writer(out);
    Bits empty;
    writer.writeBits(empty);
    writer.flush();
    EXPECT_TRUE(out.str().empty());
}
