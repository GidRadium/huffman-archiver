#include <gtest/gtest.h>
#include "archive.hpp"
#include "unarchive.hpp"
#include <sstream>
#include <string>

class ArchiveUnarchiveRoundTrip : public testing::TestWithParam<std::string> {};

TEST_P(ArchiveUnarchiveRoundTrip, ProducesOriginalData) {
    std::string input = GetParam();
    std::istringstream in(input);
    std::ostringstream compressed;
    ASSERT_NO_THROW(archive(in, compressed, CompressMode::SAVE_TO_RAM));

    std::string compressedData = compressed.str();
    std::istringstream compIn(compressedData);
    std::ostringstream decompressed;

    ASSERT_NO_THROW(unarchive(compIn, decompressed));

    EXPECT_EQ(decompressed.str(), input);
}

INSTANTIATE_TEST_SUITE_P(
    DataSets,
    ArchiveUnarchiveRoundTrip,
    testing::Values(
        "",
        std::string(1, static_cast<char>('A')),
        std::string(1, static_cast<char>(0x00)),
        std::string(10, 'B'),
        std::string(256, 'C'),
        std::string{static_cast<char>(0b01010101),
                    static_cast<char>(0b10101010),
                    static_cast<char>(0b11110000)}
    )
);
