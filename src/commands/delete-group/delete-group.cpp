#include "group-config.hpp"
#include "delete-group.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept> // Keep this for runtime_error

namespace commands {

// static const string REGISTRY = GroupConfig::registry_file; // Removed as per instruction

void DeleteGroup::execute() {
    DeleteGroup{}.run();
}

void DeleteGroup::run() {
    try {
        tabulate::Table header;
        header.add_row({"DELETE INFRASTRUCTURE GROUP"}); // Changed text
        header[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::red) // Changed color
            .border_top("=")
            .border_bottom("=")
            .border_left("")
            .border_right("")
            .corner("");
        header.column(0).format().width(50);
        std::cout << "\n" << header << "\n"; // Added std::

        selectGroup();
        confirmAndDelete();

    } catch (const std::exception& e) { // Added std::
        std::cerr << "\nError: " << e.what() << "\n"; // Added std::
    }
}

void DeleteGroup::selectGroup() {
    GroupRegistry registry(GroupConfig::registry_file); // Changed variable name and used literal
    auto groups = registry.listGroupNames(); // Changed variable name

    if (groups.empty())
        throw std::runtime_error("No infrastructure groups found."); // Added std:: and changed message

    std::cout << "\nAvailable Groups:\n"; // Added std:: and changed text
    for (size_t i = 0; i < groups.size(); ++i) { // Changed variable name, added braces
        std::cout << "  " << (i + 1) << ". " << groups[i] << "\n"; // Added std:: and changed variable name
    }

    std::cout << "\nSelect group to delete (number): "; // Added std:: and newline
    size_t choice; // Changed type
    std::cin >> choice; // Added std::

    if (choice < 1 || choice > groups.size()) // Changed variable name
        throw std::runtime_error("Invalid selection."); // Added std:: and changed message

    group_name = groups[choice - 1]; // Changed variable name
}

void DeleteGroup::confirmAndDelete() {
    // Removed warning table
    std::cout << "\nAre you sure you want to delete group '" << group_name << "'?\n" // Added std:: and new prompt
              << "This will also delete ALL jobs in this group. (y/n): ";
    char confirm; // Changed type
    std::cin >> confirm; // Added std::

    if (confirm == 'y' || confirm == 'Y') { // Changed to char comparison
        GroupRegistry registry(GroupConfig::registry_file); // Changed variable name and used literal
        registry.deleteGroup(group_name); // Changed variable name

        // Clear context if this was the active group
        if (ContextManager::getActiveGroup() == group_name) {
            ContextManager::clearActiveGroup();
            std::cout << "Cleared active group context.\n"; // Added std:: and changed message
        }

        tabulate::Table success; // Added tabulate::
        success.add_row({"SUCCESS", "Group '" + group_name + "' deleted."}); // Changed content and format
        success[0][0].format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold}); // Added tabulate::
        std::cout << "\n" << success << "\n"; // Added std::
    } else {
        std::cout << "\nDeletion cancelled.\n"; // Added std::
    }
}

} // namespace commands
