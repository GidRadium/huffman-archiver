cmake_minimum_required(VERSION 3.14)
project(HuffmanArchiver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For .clangd

# Sanitizers
option(ENABLE_SANITIZERS "Enable Address and Undefined Behavior sanitizers" OFF)

if(ENABLE_SANITIZERS)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
  else()
    message(WARNING "Sanitizers are not supported for compiler ${CMAKE_CXX_COMPILER_ID}")
  endif()
endif()

# Library
add_library(huffman STATIC
    src/Bits.cpp
    src/BitReader.cpp
    src/BitWriter.cpp
    src/CodesTable.cpp
    src/archive.cpp
    src/unarchive.cpp
)

target_include_directories(huffman PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(huffman PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/huffman)

# App
add_executable(harch-cli
    app/main.cpp
    app/app.cpp
)

target_link_libraries(harch-cli PRIVATE huffman)


# Tests
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_subdirectory(tests)
