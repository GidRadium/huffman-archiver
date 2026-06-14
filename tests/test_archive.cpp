#include "archive.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

TEST(ArchiveEmpty, NoThrow)
{
    std::istringstream in("");
    std::ostringstream out;
    EXPECT_NO_THROW(archive(in, out, CompressMode::SaveToRam));
    EXPECT_GT(out.str().size(), 0u);
}

TEST(ArchiveSingleByte, OutputNotEmpty)
{
    std::istringstream in(std::string(1, static_cast<char>('A')));
    std::ostringstream out;
    ASSERT_NO_THROW(archive(in, out, CompressMode::SaveToRam));
    EXPECT_GT(out.str().size(), 0u);
}

TEST(ArchiveRepeatByte, Compresses)
{
    std::string data(256, 'X');
    std::istringstream in(data);
    std::ostringstream out;
    ASSERT_NO_THROW(archive(in, out, CompressMode::SaveToRam));
    EXPECT_LT(out.str().size(), data.size());
}
