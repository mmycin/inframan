#pragma once
#include <string>
#include <cstdio>
#include "libraries/tfile.hpp"

// ContextManager: Handles the storage and retrieval of the active infrastructure group.
// This allows commands to operate on a "current" group without repeated selection.
class ContextManager {
public:
    static constexpr const char* CONTEXT_FILE = ".infra_context";

    static void setActiveGroup(const std::string& group_name) {
        tfile::write(CONTEXT_FILE, group_name);
    }

    static std::string getActiveGroup() {
        try {
            std::string content = tfile::read(CONTEXT_FILE);
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
        std::remove(CONTEXT_FILE);
    }

    static bool hasActiveGroup() {
        return !getActiveGroup().empty();
    }
};
