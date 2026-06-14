add_executable(huffman_tests
    test_Bits.cpp
    test_BitReader.cpp
    test_BitWriter.cpp
    test_CodesTable.cpp
    test_archive.cpp
    test_unarchive.cpp
    test_integration.cpp
)

target_link_libraries(huffman_tests PRIVATE huffman gtest_main gmock_main)

include(GoogleTest)
gtest_discover_tests(huffman_tests)
