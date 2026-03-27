#include "create-group.hpp"
#include "group-registry.hpp"
#include "group-config.hpp"
#include "libraries/tabulate.hpp"

#include <iostream>
#include <string>
#include <stdexcept>

namespace commands {

// ─── Public ──────────────────────────────────────────────────────────────────

void CreateGroup::execute() {
    CreateGroup{}.run();
}

void CreateGroup::run() {
    try {
        tabulate::Table header;
        header.add_row({"INFRASTRUCTURE GROUP CREATION"});
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

        promptForGroupName();
        validateGroupName();

        if (checkGroupExists())
            handleExistingGroup();

        displayProviderMenu();
        getProviderChoice();
        validateProviderChoice();
        setProviderFromChoice();

        displayTypeMenu();
        getTypeChoice();
        validateTypeChoice();
        setTypeFromChoice();

        displayConfirmation();

        std::cout << "\nProceed with group creation? (y/n): ";
        char confirm;
        std::cin >> confirm;

        if (confirm == 'y' || confirm == 'Y') {
            createGroupInProvider();
            saveToRegistry();

            std::cout << "\nGroup '" << group_name << "' created successfully!\n"
                      << "Location: " << GroupConfig::group_path << group_name << "\n"
                      << "Run with: inframan use " << group_name << "\n";
        } else {
            std::cout << "\nGroup creation cancelled.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
    }
}

// ─── Input / Prompts ─────────────────────────────────────────────────────────

void CreateGroup::promptForGroupName() {
    std::cout << "Enter group name: ";
    std::cin >> group_name;
}

void CreateGroup::displayProviderMenu() {
    std::cout << "\nSelect Container Runtime Provider:\n"
              << "  1. Docker\n"
              << "  2. Podman\n"
              << "  3. Containerd\n"
              << "Choice: ";
}

void CreateGroup::getProviderChoice() {
    std::cin >> provider_choice;
}

void CreateGroup::displayTypeMenu() {
    std::cout << "\nSelect Group Type:\n"
              << "  1. Dockerfile  - Build custom images\n"
              << "  2. Compose     - Multi-container applications\n"
              << "  3. Service     - Long-running services\n"
              << "  4. Task        - One-time tasks\n"
              << "  5. Network     - Container networking\n"
              << "  6. Volume      - Persistent storage\n"
              << "  7. Custom      - Custom configuration\n"
              << "Choice: ";
}

void CreateGroup::getTypeChoice() {
    std::cin >> type_choice;
}

// ─── Validation ──────────────────────────────────────────────────────────────

void CreateGroup::validateGroupName() {
    if (group_name.empty())
        throw std::runtime_error("Group name cannot be empty");

    for (char c : group_name)
        if (!isalnum(c) && c != '_' && c != '-')
            throw std::runtime_error(
                "Group name can only contain alphanumeric characters, '_', and '-'");
}

void CreateGroup::validateProviderChoice() {
    if (provider_choice < 1 || provider_choice > 3) {
        std::cout << "Invalid choice. Using default: Docker\n";
        provider_choice = 1;
    }
}

void CreateGroup::validateTypeChoice() {
    if (type_choice < 1 || type_choice > 7) {
        std::cout << "Invalid choice. Using default: Dockerfile\n";
        type_choice = 1;
    }
}

bool CreateGroup::checkGroupExists() {
    GroupRegistry registry(GroupConfig::registry_file);
    return registry.groupExists(group_name);
}

void CreateGroup::handleExistingGroup() {
    std::cout << "Warning: Group '" << group_name << "' already exists!\n"
              << "Overwrite? (y/n): ";
    char r;
    std::cin >> r;
    if (r != 'y' && r != 'Y')
        throw std::runtime_error("Group creation cancelled by user");
}

// ─── Setup ───────────────────────────────────────────────────────────────────

void CreateGroup::setProviderFromChoice() {
    switch (provider_choice) {
        case 1: provider = GroupConfig::Providers::DOCKER;     break;
        case 2: provider = GroupConfig::Providers::PODMAN;     break;
        case 3: provider = GroupConfig::Providers::CONTAINERD; break;
    }
}

void CreateGroup::setTypeFromChoice() {
    switch (type_choice) {
        case 1: group_type = GroupConfig::Type::DOCKERFILE; break;
        case 2: group_type = GroupConfig::Type::COMPOSE;    break;
        case 3: group_type = GroupConfig::Type::SERVICE;    break;
        case 4: group_type = GroupConfig::Type::TASK;       break;
        case 5: group_type = GroupConfig::Type::NETWORK;    break;
        case 6: group_type = GroupConfig::Type::VOLUME;     break;
        case 7: group_type = GroupConfig::Type::CUSTOM;     break;
    }
}

// ─── Output / Display ────────────────────────────────────────────────────────

std::string CreateGroup::getTypeDescription(GroupConfig::Type type) {
    switch (type) {
        case GroupConfig::Type::DOCKERFILE: return "Dockerfile - Build custom container images";
        case GroupConfig::Type::COMPOSE:    return "Compose - Multi-container orchestration";
        case GroupConfig::Type::SERVICE:    return "Service - Long-running microservices";
        case GroupConfig::Type::TASK:       return "Task - One-time execution tasks";
        case GroupConfig::Type::NETWORK:    return "Network - Container networking setup";
        case GroupConfig::Type::VOLUME:     return "Volume - Persistent storage management";
        case GroupConfig::Type::CUSTOM:     return "Custom - Advanced setup";
        default:                            return "Unknown";
    }
}

void CreateGroup::displayConfirmation() {
    tabulate::Table summary;
    summary.add_row({"GROUP CREATION SUMMARY", ""});
    summary.add_row({"Group Name", group_name});
    summary.add_row({"Provider", GroupConfig::providerToString(provider)});
    summary.add_row({"Type", getTypeDescription(group_type)});

    summary[0].format()
        .font_style({tabulate::FontStyle::bold})
        .font_align(tabulate::FontAlign::center)
        .font_color(tabulate::Color::cyan)
        .border_top("=")
        .border_bottom("=")
        .border_left("")
        .border_right("")
        .corner("");
    
    summary.column(0).format().font_style({tabulate::FontStyle::bold}).width(15);
    summary.column(1).format().width(35);

    std::cout << "\n" << summary << "\n";
}

// ─── Type-specific creation ──────────────────────────────────────────────────

void CreateGroup::createGroupInProvider() {
    std::cout << "\nCreating group '" << group_name << "'...\n";
    switch (group_type) {
        case GroupConfig::Type::DOCKERFILE: createDockerfileGroup(); break;
        case GroupConfig::Type::COMPOSE:    createComposeGroup();    break;
        case GroupConfig::Type::SERVICE:    createServiceGroup();    break;
        case GroupConfig::Type::TASK:       createTaskGroup();       break;
        case GroupConfig::Type::NETWORK:    createNetworkGroup();    break;
        case GroupConfig::Type::VOLUME:     createVolumeGroup();     break;
        case GroupConfig::Type::CUSTOM:     createCustomGroup();     break;
    }
}

void CreateGroup::createDockerfileGroup() {
    std::cout << "  Creating Dockerfile template...\n"
              << "  Setting up build context...\n";
}

void CreateGroup::createComposeGroup() {
    std::cout << "  Creating docker-compose.yml...\n"
              << "  Setting up service dependencies...\n";
}

void CreateGroup::createServiceGroup() {
    std::cout << "  Creating service configuration...\n"
              << "  Setting up auto-restart policy...\n";
}

void CreateGroup::createTaskGroup() {
    std::cout << "  Creating task definition...\n"
              << "  Setting up execution schedule...\n";
}

void CreateGroup::createNetworkGroup() {
    std::cout << "  Creating network configuration...\n"
              << "  Setting up subnet allocation...\n";
}

void CreateGroup::createVolumeGroup() {
    std::cout << "  Creating volume configuration...\n"
              << "  Setting up mount points...\n";
}

void CreateGroup::createCustomGroup() {
    std::cout << "  Creating custom configuration...\n"
              << "  Applying user preferences...\n";
}

// ─── Registry persistence ────────────────────────────────────────────────────

void CreateGroup::saveToRegistry() {
    const std::string registry_file = GroupConfig::registry_file;
    GroupRegistry registry(registry_file);

    registry.addOrUpdateGroup(
        group_name,
        GroupConfig::providerToString(provider),
        GroupConfig::typeToString(group_type)
    );

    // Verify the write succeeded
    if (!registry.groupExists(group_name))
        throw std::runtime_error("Registry verification failed for group: " + group_name);

    std::cout << "  Registry updated: " << registry_file << "\n";
}

} // namespace commands
