#include "unarchive.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

TEST(UnarchiveEmptyStream, ThrowsBroken)
{
    std::istringstream in("");
    std::ostringstream out;
    EXPECT_THROW(unarchive(in, out), HuffmanUnArchiveBrokenDataError);
}

TEST(UnarchiveIncompleteTable, ThrowsBroken)
{
    std::string data { static_cast<char>(0b00000001) };
    std::istringstream in(data);
    std::ostringstream out;
    EXPECT_THROW(unarchive(in, out), HuffmanUnArchiveBrokenDataError);
}

TEST(UnarchiveEmptyArchive, ProducesEmptyOutput)
{
    std::string data { static_cast<char>(0b00000000) };
    std::istringstream in(data);
    std::ostringstream out;
    EXPECT_THROW(unarchive(in, out), HuffmanUnArchiveBrokenDataError);
}
