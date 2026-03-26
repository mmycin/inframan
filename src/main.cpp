#include "CLI11.hpp"

using namespace std;

int main(int argc, char** argv) {
    CLI::App app{"InfraManager"};
    argv = app.ensure_utf8(argv);
    
    string filename = "default";
    app.add_option("-f,--file", filename, "Input file");

    CLI11_PARSE(app, argc, argv);
    return 0;
}