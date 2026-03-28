#pragma once
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>

namespace commands {

struct StatusInfo {
    std::string name;
    std::string status;
    std::string type;
    std::string command;
    std::string file_path;
    bool is_running = false;
};

class StatusProcessor {
public:
    StatusProcessor() = default;
    ~StatusProcessor() = default;

    // Initialize (no-op for basic implementation)
    bool initialize() { return true; }

    // Parse container status using advanced C++ string processing
    std::string parseContainerStatus(const std::string& output);
    
    // Parse compose status using advanced C++ string processing
    std::string parseComposeStatus(const std::string& output);
    
    // Extract job name from output
    std::string extractJobName(const std::string& output);
    
    // Extract multiple status information from output
    std::vector<StatusInfo> extractStatusInfo(const std::string& output, const std::string& type);
    
    // Format status message for display
    std::string formatStatusMessage(const std::string& job_name, 
                                   const std::string& status, 
                                   const std::string& type);
    
    // Validate and normalize status
    std::string normalizeStatus(const std::string& status);
    bool isValidStatus(const std::string& status);
    
    // Check if should skip operations
    bool shouldSkipForRun(const std::string& status);
    bool shouldSkipForStop(const std::string& status);
    
    // Advanced parsing functions
    std::vector<std::string> parseServiceNames(const std::string& output);
    std::string extractContainerId(const std::string& output);
    std::string extractServiceStatus(const std::string& output, const std::string& service_name);
    
    // Status validation helpers
    bool isContainerRunning(const std::string& status);
    bool isServiceRunning(const std::string& status);
    std::string getHealthStatus(const std::string& output);

private:
    // Helper functions
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string toLower(const std::string& str);
    bool containsIgnoreCase(const std::string& str, const std::string& substr);
};

} // namespace commands
