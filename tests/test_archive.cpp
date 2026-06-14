#include <gtest/gtest.h>
#include "archive.hpp"
#include <sstream>
#include <string>

TEST(ArchiveEmpty, NoThrow) {
    std::istringstream in("");
    std::ostringstream out;
    EXPECT_NO_THROW(archive(in, out, CompressMode::SAVE_TO_RAM));
    EXPECT_GT(out.str().size(), 0u);
}

TEST(ArchiveSingleByte, OutputNotEmpty) {
    std::istringstream in(std::string(1, static_cast<char>('A')));
    std::ostringstream out;
    ASSERT_NO_THROW(archive(in, out, CompressMode::SAVE_TO_RAM));
    EXPECT_GT(out.str().size(), 0u);
}

TEST(ArchiveRepeatByte, Compresses) {
    std::string data(256, 'X');
    std::istringstream in(data);
    std::ostringstream out;
    ASSERT_NO_THROW(archive(in, out, CompressMode::SAVE_TO_RAM));
    EXPECT_LT(out.str().size(), data.size());
}
