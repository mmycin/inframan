#include "group-config.hpp"
#include "read-job.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"
#include "rang.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace commands {

void ReadJob::execute() {
    ReadJob{}.run();
}

void ReadJob::run() {
    try {
        tabulate::Table header;
        header.add_row({"READ GROUP JOBS"});
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
        group_name = ContextManager::getActiveGroup();
        if (group_name.empty()) {
            selectGroup();
        } else {
            std::cout << "Using active group: " << group_name << "\n";
        }

        GroupRegistry registry(GroupConfig::registry_file);
        auto jobs = registry.getJobs(group_name);

        if (jobs.empty()) {
            std::cout << "\nNo jobs found in group '" << group_name << "'.\n";
            return;
        }

        std::cout << "\nJobs in '" << group_name << "':\n";
        
        tabulate::Table table;
        table.add_row({"Job Name", "File Path", "Command"});

        for (auto it = jobs.begin(); it != jobs.end(); ++it) {
            std::string name = it.key();
            auto data = it.value();
            table.add_row({
                name,
                data.value("file_path", "unknown"),
                data.value("command", "unknown")
            });
        }

        // Apply styling
        table[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::yellow);

        table.column(0).format().font_align(tabulate::FontAlign::center);

        std::cout << table << "\n";

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
    }
}

void ReadJob::selectGroup() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto names = registry.listGroupNames();

    if (names.empty())
        throw std::runtime_error("No groups found.");

    std::cout << "\nAvailable groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << names[i] << "\n";

    std::cout << "Select group to read (number): ";
    size_t choice;
    std::cin >> choice;

    if (choice < 1 || choice > names.size())
        throw std::runtime_error("Invalid selection.");

    group_name = names[choice - 1];
}

} // namespace commands
