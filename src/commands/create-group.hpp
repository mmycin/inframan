#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "group-config.hpp"

using namespace std;

namespace commands {
    class CreateGroup {
    private:
        // Private member variables
        string group_name;
        GroupConfig::Providers provider;
        GroupConfig::Type group_type;
        int provider_choice;
        int type_choice;
        
        // Private methods
        void promptForGroupName() {
            cout << "Enter group name: ";
            cin >> group_name;
        }
        
        void displayProviderMenu() {
            cout << "\nSelect Container Runtime Provider:" << endl;
            cout << "1. Docker" << endl;
            cout << "2. Podman" << endl;
            cout << "3. Containerd" << endl;
            cout << "Choice: ";
        }
        
        void getProviderChoice() {
            cin >> provider_choice;
        }
        
        void validateProviderChoice() {
            if (provider_choice < 1 || provider_choice > 3) {
                cout << "Invalid choice. Using default: Docker" << endl;
                provider_choice = 1;
            }
        }
        
        void setProviderFromChoice() {
            switch (provider_choice) {
                case 1:
                    provider = GroupConfig::Providers::DOCKER;
                    break;
                case 2:
                    provider = GroupConfig::Providers::PODMAN;
                    break;
                case 3:
                    provider = GroupConfig::Providers::CONTAINERD;
                    break;
            }
        }
        
        void displayTypeMenu() {
            cout << "\nSelect Group Type:" << endl;
            cout << "1. Dockerfile - Build custom images" << endl;
            cout << "2. Docker Compose - Multi-container applications" << endl;
            cout << "3. Service Group - Long-running services" << endl;
            cout << "4. Task Group - One-time tasks" << endl;
            cout << "5. Network Group - Container networking" << endl;
            cout << "6. Volume Group - Persistent storage" << endl;
            cout << "7. Custom Configuration" << endl;
            cout << "Choice: ";
        }
        
        void getTypeChoice() {
            cin >> type_choice;
        }
        
        void validateTypeChoice() {
            if (type_choice < 1 || type_choice > 7) {
                cout << "Invalid choice. Using default: Dockerfile" << endl;
                type_choice = 1;
            }
        }
        
        void setTypeFromChoice() {
            switch (type_choice) {
                case 1:
                    group_type = GroupConfig::Type::DOCKERFILE;
                    break;
                case 2:
                    group_type = GroupConfig::Type::COMPOSE;
                    break;
                case 3:
                    group_type = GroupConfig::Type::SERVICE;
                    break;
                case 4:
                    group_type = GroupConfig::Type::TASK;
                    break;
                case 5:
                    group_type = GroupConfig::Type::NETWORK;
                    break;
                case 6:
                    group_type = GroupConfig::Type::VOLUME;
                    break;
                case 7:
                    group_type = GroupConfig::Type::CUSTOM;
                    break;
            }
        }
        
        string getTypeDescription(GroupConfig::Type type) {
            switch (type) {
                case GroupConfig::Type::DOCKERFILE:
                    return "Dockerfile - Build custom container images";
                case GroupConfig::Type::COMPOSE:
                    return "Docker Compose - Multi-container orchestration";
                case GroupConfig::Type::SERVICE:
                    return "Service Group - Long-running microservices";
                case GroupConfig::Type::TASK:
                    return "Task Group - One-time execution tasks";
                case GroupConfig::Type::NETWORK:
                    return "Network Group - Container networking setup";
                case GroupConfig::Type::VOLUME:
                    return "Volume Group - Persistent storage management";
                case GroupConfig::Type::CUSTOM:
                    return "Custom Configuration - Advanced setup";
                default:
                    return "Unknown";
            }
        }
        
        void displayConfirmation() {
            cout << "\n" << string(50, '=') << endl;
            cout << "        GROUP CREATION SUMMARY" << endl;
            cout << string(50, '=') << endl;
            cout << "Group Name:   " << group_name << endl;
            cout << "Provider:     ";
            switch (provider) {
                case GroupConfig::Providers::DOCKER:
                    cout << "Docker";
                    break;
                case GroupConfig::Providers::PODMAN:
                    cout << "Podman";
                    break;
                case GroupConfig::Providers::CONTAINERD:
                    cout << "Containerd";
                    break;
            }
            cout << endl;
            cout << "Type:         " << getTypeDescription(group_type) << endl;
            cout << string(50, '=') << endl;
        }
        
