#pragma once
#include <string>
#include <vector>
#include "group-config.hpp"

namespace commands {

class CreateGroup {
public:
    inline static const std::string flag = "cg,--create-group";
    static const inline std::vector<std::string> flags = {"create-group", "cg"};

    static void execute();
    void run();

private:
    std::string group_name;
    GroupConfig::Providers provider = GroupConfig::Providers::DOCKER;
    GroupConfig::Type group_type   = GroupConfig::Type::DOCKERFILE;
    int provider_choice = 1;
    int type_choice     = 1;

    // Input / prompts
    void promptForGroupName();
    void displayProviderMenu();
    void getProviderChoice();
    void displayTypeMenu();
    void getTypeChoice();

    // Validation
    void validateGroupName();
    void validateProviderChoice();
    void validateTypeChoice();
    bool checkGroupExists();
    void handleExistingGroup();

    // Setup
    void setProviderFromChoice();
    void setTypeFromChoice();

    // Output
    void displayConfirmation();
    void createGroupInProvider();
    std::string getTypeDescription(GroupConfig::Type type);

    // Type-specific creation
    void createDockerfileGroup();
    void createComposeGroup();
    void createServiceGroup();
    void createTaskGroup();
    void createNetworkGroup();
    void createVolumeGroup();
    void createCustomGroup();

    // Registry persistence
    void saveToRegistry();
};

} // namespace commands