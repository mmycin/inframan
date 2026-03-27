#include "configs/group-config.hpp"
#include "include/group-registry.hpp"
#include "include/context-manager.hpp"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        std::cerr << "DEBUG: Starting test program\n";
        
        // Test getting active group
        std::cerr << "DEBUG: Getting active group\n";
        std::string group_name = ContextManager::getActiveGroup();
        std::cerr << "DEBUG: Got active group: '" << group_name << "'\n";
        
        if (group_name.empty()) {
            std::cerr << "DEBUG: No active group found\n";
            return 0;
        }
        
        // Test creating registry and listing jobs
        std::cerr << "DEBUG: Creating registry\n";
        GroupRegistry registry(GroupConfig::registry_file);
        std::cerr << "DEBUG: Registry created\n";
        
        std::cerr << "DEBUG: About to call listJobNames\n";
        auto jobs = registry.listJobNames(group_name);
        std::cerr << "DEBUG: Got job names, count: " << jobs.size() << "\n";
        
        for (const auto& job : jobs) {
            std::cerr << "DEBUG: Found job: '" << job << "'\n";
        }
        
        std::cerr << "DEBUG: Test completed successfully\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
