#pragma once

#include <istream>
#include <ostream>

enum class CompressMode {
    SAVE_TO_RAM,
    READ_TWICE,
    SAVE_TO_DISK
};

void archive(std::istream &in, std::ostream &out, CompressMode mode);
