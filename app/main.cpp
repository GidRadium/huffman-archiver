/**
 * @file app/main.cpp
 * @brief Program entry point.
 */

#include "app.hpp"

/**
 * @brief Main function.
 *
 * Delegates to runConsoleApp().
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code.
 */
int main(int argc, char** argv)
{
    return runConsoleApp(argc, argv);
}
