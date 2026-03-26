#include "add-job.hpp"
#include "group-registry.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

namespace commands {

static const string REGISTRY = "infragroups.json";

void AddJob::execute() { AddJob{}.run(); }

void AddJob::run() {
    try {
        cout << "\n" << string(50, '=') << "\n"
             << "           ADD JOB TO GROUP\n"
             << string(50, '=') << "\n";

        selectGroup();
        promptFields();
        validate();

        GroupRegistry reg(REGISTRY);
        reg.addOrUpdateJob(group_name, job_name, file_path, command);

        cout << "\nJob '" << job_name << "' added to group '" << group_name << "' successfully.\n";

    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << "\n";
    }
}

void AddJob::selectGroup() {
    GroupRegistry reg(REGISTRY);
    auto names = reg.listGroupNames();

    if (names.empty())
        throw runtime_error("No groups found. Create a group first with: inframanager cg");

    cout << "\nAvailable groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        cout << "  " << (i + 1) << ". " << names[i] << "\n";

    cout << "Select group (number): ";
    int choice;
    cin >> choice;

    if (choice < 1 || choice > static_cast<int>(names.size()))
        throw runtime_error("Invalid group selection");

    group_name = names[choice - 1];
}

void AddJob::promptFields() {
    cout << "Job name    : "; cin >> job_name;
    cout << "File path   : "; cin >> file_path;
    cout << "Command     : ";
    cin.ignore();
    getline(cin, command);
}

void AddJob::validate() {
    if (job_name.empty())   throw runtime_error("Job name cannot be empty");
    if (file_path.empty())  throw runtime_error("File path cannot be empty");
    if (command.empty())    throw runtime_error("Command cannot be empty");
}

} // namespace commands
