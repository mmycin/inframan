#include "group-config.hpp"
#include "list-group.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"
#include "libraries/rang.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace commands {

void ListGroup::execute() {
    ListGroup{}.run();
}

void ListGroup::run() {
    try {
        GroupRegistry registry(GroupConfig::registry_file);
        auto groups = registry.getGroups();

        tabulate::Table header;
        header.add_row({"INFRASTRUCTURE GROUPS"});
        header[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::cyan)
            .border_top("=")
            .border_bottom("=")
            .border_left("")
            .border_right("")
            .corner("");
        header.column(0).format().width(60);
        std::cout << "\n" << rang::fg::cyan << header << rang::fg::reset << "\n";

        if (groups.empty()) {
            std::cout << rang::fg::yellow << "No infrastructure groups found." << rang::fg::reset << "\n"
                      << "Create one with: " << rang::style::bold << "inframan cg" << rang::style::reset << "\n";
            return;
        }

        tabulate::Table table;
        table.add_row({"Group Name", "Provider", "Type", "Jobs"});

        std::string active_group = ContextManager::getActiveGroup();

        for (auto it = groups.begin(); it != groups.end(); ++it) {
            std::string name = it.key();
            auto data = it.value();
            
            std::string display_name = name;
            if (name == active_group) {
                display_name += " (*)";
            }

            int job_count = data.contains("jobs") ? static_cast<int>(data["jobs"].size()) : 0;

            table.add_row({
                display_name,
                data.value("provider", "unknown"),
                data.value("type", "unknown"),
                std::to_string(job_count)
            });
        }

        // Apply styling
        table[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::yellow);

        for (size_t i = 0; i < 4; ++i) {
            table.column(i).format().font_align(tabulate::FontAlign::center);
        }

        std::cout << table << "\n";
        
        if (!active_group.empty()) {
            std::cout << "(*) Active group context: " << rang::fg::yellow << active_group << rang::fg::reset << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "\n" << rang::fg::red << "Error listing groups: " << e.what() << rang::fg::reset << "\n";
    }
}

} // namespace commands
