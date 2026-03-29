#include "group-config.hpp"
#include "add-job.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"
#include "libraries/rang.hpp"

#include <iostream>
#include <stdexcept>

namespace commands {

void AddJob::execute() {
    AddJob{}.run();
}

void AddJob::run() {
    try {
        tabulate::Table header;
        header.add_row({"ADD NEW JOB"});
        header[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::green)
            .border_top("=")
            .border_bottom("=")
            .border_left("")
            .border_right("")
            .corner("");
        header.column(0).format().width(50);
        std::cout << "\n" << header << "\n";

        std::string group_name = ContextManager::getActiveGroup();
        if (group_name.empty()) {
            std::cout << rang::style::bold << "Enter infrastructure group name: " << rang::style::reset;
            std::cin >> group_name;
        } else {
            std::cout << "Using active group: " << rang::fg::yellow << group_name << rang::fg::reset << "\n";
        }

        std::string job_name, file_path, command;
        std::cout << rang::style::bold << "Enter job name: " << rang::style::reset;
        std::cin >> job_name;
        
        // Get group info to determine if file_path and command are required
        GroupRegistry registry(GroupConfig::registry_file);
        auto groups = registry.getGroups();
        auto group_data = groups[group_name];
        std::string provider_str = group_data.value("provider", "docker");
        std::string type_str = group_data.value("type", "compose");
        
        auto provider = GroupConfig::providerFromString(provider_str);
        auto type = GroupConfig::typeFromString(type_str);
        
        // Only prompt for file_path if required by type
        if (GroupConfig::requiresFilePath(type)) {
            std::cout << rang::style::bold << "Enter file path [" << GroupConfig::autoDetectFilePath(type, job_name) << "]: " << rang::style::reset;
            std::cin.ignore();
            std::getline(std::cin, file_path);
            if (file_path.empty()) {
                file_path = GroupConfig::autoDetectFilePath(type, job_name);
            }
        } else {
            file_path = GroupConfig::autoDetectFilePath(type, job_name);
        }
        
        // Only prompt for command if it's a custom type
        if (GroupConfig::requiresCommand(type)) {
            std::cout << rang::style::bold << "Enter shell command: " << rang::style::reset;
            std::cin.ignore();
            std::getline(std::cin, command);
        } else {
            command = GroupConfig::autoDetectCommand(provider, type, job_name);
            std::cout << "Auto-detected command: " << rang::fg::cyan << command << rang::fg::reset << "\n";
        }

        registry.addOrUpdateJob(group_name, job_name, file_path, command, "down");

        tabulate::Table success;
        success.add_row({"SUCCESS", "Job added to group '" + group_name + "'"});
        success[0][0].format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold});
        std::cout << "\n" << rang::fg::green << success << rang::fg::reset << "\n";

    } catch (const std::exception& e) {
        std::cerr << "\n" << rang::fg::red << "Error: " << e.what() << rang::fg::reset << "\n";
    }
}

} // namespace commands
