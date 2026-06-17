cmake_minimum_required(VERSION 3.14)

project(HuffmanArchiver VERSION 1.0 LANGUAGES CXX)

option(ENABLE_SANITIZERS "Enable sanitizers" OFF)

# Windows
if(MSVC OR
   (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND
    CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"))

  if(ENABLE_SANITIZERS)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
  else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()

endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sanitizers
if(ENABLE_SANITIZERS)

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    add_compile_options(
      -fsanitize=address
      -fno-omit-frame-pointer
    )

    add_link_options(
      -fsanitize=address
    )

  elseif(MSVC)

    add_compile_options(/fsanitize=address)
    add_link_options(/fsanitize=address)

  endif()

endif()

# Optimisations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported)

  if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native -funroll-loops -finline-functions -fomit-frame-pointer")

    if(NOT ipo_supported)
      set(CMAKE_CXX_FLAGS_RELEASE
          "${CMAKE_CXX_FLAGS_RELEASE} -flto=auto")
    endif()

    set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

  elseif(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ot /GL /Gy /Oi /DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE
        "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
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

target_include_directories(huffman PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(huffman PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/huffman
)

# App
add_executable(harch-cli
    app/main.cpp
    app/app.cpp
)

target_link_libraries(harch-cli PRIVATE huffman)

# Tests
option(BUILD_TESTS "Build unit tests (requires GoogleTest)" OFF)

if(BUILD_TESTS)
  include(FetchContent)

  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )

  FetchContent_MakeAvailable(googletest)

  enable_testing()
  add_subdirectory(tests)
endif()
