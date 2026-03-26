#include "update-job.hpp"
#include "group-registry.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

namespace commands {

static const string REGISTRY = "infragroups.json";

void UpdateJob::execute() { UpdateJob{}.run(); }

void UpdateJob::run() {
    try {
        cout << "\n" << string(50, '=') << "\n"
             << "           UPDATE INFRA JOB\n"
             << string(50, '=') << "\n";

        selectGroup();
        selectJob();
        promptUpdates();

        GroupRegistry reg(REGISTRY);
        reg.addOrUpdateJob(group_name, job_name, file_path, command);

        cout << "\nJob '" << job_name << "' updated successfully.\n";

    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << "\n";
    }
}

void UpdateJob::selectGroup() {
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

void UpdateJob::selectJob() {
    GroupRegistry reg(REGISTRY);
    auto jobs = reg.listJobNames(group_name);

    if (jobs.empty())
        throw runtime_error("No jobs found in group '" + group_name + "'");

    cout << "\nJobs in '" << group_name << "':\n";
    for (size_t i = 0; i < jobs.size(); ++i) {
        cout << "  " << (i + 1) << ". " << jobs[i] << "\n";
    }

    cout << "Select job to update (number): ";
    int choice; cin >> choice;

    if (choice < 1 || choice > static_cast<int>(jobs.size()))
        throw runtime_error("Invalid job selection");

    job_name = jobs[choice - 1];
}

void UpdateJob::promptUpdates() {
    GroupRegistry reg(REGISTRY);
    auto job_data = reg.getJobs(group_name).at(job_name);

    cout << "\nUpdating job '" << job_name << "'. Leave empty to keep current value.\n";

    string current_file = job_data.value("file_path", "unknown");
    string current_cmd = job_data.value("command", "unknown");

    cout << "File path [" << current_file << "]: ";
    string input_file;
    cin.ignore();
    getline(cin, input_file);
    file_path = input_file.empty() ? current_file : input_file;

    cout << "Command   [" << current_cmd << "]: ";
    string input_cmd;
    getline(cin, input_cmd);
    command = input_cmd.empty() ? current_cmd : input_cmd;
}

} // namespace commands
