#include "group-config.hpp"
#include "use-group.hpp"
#include "group-registry.hpp"
#include "context-manager.hpp"
#include "libraries/tabulate.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace commands {

void UseGroup::execute() {
    UseGroup{}.run();
}

void UseGroup::run() {
    try {
        tabulate::Table header;
        header.add_row({"SET ACTIVE GROUP"});
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

        selectGroup();

        ContextManager::setActiveGroup(group_name);

        tabulate::Table success;
        success.add_row({"SUCCESS", "Active group set to '" + group_name + "'"});
        success[0][0].format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold});
        std::cout << "\n" << success << "\n";

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
    }
}

void UseGroup::selectGroup() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto names = registry.listGroupNames();

    if (names.empty())
        throw std::runtime_error("No infrastructure groups found.");

    std::cout << "\nAvailable Groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << names[i] << "\n";

    std::cout << "\nSelect group to use (number): ";
    size_t choice;
    std::cin >> choice;

    if (choice < 1 || choice > names.size())
        throw std::runtime_error("Invalid selection.");

    group_name = names[choice - 1];
}

} // namespace commands
