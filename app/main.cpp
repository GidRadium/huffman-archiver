#include <fstream>
#include <iostream>
#include "archive.hpp"
#include "unarchive.hpp"

int main() {
    // ---------- Сжатие data.txt -> data.txt.harch ----------
    std::ifstream inputFile("data.txt");
    if (!inputFile) {
        std::cerr << "Не удалось открыть data.txt\n";
        return 1;
    }

    std::ofstream compressedFile("data.txt.harch", std::ios::binary);
    if (!compressedFile) {
        std::cerr << "Не удалось создать data.txt.harch\n";
        return 1;
    }

    archive(inputFile, compressedFile, CompressMode::READ_TWICE);
    inputFile.close();
    compressedFile.close();

    std::cout << "Файл data.txt сжат и сохранён как data.txt.harch\n";

    // ---------- Разархивирование data.txt.harch -> data_reharch.txt ----------
    std::ifstream archiveFile("data.txt.harch", std::ios::binary);
    if (!archiveFile) {
        std::cerr << "Не удалось открыть data.txt.harch\n";
        return 1;
    }

    std::ofstream outputFile("data_reharch.txt");
    if (!outputFile) {
        std::cerr << "Не удалось создать data_reharch.txt\n";
        return 1;
    }

    unarchive(archiveFile, outputFile);
    archiveFile.close();
    outputFile.close();

    std::cout << "Архив разархивирован в data_reharch.txt\n";
    return 0;
}
