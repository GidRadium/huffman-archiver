/**
 * @file app/app.hpp
 * @brief Console application entry point declaration.
 */

#pragma once

/**
 * @brief Runs the command-line archive/unarchive utility.
 *
 * Parses arguments and executes compression or decompression.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code (0 on success, non-zero on error).
 */
int runConsoleApp(int argc, char* argv[]);
