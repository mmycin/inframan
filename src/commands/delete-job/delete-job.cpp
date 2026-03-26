#include "delete-job.hpp"
#include "group-registry.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

namespace commands {

static const string REGISTRY = "infragroups.json";

void DeleteJob::execute() { DeleteJob{}.run(); }

void DeleteJob::run() {
    try {
        cout << "\n" << string(50, '=') << "\n"
             << "           DELETE INFRA JOB\n"
             << string(50, '=') << "\n";

        selectGroup();
        selectJob();
        confirmAndDelete();

    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << "\n";
    }
}

void DeleteJob::selectGroup() {
    GroupRegistry reg(REGISTRY);
    auto names = reg.listGroupNames();

    if (names.empty())
        throw runtime_error("No groups found.");

    cout << "\nAvailable groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        cout << "  " << (i + 1) << ". " << names[i] << "\n";

    cout << "Select group (number): ";
    int choice; cin >> choice;

    if (choice < 1 || choice > static_cast<int>(names.size()))
        throw runtime_error("Invalid group selection");

    group_name = names[choice - 1];
}

void DeleteJob::selectJob() {
    GroupRegistry reg(REGISTRY);
    auto jobs = reg.listJobNames(group_name);

    if (jobs.empty())
        throw runtime_error("No jobs found in group '" + group_name + "'");

    cout << "\nJobs in '" << group_name << "':\n";
    for (size_t i = 0; i < jobs.size(); ++i) {
        cout << "  " << (i + 1) << ". " << jobs[i] << "\n";
    }

    cout << "Select job to delete (number): ";
    int choice; cin >> choice;

    if (choice < 1 || choice > static_cast<int>(jobs.size()))
        throw runtime_error("Invalid job selection");

    job_name = jobs[choice - 1];
}

void DeleteJob::confirmAndDelete() {
    cout << "\nAre you sure you want to delete job '" << job_name << "'? (y/N): ";
    string confirm; cin >> confirm;

    if (confirm == "y" || confirm == "Y") {
        GroupRegistry reg(REGISTRY);
        reg.deleteJob(group_name, job_name);
        cout << "\nJob '" << job_name << "' has been deleted.\n";
    } else {
        cout << "\nDeletion cancelled.\n";
    }
}

} // namespace commands
