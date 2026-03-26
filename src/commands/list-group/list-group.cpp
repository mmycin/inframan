#include "list-group.hpp"
#include "group-registry.hpp"

#include <iostream>
#include <iomanip>

using namespace std;

namespace commands {

static const string REGISTRY = "infragroups.json";

void ListGroup::execute() { ListGroup{}.run(); }

void ListGroup::run() {
    try {
        GroupRegistry reg(REGISTRY);
        auto groups = reg.getGroups();

        if (groups.empty()) {
            cout << "\nNo infrastructure groups found. Create one with: inframanager cg\n";
            return;
        }

        cout << "\n" << string(68, '=') << "\n"
             << left << setw(20) << "GROUP NAME" 
             << setw(15) << "PROVIDER" 
             << setw(15) << "TYPE" 
             << setw(10) << "JOBS" << "\n"
             << string(68, '=') << "\n";

        for (const auto& [name, data] : groups.items()) {
            string provider = data.value("provider", "unknown");
            string type = data.value("type", "unknown");
            int job_count = data.contains("jobs") ? static_cast<int>(data["jobs"].size()) : 0;

            cout << left << setw(20) << name
                 << setw(15) << provider
                 << setw(15) << type
                 << setw(10) << job_count << "\n";
        }
        cout << string(68, '=') << "\n";

    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << "\n";
    }
}

} // namespace commands
