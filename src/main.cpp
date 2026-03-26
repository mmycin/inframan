#include "CLI11.hpp"
#include "app-config.hpp"
#include "commands/version/version.hpp"
#include "commands/create-group/create-group.hpp"
#include "commands/list-group/list-group.hpp"
#include "commands/delete-group/delete-group.hpp"
#include "commands/add-job/add-job.hpp"
#include "commands/update-job/update-job.hpp"
#include "commands/read-job/read-job.hpp"
#include "commands/delete-job/delete-job.hpp"

// Helper: register a subcommand with an alias and callback
template <typename Cmd>
void registerCmd(CLI::App& app, const std::string& desc) {
    auto sub = app.add_subcommand(Cmd::flag, desc);
    if (Cmd::flags.size() > 1)
        sub->alias(Cmd::flags[1]);
    sub->callback([]() { Cmd::execute(); });
}

int main(int argc, char** argv) {
    CLI::App app{AppConfig::description, AppConfig::name};

    // Version flag
    app.add_flag_function(commands::Version::flag, [](int) {
        commands::Version::execute();
        exit(0);
    }, "Print version information");

    // Group commands
    registerCmd<commands::CreateGroup>(app, "Create a new infrastructure group");
    registerCmd<commands::ListGroup>  (app, "List all groups             [alias: lg]");
    registerCmd<commands::DeleteGroup>(app, "Delete a group              [alias: dg]");

    // Job commands
    registerCmd<commands::AddJob>    (app, "Add a job to a group        [alias: aj]");
    registerCmd<commands::UpdateJob> (app, "Update a job in a group     [alias: uj]");
    registerCmd<commands::ReadJob>   (app, "Read jobs of a group        [alias: rj]");
    registerCmd<commands::DeleteJob> (app, "Delete a job from a group   [alias: dj]");

    CLI11_PARSE(app, argc, argv);
    return 0;
}