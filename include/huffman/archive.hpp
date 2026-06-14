#pragma once

#include <istream>
#include <ostream>

enum class CompressMode {
    SAVE_TO_RAM,
    READ_TWICE,
    // SAVE_TO_DISK
};


class HuffmanArchiveException : public std::runtime_error {
public:
    explicit HuffmanArchiveException(const std::string& msg) : std::runtime_error(msg) {}
};

class HuffmanArchiveIStreamError : public HuffmanArchiveException {
public:
    explicit HuffmanArchiveIStreamError(const std::string& msg) : HuffmanArchiveException(msg) {}
};

class HuffmanArchiveOStreamError : public HuffmanArchiveException {
public:
    explicit HuffmanArchiveOStreamError(const std::string& msg) : HuffmanArchiveException(msg) {}
};

void archive(std::istream &in, std::ostream &out, CompressMode mode);
