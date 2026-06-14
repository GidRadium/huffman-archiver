# huffman-archiver

[![Lint and test](https://github.com/GidRadium/huffman-archiver/actions/workflows/build-lint-test.yml/badge.svg)](https://github.com/GidRadium/huffman-archiver/actions/workflows/build-lint-test.yml)
[![Sanitize test](https://github.com/GidRadium/huffman-archiver/actions/workflows/build-sanitize-test.yml/badge.svg)](https://github.com/GidRadium/huffman-archiver/actions/workflows/build-sanitize-test.yml)

Console archiver based on the Huffman algorithm. Works with files of any size. Can compress either in RAM or after two passes of the file.

## Install
`git clone https://github.com/GidRadium/huffman-archiver.git`

## Build
`cd huffman-archiver`

`cmake -B build-release -DCMAKE_BUILD_TYPE=Release`

`cmake --build build-release -j$(nproc)`

## Usage
Help:

`./build/harch-cli -h`

Compress:

`./build/harch-cli c data.txt data.txt.harch`

Decompress:

`./build/harch-cli d data.txt.harch data_reharch.txt`

## Test
Run:

`cd build && ctest && cd ..`

## Lint

`find . -path './build' -prune -o -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' -o -name '*.c' \) -print | xargs clang-format -i`

## Sanitize

`cmake -B build-san -S . -DENABLE_SANITIZERS=ON`

`cmake --build build-san`

`ctest --test-dir build-san`
