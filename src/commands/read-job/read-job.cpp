#include "read-job.hpp"
#include "group-registry.hpp"

#include <iostream>
#include <iomanip>

using namespace std;

namespace commands {

static const string REGISTRY = "infragroups.json";

void ReadJob::execute() { ReadJob{}.run(); }

void ReadJob::run() {
    try {
        cout << "\n" << string(50, '=') << "\n"
             << "           READ GROUP JOBS\n"
             << string(50, '=') << "\n";

        selectGroup();

        GroupRegistry reg(REGISTRY);
        auto jobs = reg.getJobs(group_name);

        if (jobs.empty()) {
            cout << "\nNo jobs found in group '" << group_name << "'.\n";
            return;
        }

        cout << "\nJobs in '" << group_name << "':\n"
             << string(68, '-') << "\n"
             << left << setw(20) << "JOB NAME" 
             << setw(20) << "FILE PATH" 
             << "COMMAND" << "\n"
             << string(68, '-') << "\n";

        for (const auto& [name, data] : jobs.items()) {
            string file = data.value("file_path", "unknown");
            string cmd = data.value("command", "unknown");

            cout << left << setw(20) << name
                 << setw(20) << file
                 << cmd << "\n";
        }
        cout << string(68, '-') << "\n";

    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << "\n";
    }
}

void ReadJob::selectGroup() {
    GroupRegistry reg(REGISTRY);
    auto names = reg.listGroupNames();

    if (names.empty())
        throw runtime_error("No groups found.");

    cout << "\nAvailable groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        cout << "  " << (i + 1) << ". " << names[i] << "\n";

    cout << "Select group to read (number): ";
    int choice; cin >> choice;

    if (choice < 1 || choice > static_cast<int>(names.size()))
        throw runtime_error("Invalid group selection");

    group_name = names[choice - 1];
}

} // namespace commands
