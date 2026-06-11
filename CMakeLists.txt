cmake_minimum_required(VERSION 3.10)
project(HuffmanArchiver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For .clangd

add_library(huffman STATIC
    src/Bits.cpp
    src/BitReader.cpp
    src/BitWriter.cpp
    src/CodesTable.cpp
    src/CountTable.cpp
    src/archive.cpp
    src/unarchive.cpp
)

target_include_directories(huffman PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(huffman PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/huffman)

add_executable(harch-cli
    app/main.cpp
    app/app.cpp
)

target_link_libraries(harch-cli PRIVATE huffman)
