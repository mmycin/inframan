#include "delete-group.hpp"
#include "group-registry.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

namespace commands {

static const string REGISTRY = "infragroups.json";

void DeleteGroup::execute() { DeleteGroup{}.run(); }

void DeleteGroup::run() {
    try {
        cout << "\n" << string(50, '=') << "\n"
             << "           DELETE INFRA GROUP\n"
             << string(50, '=') << "\n";

        selectGroup();
        confirmAndDelete();

    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << "\n";
    }
}

void DeleteGroup::selectGroup() {
    GroupRegistry reg(REGISTRY);
    auto names = reg.listGroupNames();

    if (names.empty())
        throw runtime_error("No groups found. Create a group first with: inframanager cg");

    cout << "\nAvailable groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        cout << "  " << (i + 1) << ". " << names[i] << "\n";

    cout << "Select group to delete (number): ";
    int choice;
    cin >> choice;

    if (choice < 1 || choice > static_cast<int>(names.size()))
        throw runtime_error("Invalid group selection");

    group_name = names[choice - 1];
}

void DeleteGroup::confirmAndDelete() {
    cout << "\nWARNING: Deleting group '" << group_name << "' will also delete all associated jobs.\n"
         << "Are you sure? (y/N): ";
    
    string confirm;
    cin >> confirm;

    if (confirm == "y" || confirm == "Y") {
        GroupRegistry reg(REGISTRY);
        reg.deleteGroup(group_name);
        cout << "\nGroup '" << group_name << "' and all its jobs have been deleted.\n";
    } else {
        cout << "\nDeletion cancelled.\n";
    }
}

} // namespace commands
