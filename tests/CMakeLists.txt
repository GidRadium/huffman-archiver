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
target_include_directories(huffman_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

include(GoogleTest)
gtest_add_tests(TARGET huffman_tests)
