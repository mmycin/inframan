#pragma once
#include <string>
#include <vector>
#include "group-config.hpp"

using namespace std;

namespace commands {

class CreateGroup {
public:
    inline static const vector<string> flags = {"create-group", "cg", "create"};
    inline static const string flag = "create-group";

    static void execute();
    void run();

private:
    string group_name;
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
    string getTypeDescription(GroupConfig::Type type);

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