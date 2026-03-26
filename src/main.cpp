#include "CLI11.hpp"
#include "app-config.hpp"
#include "commands/version.hpp"

using namespace std;
using namespace CLI;

int main(int argc, char** argv) {
    App app{AppConfig::name, AppConfig::description};
    argv = app.ensure_utf8(argv);
    
    app.add_flag_function(commands::Version::flag, [&](int count) {
        commands::Version::execute();
        exit(0);
    }, "Print version information");

    CLI11_PARSE(app, argc, argv); 
    return 0;
}