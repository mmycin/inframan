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
        std::cout << "Enter file path: ";
        std::cin >> file_path;
        std::cout << "Enter shell command: ";
        std::cin.ignore();
        std::getline(std::cin, command);

        GroupRegistry registry(GroupConfig::registry_file);
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
