#include <cstdio>
#include <fstream>
#include <iterator>
#include <sstream>
#include <iostream>
#include "archive.hpp"


int main() {
    std::ifstream inputFile("data.txt");
    if (!inputFile) {
        std::cerr << "Не удалось открыть data.txt\n";
        return 1;
    }

    std::cout << "0" << std::endl;

    // Пример 1: SAVE_TO_RAM
    std::ostringstream ramStream;
    archive(inputFile, ramStream, CompressMode::SAVE_TO_RAM);
    std::cout << "Сжато в RAM: " << ramStream.str().size() << " байт\n";

    return 0;
}
