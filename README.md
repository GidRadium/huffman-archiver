# huffman-archiver

## Build
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