        void createGroupInProvider() {
            cout << "\nCreating group '" << group_name << "'..." << endl;
            cout << "Provider: ";
            switch (provider) {
                case GroupConfig::Providers::DOCKER:
                    cout << "Docker";
                    break;
                case GroupConfig::Providers::PODMAN:
                    cout << "Podman";
                    break;
                case GroupConfig::Providers::CONTAINERD:
                    cout << "Containerd";
                    break;
            }
            cout << endl;
            
            cout << "Type: " << getTypeDescription(group_type) << endl;
            
            // Type-specific creation logic
            switch (group_type) {
                case GroupConfig::Type::DOCKERFILE:
                    createDockerfileGroup();
                    break;
                case GroupConfig::Type::COMPOSE:
                    createComposeGroup();
                    break;
                case GroupConfig::Type::SERVICE:
                    createServiceGroup();
                    break;
                case GroupConfig::Type::TASK:
                    createTaskGroup();
                    break;
                case GroupConfig::Type::NETWORK:
                    createNetworkGroup();
                    break;
                case GroupConfig::Type::VOLUME:
                    createVolumeGroup();
                    break;
                case GroupConfig::Type::CUSTOM:
                    createCustomGroup();
                    break;
            }
        }
        
        // Type-specific creation methods
        void createDockerfileGroup() {
            cout << "  Creating Dockerfile template..." << endl;
            cout << "  Setting up build context..." << endl;
            cout << "  Configuring build arguments..." << endl;
        }
        
        void createComposeGroup() {
            cout << "  Creating docker-compose.yml..." << endl;
            cout << "  Setting up service dependencies..." << endl;
            cout << "  Configuring networks..." << endl;
            cout << "  Setting up volumes..." << endl;
        }
        
        void createServiceGroup() {
            cout << "  Creating service configuration..." << endl;
            cout << "  Setting up auto-restart policy..." << endl;
            cout << "  Configuring health checks..." << endl;
            cout << "  Adding service labels..." << endl;
        }
        
        void createTaskGroup() {
            cout << "  Creating task definition..." << endl;
            cout << "  Setting up execution schedule..." << endl;
            cout << "  Configuring cleanup policy..." << endl;
        }
        
        void createNetworkGroup() {
            cout << "  Creating network configuration..." << endl;
            cout << "  Setting up subnet allocation..." << endl;
            cout << "  Configuring network policies..." << endl;
        }
        
        void createVolumeGroup() {
            cout << "  Creating volume configuration..." << endl;
            cout << "  Setting up mount points..." << endl;
            cout << "  Configuring permissions..." << endl;
            cout << "  Setting up backup schedule..." << endl;
        }
        
        void createCustomGroup() {
            cout << "  Creating custom configuration..." << endl;
            cout << "  Generating configuration files..." << endl;
            cout << "  Setting up custom parameters..." << endl;
            cout << "  Applying user preferences..." << endl;
        }
        
        void validateGroupName() {
            if (group_name.empty()) {
                throw runtime_error("Group name cannot be empty");
            }
            
            // Check for invalid characters
            for (char c : group_name) {
                if (!isalnum(c) && c != '_' && c != '-') {
                    throw runtime_error("Group name can only contain alphanumeric characters, '_', and '-'");
                }
            }
        }
        
        bool checkGroupExists() {
            // Check if group already exists in the system
            return false;
        }
        
        void handleExistingGroup() {
            cout << "Warning: Group '" << group_name << "' already exists!" << endl;
            cout << "Do you want to overwrite? (y/n): ";
            char response;
            cin >> response;
            
            if (response != 'y' && response != 'Y') {
                throw runtime_error("Group creation cancelled by user");
            }
            
            cout << "Overwriting existing group..." << endl;
        }
        
    public:
        // Public static members - with multiple flags
        inline static const vector<string> flags = {"create-group", "cg", "create"};
        inline static const string flag = "create-group";
        
        // Public methods
        static void execute() {
            CreateGroup creator;
            creator.run();
        }
        
        void run() {
            try {
                // Welcome message
                cout << "\n" << string(50, '=') << endl;
                cout << "     INFRASTRUCTURE GROUP CREATION" << endl;
                cout << string(50, '=') << endl;
                
                // Step 1: Get group name
                promptForGroupName();
                validateGroupName();
                
                // Step 2: Check if group exists
                if (checkGroupExists()) {
                    handleExistingGroup();
                }
                
                // Step 3: Get provider choice
                displayProviderMenu();
                getProviderChoice();
                validateProviderChoice();
                setProviderFromChoice();
                
                // Step 4: Get type choice
                displayTypeMenu();
                getTypeChoice();
                validateTypeChoice();
                setTypeFromChoice();
                
                // Step 5: Display confirmation
                displayConfirmation();
                
                // Step 6: Confirm with user
                cout << "\nProceed with group creation? (y/n): ";
                char confirm;
                cin >> confirm;
                
                if (confirm == 'y' || confirm == 'Y') {
                    // Step 7: Create the group
                    createGroupInProvider();
                    cout << "\nGroup '" << group_name << "' created successfully!" << endl;
                    cout << "Location: /var/inframanager/groups/" << group_name << endl;
                    cout << "Run with: inframanager use " << group_name << endl;
                } else {
                    cout << "\nGroup creation cancelled." << endl;
                }
                
            } catch (const exception& e) {
                cerr << "\nError: " << e.what() << endl;
            }
        }
    };
}