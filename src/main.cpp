#include "CLI11.hpp"

using namespace std;

int main(int argc, char** argv) {
    CLI::App app{"InfraManager - Run your infrastructure regionally."};
    argv = app.ensure_utf8(argv);

    CLI11_PARSE(app, argc, argv);
    return 0;
}