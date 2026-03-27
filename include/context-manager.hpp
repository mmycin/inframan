#pragma once
#include <string>
#include <cstdio>
#include "libraries/tfile.hpp"
#include "group-config.hpp"
#include <filesystem>


// ContextManager: Handles the storage and retrieval of the active infrastructure group.
// This allows commands to operate on a "current" group without repeated selection.
class ContextManager {
public:
    static std::string getContextFile() {
        return GroupConfig::group_path + ".infra_context";
    }

    static void setActiveGroup(const std::string& group_name) {
        try {
            std::filesystem::path p(getContextFile());
            if (p.has_parent_path()) {
                std::filesystem::create_directories(p.parent_path());
            }
        } catch (...) {}
        tfile::write(getContextFile().c_str(), group_name);
    }

    static std::string getActiveGroup() {
        try {
            std::string content = tfile::read(getContextFile().c_str());
            if (content.empty()) return "";
            // Trim whitespace/newlines if any
            auto last = content.find_last_not_of(" \n\r\t");
            if (last != std::string::npos) {
                content.erase(last + 1);
            }
            return content;
        } catch (...) {
            return "";
        }
    }

    static void clearActiveGroup() {
        std::remove(getContextFile().c_str());
    }

    static bool hasActiveGroup() {
        return !getActiveGroup().empty();
    }
};
