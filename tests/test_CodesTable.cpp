#include "Bits.hpp"
#include "CodesTable.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <list>

TEST(CodesTableEmpty, DefaultTable)
{
    CodesTable ct;
    for (int i = 0; i < 256; ++i) {
        EXPECT_EQ(ct.getCodeLength(static_cast<uint8_t>(i)), 0u);
    }
}

TEST(CodesTableEmpty, SerializeEmpty)
{
    CodesTable ct;
    Bits bits = ct.toBits();
    EXPECT_EQ(bits.size(), 8u);
    for (size_t i = 0; i < 8; ++i)
        EXPECT_FALSE(bits.bitAt(i));
}

TEST(CodesTableAddBits, BuildAndCheck)
{
    CodesTable ct;
    std::list<uint8_t> syms = { 'A', 'B' };
    ct.addBits(syms, true);
    ct.addBits({ 'A' }, false);
    EXPECT_EQ(ct.getCodeLength('A'), 2u);
    EXPECT_EQ(ct.getCodeLength('B'), 1u);
    const Bits& codeA = ct.getCode('A');
    EXPECT_TRUE(codeA.bitAt(0));
    EXPECT_FALSE(codeA.bitAt(1));
    const Bits& codeB = ct.getCode('B');
    EXPECT_TRUE(codeB.bitAt(0));
}

TEST(CodesTableRoundTrip, ToBitsAndBack)
{
    CodesTable ct1;
    ct1.addBits({ 'X' }, true);
    ct1.addBits({ 'X' }, false);
    ct1.addBits({ 'X' }, true);
    Bits serialized = ct1.toBits();
    CodesTable ct2(serialized);
    EXPECT_EQ(ct2.getCodeLength('X'), 3u);
    const Bits& code = ct2.getCode('X');
    EXPECT_TRUE(code.bitAt(0));
    EXPECT_FALSE(code.bitAt(1));
    EXPECT_TRUE(code.bitAt(2));
}

TEST(CodesTableDeserializeError, IncompleteData)
{
    Bits bits;
    for (int i = 0; i < 8; ++i)
        bits.addBit(i == 7);
    EXPECT_THROW({ CodesTable ct(bits); }, CodesTableIncorrectBitsData);
}

TEST(CodesTableReverseBits, ReversesAllCodes)
{
    CodesTable ct;
    ct.addBits({ 'P' }, true);
    ct.addBits({ 'P' }, false);
    ct.reverseAllBits();
    const Bits& code = ct.getCode('P');
    EXPECT_EQ(code.size(), 2u);
    EXPECT_FALSE(code.bitAt(0));
    EXPECT_TRUE(code.bitAt(1));
}
