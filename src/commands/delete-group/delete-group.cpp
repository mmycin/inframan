#include "group-config.hpp"
#include "delete-group.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"
#include "rang.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace commands {

void DeleteGroup::execute() {
    DeleteGroup{}.run();
}

void DeleteGroup::run() {
    try {
        tabulate::Table header;
        header.add_row({"DELETE INFRASTRUCTURE GROUP"});
        header[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::red)
            .border_top("=")
            .border_bottom("=")
            .border_left("")
            .border_right("")
            .corner("");
        header.column(0).format().width(50);
        std::cout << "\n" << rang::fg::red << header << rang::fg::reset << "\n";

        selectGroup();
        confirmAndDelete();

    } catch (const std::exception& e) {
        std::cerr << "\n" << rang::fg::red << "Error: " << e.what() << rang::fg::reset << "\n";
    }
}

void DeleteGroup::selectGroup() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto groups = registry.listGroupNames();

    if (groups.empty())
        throw std::runtime_error("No infrastructure groups found.");

    std::cout << "\nAvailable Groups:\n";
    for (size_t i = 0; i < groups.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << groups[i] << "\n";
    }

    std::cout << "\nSelect group to delete (number): " << rang::style::bold;
    size_t choice;
    std::cin >> choice;
    std::cout << rang::style::reset;

    if (choice < 1 || choice > groups.size())
        throw std::runtime_error("Invalid selection.");

    group_name = groups[choice - 1];
}

void DeleteGroup::confirmAndDelete() {
    std::cout << "\n" << rang::fg::yellow << "Are you sure you want to delete group '" << group_name << "'?" << rang::fg::reset << "\n"
              << rang::style::bold << "This will also delete ALL jobs in this group. (y/n): " << rang::style::reset;
    char confirm;
    std::cin >> confirm;

    if (confirm == 'y' || confirm == 'Y') {
        GroupRegistry registry(GroupConfig::registry_file);
        registry.deleteGroup(group_name);

        if (ContextManager::getActiveGroup() == group_name) {
            ContextManager::clearActiveGroup();
            std::cout << rang::fg::yellow << "Cleared active group context." << rang::fg::reset << "\n";
        }

        tabulate::Table success;
        success.add_row({"SUCCESS", "Group '" + group_name + "' deleted."});
        success[0][0].format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold});
        std::cout << "\n" << rang::fg::green << success << rang::fg::reset << "\n";
    } else {
        std::cout << "\n" << rang::fg::yellow << "Deletion cancelled." << rang::fg::reset << "\n";
    }
}

} // namespace commands
