#pragma once

#include <istream>
#include <ostream>

class HuffmanUnArchiveException : public std::runtime_error {
public:
    explicit HuffmanUnArchiveException(const std::string& msg) : std::runtime_error(msg) {}
};

class HuffmanUnArchiveIStreamError : public HuffmanUnArchiveException {
public:
    explicit HuffmanUnArchiveIStreamError(const std::string& msg) : HuffmanUnArchiveException(msg) {}
};

class HuffmanUnArchiveOStreamError : public HuffmanUnArchiveException {
public:
    explicit HuffmanUnArchiveOStreamError(const std::string& msg) : HuffmanUnArchiveException(msg) {}
};

class HuffmanUnArchiveBrokenDataError : public HuffmanUnArchiveException {
public:
    explicit HuffmanUnArchiveBrokenDataError(const std::string& msg) : HuffmanUnArchiveException(msg) {}
};

void unarchive(std::istream& in, std::ostream& out);
