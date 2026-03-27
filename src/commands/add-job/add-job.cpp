#include "group-config.hpp"
#include "add-job.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"

#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#undef AddJob
#endif

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
            std::cout << "Enter infrastructure group name: ";
            std::cin >> group_name;
        } else {
            std::cout << "Using active group: " << group_name << "\n";
        }

        std::string job_name, file_path, command;
        std::cout << "Enter job name: ";
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
            std::cout << "Enter file path [" << GroupConfig::autoDetectFilePath(type, job_name) << "]: ";
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
            std::cout << "Enter shell command: ";
            std::cin.ignore();
            std::getline(std::cin, command);
        } else {
            command = GroupConfig::autoDetectCommand(provider, type, job_name);
            std::cout << "Auto-detected command: " << command << "\n";
        }

        registry.addOrUpdateJob(group_name, job_name, file_path, command);

        tabulate::Table success;
        success.add_row({"SUCCESS", "Job added to group '" + group_name + "'"});
        success[0][0].format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold});
        std::cout << "\n" << success << "\n";

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
    }
}

} // namespace commands
