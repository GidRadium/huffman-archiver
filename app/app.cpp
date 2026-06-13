#include "app.hpp"

#include "archive.hpp"
#include "unarchive.hpp"

#include <string.h>
#include <iostream>
#include <fstream>

struct Options
{
    bool compress = false;
    bool decompress = false;
    std::string input;
    std::string output;
    CompressMode mode = CompressMode::READ_TWICE;
    bool show_help = false;
    bool valid = true;
    std::string error;
};

// <exec> <command> <input> <output> [options]
Options parseArguments(int argc, char* argv[])
{
    Options options;

    if (argc < 2) {
        options.valid = false;
        options.error = "No command specified.";

        return options;
    }

    std::string command = argv[1];

    if (command == "-h" || command == "-H" || command == "-help" || command == "--help") {
        options.show_help = true;

        return options;
    }

    if (command == "compress" || command == "c") {
        options.compress = true;
    } else if (command == "decompress" || command == "d") {
        options.decompress = true;
    } else {
        options.valid = false;
        options.error = "Unknown command: " + command + ". Expected 'compress' ('c') or 'decompress' ('d').";

        return options;
    }

    if (argc < 3) {
        options.valid = false;
        options.error = "Missing input file.";

        return options;
    }

    options.input = argv[2];

    if (argc < 4) {
        options.valid = false;
        options.error = "Missing output file.";

        return options;
    }

    options.output = argv[3];

    for (size_t i = 4; i < argc; ++i) {
        std::string option = argv[i];
        if (option == "-m" || option == "--mode") {
            if (i + 1 >= argc) {
                options.valid = false;
                options.error = "Missing mode value after " + option + ".";

                return options;
            }

            std::string mode = argv[i + 1];
            ++i;
            if (mode == "ram" || mode == "save_to_ram") {
                options.mode = CompressMode::SAVE_TO_RAM;
            } else if (mode == "twice" || mode == "read_twice") {
                options.mode = CompressMode::READ_TWICE;
            } else if (mode == "disk" || mode == "save_to_disk") {
                options.mode = CompressMode::SAVE_TO_DISK;
            } else {
                options.valid = false;
                options.error = "Unknown mode: " + mode + ". Available: ram, twice, disk.";

                return options;
            }
        } else if (option == "-h" || option == "--help") {
            options.show_help = true;

            return options;
        } else {
            options.valid = false;
            options.error = "Unknown option: " + option;

            return options;
        }
    }

    return options;
}

void printUsage(const char* executableName)
{
    std::cerr << "Usage\n\n"
        << "  " << executableName << " <command> <input> <output> [options]\n\n"
        << "Commands\n\n"
        << "  compress (c)      = Compress input to output\n"
        << "  decompress (d)    = Decompress input to output\n\n"
        << "Options\n\n"
        << "  -m, --mode <mode> = Set compression mode (default: twice)\n"
        << "                      Modes:\n"
        << "                      ram (load full input file to RAM),\n"
        << "                      twice (read input file two times),\n"
        << "                      disk (use disk to temporary save input data).\n"
        << "  -h, -H, -help\n"
        << "  --help            = Print usage information and exit.\n";
}


int runConsoleApp(int argc, char* argv[])
{
    Options options = parseArguments(argc, argv);

    if (!options.valid) {
        std::cerr << "Error: " << options.error << '\n'
            << "Specify --help for usage.\n";

        return 1;
    }

    if (options.show_help) {
        printUsage(argv[0]);

        return 0;
    }

    if (int(options.compress) + int(options.decompress) != 1) {
        std::cerr << "Error: invalid command.\n";

        return -1;
    }

    std::ifstream inputFile(options.input, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error: can't open " << options.input << ".\n";

        return -1;
    }

    std::ofstream outputFile(options.output, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error: can't open " << options.output << ".\n";

        return -1;
    }

    try {
        if (options.compress) {
            archive(inputFile, outputFile, options.mode);
        } else {
            unarchive(inputFile, outputFile);
        }
    } catch (...) {
        std::cerr << "Error: unexpected error." << ".\n";
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
