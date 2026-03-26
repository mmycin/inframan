#include "create-group.hpp"
#include "group-registry.hpp"
#include "group-config.hpp"

#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;

namespace commands {

// ─── Public ──────────────────────────────────────────────────────────────────

void CreateGroup::execute() {
    CreateGroup{}.run();
}

void CreateGroup::run() {
    try {
        cout << "\n" << string(50, '=') << "\n"
                  << "     INFRASTRUCTURE GROUP CREATION\n"
                  << string(50, '=') << "\n";

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

        cout << "\nProceed with group creation? (y/n): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y') {
            createGroupInProvider();
            saveToRegistry();

            cout << "\nGroup '" << group_name << "' created successfully!\n"
                      << "Location: " << GroupConfig::group_path << group_name << "\n"
                      << "Run with: inframanager use " << group_name << "\n";
        } else {
            cout << "\nGroup creation cancelled.\n";
        }

    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << "\n";
    }
}

// ─── Input / Prompts ─────────────────────────────────────────────────────────

void CreateGroup::promptForGroupName() {
    cout << "Enter group name: ";
    cin >> group_name;
}

void CreateGroup::displayProviderMenu() {
    cout << "\nSelect Container Runtime Provider:\n"
              << "  1. Docker\n"
              << "  2. Podman\n"
              << "  3. Containerd\n"
              << "Choice: ";
}

void CreateGroup::getProviderChoice() {
    cin >> provider_choice;
}

void CreateGroup::displayTypeMenu() {
    cout << "\nSelect Group Type:\n"
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
    cin >> type_choice;
}

// ─── Validation ──────────────────────────────────────────────────────────────

void CreateGroup::validateGroupName() {
    if (group_name.empty())
        throw runtime_error("Group name cannot be empty");

    for (char c : group_name)
        if (!isalnum(c) && c != '_' && c != '-')
            throw runtime_error(
                "Group name can only contain alphanumeric characters, '_', and '-'");
}

void CreateGroup::validateProviderChoice() {
    if (provider_choice < 1 || provider_choice > 3) {
        cout << "Invalid choice. Using default: Docker\n";
        provider_choice = 1;
    }
}

void CreateGroup::validateTypeChoice() {
    if (type_choice < 1 || type_choice > 7) {
        cout << "Invalid choice. Using default: Dockerfile\n";
        type_choice = 1;
    }
}

bool CreateGroup::checkGroupExists() {
    GroupRegistry registry("infragroups.json");
    return registry.groupExists(group_name);
}

void CreateGroup::handleExistingGroup() {
    cout << "Warning: Group '" << group_name << "' already exists!\n"
              << "Overwrite? (y/n): ";
    char r;
    cin >> r;
    if (r != 'y' && r != 'Y')
        throw runtime_error("Group creation cancelled by user");
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

string CreateGroup::getTypeDescription(GroupConfig::Type type) {
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
    cout << "\n" << string(50, '=') << "\n"
              << "        GROUP CREATION SUMMARY\n"
              << string(50, '=') << "\n"
              << "Group Name : " << group_name << "\n"
              << "Provider   : " << GroupConfig::providerToString(provider) << "\n"
              << "Type       : " << getTypeDescription(group_type) << "\n"
              << string(50, '=') << "\n";
}

// ─── Type-specific creation ──────────────────────────────────────────────────

void CreateGroup::createGroupInProvider() {
    cout << "\nCreating group '" << group_name << "'...\n";
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
    cout << "  Creating Dockerfile template...\n"
              << "  Setting up build context...\n";
}

void CreateGroup::createComposeGroup() {
    cout << "  Creating docker-compose.yml...\n"
              << "  Setting up service dependencies...\n";
}

void CreateGroup::createServiceGroup() {
    cout << "  Creating service configuration...\n"
              << "  Setting up auto-restart policy...\n";
}

void CreateGroup::createTaskGroup() {
    cout << "  Creating task definition...\n"
              << "  Setting up execution schedule...\n";
}

void CreateGroup::createNetworkGroup() {
    cout << "  Creating network configuration...\n"
              << "  Setting up subnet allocation...\n";
}

void CreateGroup::createVolumeGroup() {
    cout << "  Creating volume configuration...\n"
              << "  Setting up mount points...\n";
}

void CreateGroup::createCustomGroup() {
    cout << "  Creating custom configuration...\n"
              << "  Applying user preferences...\n";
}

// ─── Registry persistence ────────────────────────────────────────────────────

void CreateGroup::saveToRegistry() {
    const string registry_file = "infragroups.json";
    GroupRegistry registry(registry_file);

    registry.addOrUpdateGroup(
        group_name,
        GroupConfig::providerToString(provider),
        GroupConfig::typeToString(group_type)
    );

    // Verify the write succeeded
    if (!registry.groupExists(group_name))
        throw runtime_error("Registry verification failed for group: " + group_name);

    cout << "  Registry updated: " << registry_file << "\n";
}

} // namespace commands
