# huffman-archiver

[![Tests and sanitizers](https://github.com/GidRadium/huffman-archiver/actions/workflows/build-lint-test-sanz-doc.yml/badge.svg)](https://github.com/GidRadium/huffman-archiver/actions/workflows/build-lint-test-sanz-doc.yml)

## Install
`git clone https://github.com/GidRadium/huffman-archiver.git`

## Build
`cd huffman-archiver`

`rm -rf build`

`cmake -B build -S .`

`cmake --build build`

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
