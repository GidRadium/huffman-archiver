#include "app.hpp"

#include "archive.hpp"
#include "unarchive.hpp"

#include <string.h>

struct Options {
    bool compress = false;
    bool decompress = false;
    std::string input;
    std::string output;
    CompressMode mode = CompressMode::READ_TWICE;
    bool show_help = false;
    bool valid = true;
    std::string error;
};

Options parse_arguments(int argc, char* argv[]) {
    Options opts;

    return opts;
}

void print_usage(const char* prog_name) {
    std::cerr << "Usage\n\n"
        << prog_name << " <command> <input> <output> [options]\n"
        << "Commands:\n"
        << "  compress (c)      = Compress input to output\n"
        << "  decompress (d)    = Decompress input to output\n"
        << "Options:\n"
        << "  -m, --mode <mode> = Set compression mode (default: twice)\n"
        << "                      Modes:\n"
        << "                      ram (load full input file to RAM),\n"
        << "                      twice (read input file two times),disk\n"
        << "                      disk (use disk to temporary save input data)\n"
        << "  -h, -H, -help\n"
        << "  --help            = Print usage information and exit.\n";
}
