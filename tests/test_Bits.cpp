#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>

#include "Bits.hpp"

TEST(BitsTest, Empty)
{
    Bits b;
    EXPECT_EQ(b.size(), 0u);
    EXPECT_THROW(static_cast<void>(b.bitAt(0)), std::out_of_range);
    EXPECT_THROW(static_cast<void>(b.byteAt(0)), std::out_of_range);

    Bits s = b.slice(0, 0);
    EXPECT_EQ(s.size(), 0u);

    EXPECT_THROW(static_cast<void>(b.toByte(0)), std::out_of_range);

    b.reverse();
    EXPECT_EQ(b.size(), 0u);
    std::ostringstream oss;
    oss << b;
    EXPECT_EQ(oss.str(), "");
}

TEST(BitsTest, SingleBit)
{
    Bits b;
    b.addBit(true);
    EXPECT_EQ(b.size(), 1u);
    EXPECT_TRUE(b.bitAt(0));
    b.addBit(false);
    EXPECT_EQ(b.size(), 2u);
    EXPECT_FALSE(b.bitAt(1));
}

TEST(BitsTest, FromByte)
{
    uint8_t byte = 0b10101010;
    Bits b(byte);
    EXPECT_EQ(b.size(), 8u);

    EXPECT_TRUE(b.bitAt(0));
    EXPECT_FALSE(b.bitAt(1));
    EXPECT_TRUE(b.bitAt(2));
    EXPECT_FALSE(b.bitAt(3));
    EXPECT_TRUE(b.bitAt(4));
    EXPECT_FALSE(b.bitAt(5));
    EXPECT_TRUE(b.bitAt(6));
    EXPECT_FALSE(b.bitAt(7));

    EXPECT_EQ(b.toByte(0), byte);
}

TEST(BitsTest, FromBytes)
{
    uint8_t data[] = { 0b11111111, 0b00000000 };
    Bits b(data, 2);
    EXPECT_EQ(b.size(), 16u);
    for (size_t i = 0; i < 8; ++i)
        EXPECT_TRUE(b.bitAt(i));
    for (size_t i = 8; i < 16; ++i)
        EXPECT_FALSE(b.bitAt(i));
    EXPECT_EQ(b.byteAt(0), 0b11111111);
    EXPECT_EQ(b.byteAt(1), 0b00000000);
}

TEST(BitsTest, AddBitsFullByte)
{
    Bits b;
    b.addBit(true);
    b.addBit(false);
    b.addBit(true);
    b.addBit(false);
    b.addBit(true);
    b.addBit(false);
    b.addBit(true);
    b.addBit(false);
    EXPECT_EQ(b.size(), 8u);
    EXPECT_EQ(b.byteAt(0), 0b10101010);
    EXPECT_TRUE(b.bitAt(0));
    EXPECT_FALSE(b.bitAt(7));
}

TEST(BitsTest, BitAtOutOfRange)
{
    Bits b;
    b.addBit(true);
    EXPECT_THROW(static_cast<void>(b.bitAt(1)), std::out_of_range);
    EXPECT_THROW(static_cast<void>(b.bitAt(100)), std::out_of_range);
}

TEST(BitsTest, ByteAtOutOfRange)
{
    uint8_t data[] = { 0b10101010, 0b10111011 };
    Bits b(data, 2);
    EXPECT_NO_THROW(static_cast<void>(b.byteAt(0)));
    EXPECT_NO_THROW(static_cast<void>(b.byteAt(1)));
    EXPECT_THROW(static_cast<void>(b.byteAt(2)), std::out_of_range);
}

TEST(BitsTest, Append)
{
    Bits a;
    a.addBit(true);
    a.addBit(false);

    Bits b;
    b.addBit(true);
    b.addBit(true);
    b.addBit(false);

    a.append(b);
    EXPECT_EQ(a.size(), 5u);
    EXPECT_TRUE(a.bitAt(0));
    EXPECT_FALSE(a.bitAt(1));
    EXPECT_TRUE(a.bitAt(2));
    EXPECT_TRUE(a.bitAt(3));
    EXPECT_FALSE(a.bitAt(4));
}

TEST(BitsTest, Slice)
{
    uint8_t data[] = { 0b11001100, 0b11110000 };
    Bits b(data, 2);

    Bits firstByte = b.slice(0, 8);
    EXPECT_EQ(firstByte.size(), 8u);
    EXPECT_EQ(firstByte.toByte(0), 0b11001100);

    Bits mid = b.slice(4, 8);
    EXPECT_EQ(mid.size(), 8u);
    EXPECT_EQ(mid.toByte(0), 0b11001111);
}

TEST(BitsTest, Reverse)
{
    Bits b;
    b.addBit(true);
    b.addBit(false);
    b.addBit(true);
    EXPECT_EQ(b.size(), 3u);
    b.reverse();
    EXPECT_TRUE(b.bitAt(0));
    EXPECT_FALSE(b.bitAt(1));
    EXPECT_TRUE(b.bitAt(2));
}

TEST(BitsTest, ToByte)
{
    uint8_t data[] = { 0b00001111, 0b10101010 };
    Bits b(data, 2);
    EXPECT_EQ(b.toByte(0), 0b00001111);
    EXPECT_EQ(b.toByte(2), 0b00111110);
}

TEST(BitsTest, ToByteOutOfRange)
{
    Bits b(uint8_t { 0b11111111 });
    EXPECT_NO_THROW(static_cast<void>(b.toByte(0)));
    EXPECT_THROW(static_cast<void>(b.toByte(1)), std::out_of_range);
    b.addBit(false);
    EXPECT_NO_THROW(static_cast<void>(b.toByte(1)));
    EXPECT_THROW(static_cast<void>(b.toByte(2)), std::out_of_range);
}

TEST(BitsTest, Output)
{
    Bits b;
    b.addBit(true);
    b.addBit(false);
    b.addBit(true);
    b.addBit(true);
    std::ostringstream oss;
    oss << b;
    EXPECT_EQ(oss.str(), "1011");
}
