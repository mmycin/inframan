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
#include "commands/use-group/use-group.hpp"
#include "commands/update-group/update-group.hpp"
#include "commands/execute-job/execute-job.hpp"
#include "commands/run-group/run-group.hpp"
#include "commands/stop-group/stop-group.hpp"
#include "commands/status/status.hpp"
#include "libraries/rang.hpp"
#include "banner.hpp"
#include "path-completer.hpp"
#include <locale>

// Helper: register a subcommand with an alias and callback
template <typename Cmd>
void registerCmd(CLI::App& app, const std::string& desc) {
    auto sub = app.add_subcommand(Cmd::flag, desc);
    if (Cmd::flags.size() > 1)
        sub->alias(Cmd::flags[1]);
    sub->callback([]() { Cmd::execute(); });
}

int main(int argc, char** argv) {
    // Set locale to fix locale issues
    try {
        std::locale::global(std::locale(""));
        std::cout.imbue(std::locale(""));
        std::cerr.imbue(std::locale(""));
    } catch (...) {
        // Fallback to "C" locale
        try {
            std::locale::global(std::locale("C"));
            std::cout.imbue(std::locale("C"));
            std::cerr.imbue(std::locale("C"));
        } catch (...) {}
    }

    // Print ASCII banner
    AsciiBanner::print();

    // Setup path completion for linenoise
    PathCompleter::setup();

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
    registerCmd<commands::UseGroup>   (app, "Set active group context    [alias: use]");
    registerCmd<commands::UpdateGroup>(app, "Update group metadata       [alias: ug]");

    // Job commands
    registerCmd<commands::AddJob>    (app, "Add a job to a group        [alias: aj]");
    registerCmd<commands::UpdateJob> (app, "Update a job in a group     [alias: uj]");
    registerCmd<commands::ReadJob>   (app, "Read jobs of a group        [alias: rj]");
    registerCmd<commands::DeleteJob> (app, "Delete a job from a group   [alias: dj]");
    registerCmd<commands::ExecuteJob>(app, "Execute a single job        [alias: execute]");
    registerCmd<commands::RunGroup>  (app, "Run all jobs in a group    [alias: rg]");
    registerCmd<commands::StopGroup> (app, "Stop all jobs in a group   [alias: sg]");
    registerCmd<commands::Status>    (app, "Show status of all jobs    [alias: st]");

    CLI11_PARSE(app, argc, argv);
    return 0;
}