#include "CLI11.hpp"
#include "app-config.hpp"
#include "tfile.h"

using namespace std;
using namespace CLI;

int main(int argc, char** argv) {
    App app{AppConfig::name, AppConfig::description};
    argv = app.ensure_utf8(argv);

    CLI11_PARSE(app, argc, argv); 
    return 0;
}