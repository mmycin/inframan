#include "group-config.hpp"
#include "execute-job.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <filesystem>


namespace commands {

void ExecuteJob::execute() {
    ExecuteJob{}.run();
}

void ExecuteJob::run() {
    try {
        tabulate::Table header;
        header.add_row({"EXECUTE INFRASTRUCTURE JOB"});
        header[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::magenta)
            .border_top("=")
            .border_bottom("=")
            .border_left("")
            .border_right("")
            .corner("");
        header.column(0).format().width(50);
        std::cout << "\n" << header << "\n";

        // Use active group context if available
        group_name = ContextManager::getActiveGroup();
        if (group_name.empty()) {
            selectGroup();
        } else {
            std::cout << "Using active group: " << group_name << "\n";
        }

        selectJob();
        runCommand();

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
    }
}

void ExecuteJob::selectGroup() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto names = registry.listGroupNames();

    if (names.empty())
        throw std::runtime_error("No infrastructure groups found.");

    std::cout << "\nAvailable Groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << names[i] << "\n";

    std::cout << "\nSelect group (number): ";
    size_t choice;
    std::cin >> choice;

    if (choice < 1 || choice > names.size())
        throw std::runtime_error("Invalid selection.");

    group_name = names[choice - 1];
}

void ExecuteJob::selectJob() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto jobs = registry.listJobNames(group_name);

    if (jobs.empty())
        throw std::runtime_error("No jobs found in group '" + group_name + "'.");

    std::cout << "\nJobs in group '" << group_name << "':\n";
    for (size_t i = 0; i < jobs.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << jobs[i] << "\n";
    }

    std::cout << "\nSelect job to execute (number): ";
    size_t choice;
    std::cin >> choice;

    if (choice < 1 || choice > jobs.size())
        throw std::runtime_error("Invalid selection.");

    job_name = jobs[choice - 1];
}

void ExecuteJob::runCommand() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto jobs = registry.getJobs(group_name);
    auto job_data = jobs.at(job_name);

    std::string command = job_data.value("command", "");
    std::string file_path = job_data.value("file_path", "");

    if (command.empty())
        throw std::runtime_error("Job '" + job_name + "' has no command defined.");

    tabulate::Table exec_info;
    exec_info.add_row({"EXECUTING INFRASTRUCTURE JOB", ""});
    exec_info.add_row({"Job", job_name});
    exec_info.add_row({"Group", group_name});
    exec_info.add_row({"Command", command});
    if (!file_path.empty())
        exec_info.add_row({"Path", file_path});
    
    exec_info[0].format()
        .font_style({tabulate::FontStyle::bold})
        .font_color(tabulate::Color::cyan);
    
    for (size_t i = 1; i < exec_info.size(); ++i) {
        exec_info[i][0].format().font_style({tabulate::FontStyle::bold});
    }

    std::cout << "\n" << exec_info << "\n";

    std::cout << "--- Shell Output Starts ---\n";

    std::filesystem::path original_dir;
    bool dir_changed = false;

    if (!file_path.empty()) {
        try {
            std::filesystem::path target_dir = std::filesystem::path(file_path).parent_path();
            if (!target_dir.empty() && std::filesystem::exists(target_dir)) {
                original_dir = std::filesystem::current_path();
                std::filesystem::current_path(target_dir);
                dir_changed = true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not change directory (" << e.what() << ")\n";
        }
    }

    int status = std::system(command.c_str());

    if (dir_changed) {
        try {
            std::filesystem::current_path(original_dir);
        } catch (...) {}
    }

    std::cout << "--- Shell Output Ends ---\n";

    if (status != 0) {
        std::cerr << "\nWarning: Command exited with status " << status << "\n";
    } else {
        tabulate::Table success;
        success.add_row({"SUCCESS", "Job '" + job_name + "' finished successfully."});
        success[0][0].format()
            .font_color(tabulate::Color::green)
            .font_style({tabulate::FontStyle::bold});
        std::cout << "\n" << success << "\n";
    }
}

} // namespace commands
