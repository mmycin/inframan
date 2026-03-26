#include "CLI11.hpp"
#include "app-config.hpp"
#include "commands/version.hpp"
#include "commands/create-group.hpp"

int main(int argc, char** argv) {
    // Fix: CLI::App expects (description, name) order
    CLI::App app{AppConfig::description, AppConfig::name};
    
    // Register commands
    app.add_flag_function(commands::Version::flag, [](int count) {
        commands::Version::execute();
        exit(0);
    }, "Print version information");
    
    // Register create-group command
    auto create_group_cmd = app.add_subcommand(commands::CreateGroup::flag, "Create a new group");
        create_group_cmd->alias("cg");  // Add "cg" as an alias
        create_group_cmd->callback([]() {
            commands::CreateGroup::execute();
    });
    
    CLI11_PARSE(app, argc, argv);
    
    return 0;
}