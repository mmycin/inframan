#include "group-config.hpp"
#include "update-job.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"
#include "libraries/rang.hpp"
#include "libraries/linenoise.hpp"
#include "path-completer.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace commands {

void UpdateJob::execute() {
    UpdateJob{}.run();
}

void UpdateJob::run() {
    try {
        tabulate::Table header;
        header.add_row({"UPDATE INFRASTRUCTURE JOB"});
        header[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::yellow)
            .border_top("=")
            .border_bottom("=")
            .border_left("")
            .border_right("")
            .corner("");
        header.column(0).format().width(50);
        std::cout << "\n" << rang::fg::yellow << header << rang::fg::reset << "\n";

        // Use active group context if available
        group_name = ContextManager::getActiveGroup();
        
        if (group_name.empty()) {
            selectGroup();
        } else {
            std::cout << "Using active group: " << rang::fg::yellow << group_name << rang::fg::reset << "\n";
        }

        selectJob();
        promptUpdates();

        GroupRegistry registry(GroupConfig::registry_file);
        registry.addOrUpdateJob(group_name, job_name, file_path, command);

        tabulate::Table success;
        success.add_row({"SUCCESS", "Job '" + job_name + "' updated."});
        success[0][0].format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold});
        std::cout << "\n" << rang::fg::green << success << rang::fg::reset << "\n";

    } catch (const std::exception& e) {
        std::cerr << "\n" << rang::fg::red << "Error: " << e.what() << rang::fg::reset << "\n";
    }
}

void UpdateJob::selectGroup() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto names = registry.listGroupNames();

    if (names.empty())
        throw std::runtime_error("No infrastructure groups found.");

    std::cout << "\nAvailable Groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << names[i] << "\n";

    std::cout << "\nSelect group (number): " << rang::style::bold;
    size_t choice;
    std::cin >> choice;
    std::cout << rang::style::reset;

    if (choice < 1 || choice > names.size())
        throw std::runtime_error("Invalid selection.");

    group_name = names[choice - 1];
}

void UpdateJob::selectJob() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto jobs = registry.listJobNames(group_name);

    if (jobs.empty())
        throw std::runtime_error("No jobs found in group '" + group_name + "'.");

    std::cout << "\nJobs in group '" << group_name << "':\n";
    for (size_t i = 0; i < jobs.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << jobs[i] << "\n";
    }

    std::cout << "\nSelect job to update (number): " << rang::style::bold;
    size_t choice;
    std::cin >> choice;
    std::cout << rang::style::reset;

    if (choice < 1 || choice > jobs.size())
        throw std::runtime_error("Invalid selection.");

    job_name = jobs[choice - 1];
}

void UpdateJob::promptUpdates() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto jobs = registry.getJobs(group_name);
    
    if (!jobs.contains(job_name)) {
        throw std::runtime_error("Job '" + job_name + "' not found in group '" + group_name + "'.");
    }
    
    auto job_data = jobs[job_name];

    std::cout << "\nUpdating job '" << job_name << "'. " << rang::fg::gray << "Leave empty to keep current value." << rang::fg::reset << "\n";

    std::string current_file = job_data.value("file_path", "unknown");
    std::string current_cmd = job_data.value("command", "unknown");

    std::string prompt = "File path [" + current_file + "]: ";
    if (!linenoise::Readline(prompt.c_str(), file_path)) {
        throw std::runtime_error("Input cancelled");
    }
    if (file_path.empty()) file_path = current_file;

    std::cout << rang::style::bold << "Command   [" << current_cmd << "]: " << rang::style::reset;
    std::string input_cmd;
    // Note: We're not using linenoise for command for now, only for path as requested
    std::cin.ignore();
    std::getline(std::cin, input_cmd);
    command = input_cmd.empty() ? current_cmd : input_cmd;
}

} // namespace commands
