#pragma once
#include <string>
#include <stdexcept>
#include "json.hpp"
#include "tfile.hpp"

using namespace std;
using json = nlohmann::json;

// GroupRegistry: Manages persistence of group configs to infragroups.json.
// All file I/O uses tfile.hpp for cross-platform compatibility.
class GroupRegistry {
public:
    explicit GroupRegistry(const string& registry_path)
        : path_(registry_path) {}

    // Adds or updates a group entry. Returns true on success.
    bool addOrUpdateGroup(const string& name,
                          const string& provider,
                          const string& type) {
        json root = loadRegistry();
        root["groups"][name] = {
            {"name",     name},
            {"provider", provider},
            {"type",     type}
        };
        return saveRegistry(root);
    }

    // Returns all groups as a json object, or empty object on failure.
    json getGroups() {
        json root = loadRegistry();
        return root.value("groups", json::object());
    }

    // Verifies a group exists in the registry.
    bool verifyGroup(const string& name) {
        json groups = getGroups();
        return groups.contains(name);
    }

private:
    string path_;

    // Loads infragroups.json; returns fresh object if file doesn't exist yet.
    json loadRegistry() {
        try {
            string content = tfile::read(path_.c_str());
            if (!content.empty())
                return json::parse(content);
        } catch (...) {
            // File doesn't exist or is unreadable — start fresh
        }
        return json{{"groups", json::object()}};
    }

    // Serialises and writes the registry; returns true on success.
    bool saveRegistry(const json& root) {
        try {
            string content = root.dump(4);  // 4-space pretty print
            tfile::write(path_.c_str(), content);
            return true;
        } catch (const exception& e) {
            throw runtime_error(
                string("Failed to save registry: ") + e.what());
        }
    }
};
