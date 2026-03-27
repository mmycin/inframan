#include "group-config.hpp"
#include "update-group.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace commands {

void UpdateGroup::execute() {
    UpdateGroup{}.run();
}

void UpdateGroup::run() {
    try {
        tabulate::Table header;
        header.add_row({"UPDATE INFRASTRUCTURE GROUP"});
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

        promptUpdates();

        GroupRegistry registry(GroupConfig::registry_file);
        registry.addOrUpdateGroup(group_name, GroupConfig::providerToString(provider), GroupConfig::typeToString(group_type));

        tabulate::Table success;
        success.add_row({"SUCCESS", "Group '" + group_name + "' updated."});
        success[0][0].format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold});
        std::cout << "\n" << success << "\n";

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
    }
}

void UpdateGroup::selectGroup() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto names = registry.listGroupNames();

    if (names.empty())
        throw std::runtime_error("No infrastructure groups found.");

    std::cout << "\nAvailable Groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << names[i] << "\n";

    std::cout << "\nSelect group to update (number): ";
    size_t choice;
    std::cin >> choice;

    if (choice < 1 || choice > names.size())
        throw std::runtime_error("Invalid selection.");

    group_name = names[choice - 1];
}

void UpdateGroup::promptUpdates() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto groups = registry.getGroups();
    auto group_data = groups.at(group_name);

    std::string current_provider = group_data.value("provider", "unknown");
    std::string current_type = group_data.value("type", "unknown");

    std::cout << "\nUpdating group '" << group_name << "'. Leave empty to keep current value.\n";

    std::cout << "Provider [" << current_provider << "] (1.docker, 2.podman, 3.containerd): ";
    std::string p_choice;
    std::cin.ignore();
    std::getline(std::cin, p_choice);
    
    if (p_choice.empty()) {
        provider = GroupConfig::providerFromString(current_provider);
    } else {
        try {
            int c = std::stoi(p_choice);
            switch (c) {
                case 1: provider = GroupConfig::Providers::DOCKER; break;
                case 2: provider = GroupConfig::Providers::PODMAN; break;
                case 3: provider = GroupConfig::Providers::CONTAINERD; break;
                default: throw std::runtime_error("Invalid provider choice");
            }
        } catch (...) {
            throw std::runtime_error("Invalid input for provider choice");
        }
    }

    std::cout << "Type [" << current_type << "] (1.dockerfile, 2.compose, 3.service, 4.task, 5.network, 6.volume, 7.custom): ";
    std::string t_choice;
    std::getline(std::cin, t_choice);
    
    if (t_choice.empty()) {
        group_type = GroupConfig::typeFromString(current_type);
    } else {
        try {
            int c = std::stoi(t_choice);
            switch (c) {
                case 1: group_type = GroupConfig::Type::DOCKERFILE; break;
                case 2: group_type = GroupConfig::Type::COMPOSE; break;
                case 3: group_type = GroupConfig::Type::SERVICE; break;
                case 4: group_type = GroupConfig::Type::TASK; break;
                case 5: group_type = GroupConfig::Type::NETWORK; break;
                case 6: group_type = GroupConfig::Type::VOLUME; break;
                case 7: group_type = GroupConfig::Type::CUSTOM; break;
                default: throw std::runtime_error("Invalid type choice");
            }
        } catch (...) {
            throw std::runtime_error("Invalid input for type choice");
        }
    }
}

} // namespace commands
