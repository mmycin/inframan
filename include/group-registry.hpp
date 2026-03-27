#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include "libraries/json.hpp"
#include "libraries/tfile.hpp"
#include <filesystem>

// GroupRegistry: Single source of truth for the registry file.
// All file I/O uses tfile.hpp for cross-platform compatibility.
class GroupRegistry {
public:
    explicit GroupRegistry(const std::string& registry_path)
        : path_(registry_path) {}

    // ─── Group Operations ────────────────────────────────────────────────────

    void addOrUpdateGroup(const std::string& name,
                          const std::string& provider,
                          const std::string& type) {
        nlohmann::json root = load();
        root["groups"][name]["name"]     = name;
        root["groups"][name]["provider"] = provider;
        root["groups"][name]["type"]     = type;
        if (!root["groups"][name].contains("jobs"))
            root["groups"][name]["jobs"] = nlohmann::json::object();
        save(root);
    }

    void deleteGroup(const std::string& name) {
        nlohmann::json root = load();
        requireGroup(root, name);
        root["groups"].erase(name);
        save(root);
    }

    nlohmann::json getGroups() {
        return load().value("groups", nlohmann::json::object());
    }

    bool groupExists(const std::string& name) {
        return getGroups().contains(name);
    }

    std::vector<std::string> listGroupNames() {
        std::vector<std::string> names;
        for (auto& item : getGroups().items())
            names.push_back(item.key());
        return names;
    }

    // ─── Job Operations ──────────────────────────────────────────────────────

    void addOrUpdateJob(const std::string& group,
                        const std::string& job_name,
                        const std::string& file_path,
                        const std::string& command) {
        nlohmann::json root = load();
        requireGroup(root, group);
        root["groups"][group]["jobs"][job_name] = {
            {"name",      job_name},
            {"file_path", file_path},
            {"command",   command}
        };
        save(root);
    }

    void deleteJob(const std::string& group, const std::string& job_name) {
        nlohmann::json root = load();
        requireGroup(root, group);
        requireJob(root, group, job_name);
        root["groups"][group]["jobs"].erase(job_name);
        save(root);
    }

    nlohmann::json getJobs(const std::string& group) {
        nlohmann::json root = load();
        requireGroup(root, group);
        return root["groups"][group].value("jobs", nlohmann::json::object());
    }

    std::vector<std::string> listJobNames(const std::string& group) {
        std::vector<std::string> names;
        for (auto& item : getJobs(group).items())
            names.push_back(item.key());
        return names;
    }

    bool jobExists(const std::string& group, const std::string& job_name) {
        return getJobs(group).contains(job_name);
    }

private:
    std::string path_;

    nlohmann::json load() {
        try {
            std::string content = tfile::read(path_.c_str());
            if (!content.empty())
                return nlohmann::json::parse(content);
        } catch (...) {}
        return nlohmann::json{{"groups", nlohmann::json::object()}};
    }

    void save(const nlohmann::json& root) {
        try {
            std::filesystem::path p(path_);
            if (p.has_parent_path()) {
                std::filesystem::create_directories(p.parent_path());
            }
            tfile::write(path_.c_str(), root.dump(4));
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Registry write failed: ") + e.what());
        }
    }

    void requireGroup(const nlohmann::json& root, const std::string& name) {
        if (!root["groups"].contains(name))
            throw std::runtime_error("Group not found: " + name);
    }

    void requireJob(const nlohmann::json& root,
                    const std::string& group,
                    const std::string& job_name) {
        if (!root["groups"][group]["jobs"].contains(job_name))
            throw std::runtime_error("Job not found: " + job_name);
    }
};
