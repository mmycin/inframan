#include "group-config.hpp"
#include "update-job.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"

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
        std::cerr << "DEBUG: Starting UpdateJob::run()\n";
        
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
        std::cout << "\n" << header << "\n";

        // Use active group context if available
        std::cerr << "DEBUG: Getting active group\n";
        group_name = ContextManager::getActiveGroup();
        std::cerr << "DEBUG: Got active group: '" << group_name << "'\n";
        
        if (group_name.empty()) {
            std::cerr << "DEBUG: Group name is empty, calling selectGroup()\n";
            selectGroup();
        } else {
            std::cout << "Using active group: " << group_name << "\n";
        }

        std::cerr << "DEBUG: About to call selectJob()\n";
        selectJob();
        std::cerr << "DEBUG: selectJob() completed successfully\n";
        
        promptUpdates();

        GroupRegistry registry(GroupConfig::registry_file);
        registry.addOrUpdateJob(group_name, job_name, file_path, command);

        tabulate::Table success;
        success.add_row({"SUCCESS", "Job '" + job_name + "' updated."});
        success[0][0].format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold});
        std::cout << "\n" << success << "\n";

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
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

    std::cout << "\nSelect group (number): ";
    size_t choice;
    std::cin >> choice;

    if (choice < 1 || choice > names.size())
        throw std::runtime_error("Invalid selection.");

    group_name = names[choice - 1];
}

void UpdateJob::selectJob() {
    std::cerr << "DEBUG: Entering selectJob() with group_name: '" << group_name << "'\n";
    
    GroupRegistry registry(GroupConfig::registry_file);
    std::cerr << "DEBUG: Registry created\n";
    
    auto jobs = registry.listJobNames(group_name);
    std::cerr << "DEBUG: Got job names, count: " << jobs.size() << "\n";

    if (jobs.empty())
        throw std::runtime_error("No jobs found in group '" + group_name + "'.");

    std::cout << "\nJobs in group '" << group_name << "':\n";
    for (size_t i = 0; i < jobs.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << jobs[i] << "\n";
    }

    std::cout << "\nSelect job to update (number): ";
    size_t choice;
    std::cin >> choice;

    if (choice < 1 || choice > jobs.size())
        throw std::runtime_error("Invalid selection.");

    job_name = jobs[choice - 1];
    std::cerr << "DEBUG: Selected job: '" << job_name << "'\n";
}

void UpdateJob::promptUpdates() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto jobs = registry.getJobs(group_name);
    
    if (!jobs.contains(job_name)) {
        throw std::runtime_error("Job '" + job_name + "' not found in group '" + group_name + "'.");
    }
    
    auto job_data = jobs[job_name];

    std::cout << "\nUpdating job '" << job_name << "'. Leave empty to keep current value.\n";

    std::string current_file = job_data.value("file_path", "unknown");
    std::string current_cmd = job_data.value("command", "unknown");

    std::cout << "File path [" << current_file << "]: ";
    std::string input_file;
    std::cin.ignore();
    std::getline(std::cin, input_file);
    file_path = input_file.empty() ? current_file : input_file;

    std::cout << "Command   [" << current_cmd << "]: ";
    std::string input_cmd;
    std::getline(std::cin, input_cmd);
    command = input_cmd.empty() ? current_cmd : input_cmd;
}

} // namespace commands
