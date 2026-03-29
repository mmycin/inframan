#include "status-processor.hpp"
#include "group-config.hpp"
#include "rang.hpp"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <locale>

namespace commands {

// Helper functions
std::string StatusProcessor::trim(const std::string& str) {
    if (str.empty()) return "";
    
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> StatusProcessor::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

std::string StatusProcessor::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool StatusProcessor::containsIgnoreCase(const std::string& str, const std::string& substr) {
    return toLower(str).find(toLower(substr)) != std::string::npos;
}

// Advanced parsing functions
std::vector<std::string> StatusProcessor::parseServiceNames(const std::string& output) {
    std::vector<std::string> services;
    std::istringstream iss(output);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = trim(line);
        if (!line.empty() && line.find("NAME") == std::string::npos) {
            // Handle both docker-compose output and podman-compose ps output
            // Skip separator lines, headers, and empty lines
            if (line.find("---") == std::string::npos && 
                line.find("NAME") == std::string::npos &&
                line.find("STATUS") == std::string::npos &&
                line.find("CONTAINER") == std::string::npos &&
                line.find("IMAGE") == std::string::npos &&
                line.find("COMMAND") == std::string::npos &&
                line.find("PORTS") == std::string::npos) {
                
                // Extract service name - first word for podman, first column for docker
                std::istringstream line_stream(line);
                std::string first_word;
                if (line_stream >> first_word) {
                    // Skip container IDs (typically long hex strings)
                    if (first_word.length() < 20) { // Service names are usually shorter than container IDs
                        services.push_back(first_word);
                    }
                }
            }
        }
    }
    
    return services;
}

std::string StatusProcessor::extractContainerId(const std::string& output) {
    std::istringstream iss(output);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = trim(line);
        if (!line.empty()) {
            // Simple parsing: first word is container ID/name
            std::istringstream line_stream(line);
            std::string first_word;
            if (line_stream >> first_word) {
                return first_word;
            }
        }
    }
    
    return "";
}

std::string StatusProcessor::extractServiceStatus(const std::string& output, const std::string& service_name) {
    std::istringstream iss(output);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = trim(line);
        if (containsIgnoreCase(line, service_name)) {
            // Check for running indicators
            if (containsIgnoreCase(line, "running") || containsIgnoreCase(line, "up")) {
                return "up";
            } else if (containsIgnoreCase(line, "exited") || containsIgnoreCase(line, "down")) {
                return "down";
            }
        }
    }
    
    return "down";
}

// Status validation helpers
bool StatusProcessor::isContainerRunning(const std::string& status) {
    std::string lower_status = toLower(status);
    return lower_status.find("up") != std::string::npos || 
           lower_status.find("running") != std::string::npos;
}

bool StatusProcessor::isServiceRunning(const std::string& status) {
    return isContainerRunning(status); // Same logic for services
}

std::string StatusProcessor::getHealthStatus(const std::string& output) {
    if (containsIgnoreCase(output, "healthy")) {
        return "healthy";
    } else if (containsIgnoreCase(output, "unhealthy")) {
        return "unhealthy";
    } else if (containsIgnoreCase(output, "starting")) {
        return "starting";
    }
    return "unknown";
}

// Main parsing functions
std::string StatusProcessor::parseContainerStatus(const std::string& output) {
    if (output.empty()) {
        return "down";
    }
    
    std::string trimmed = trim(output);
    
    // Simple string-based detection (avoid regex locale issues)
    if (containsIgnoreCase(trimmed, "up") || containsIgnoreCase(trimmed, "running")) {
        return "up";
    } else if (containsIgnoreCase(trimmed, "exited") || 
               containsIgnoreCase(trimmed, "created") || 
               containsIgnoreCase(trimmed, "paused") ||
               containsIgnoreCase(trimmed, "dead")) {
        return "down";
    }
    
    return "down";
}

std::string StatusProcessor::parseComposeStatus(const std::string& output) {
    if (output.empty()) {
        return "down";
    }
    
    std::string trimmed = trim(output);
    
    // Parse docker-compose ps output (both Docker and Podman)
    std::istringstream iss(trimmed);
    std::string line;
    bool has_running = false;
    bool has_up = false;
    
    while (std::getline(iss, line)) {
        line = trim(line);
        if (!line.empty() && 
            line.find("NAME") == std::string::npos && 
            line.find("---") == std::string::npos &&
            line.find("STATUS") == std::string::npos &&
            line.find("CONTAINER") == std::string::npos &&
            line.find("IMAGE") == std::string::npos &&
            line.find("COMMAND") == std::string::npos &&
            line.find("PORTS") == std::string::npos) {
            
            // Check for running indicators in the line
            if (containsIgnoreCase(line, "running") || 
                containsIgnoreCase(line, "up") ||
                containsIgnoreCase(line, "Up") ||
                line.find("Up") != std::string::npos) {
                has_running = true;
                has_up = true;
            }
            
            // Also check for "healthy" which indicates running
            if (containsIgnoreCase(line, "healthy")) {
                has_running = true;
                has_up = true;
            }
        }
    }
    
    return (has_running || has_up) ? "up" : "down";
}

std::string StatusProcessor::extractJobName(const std::string& output) {
    return extractContainerId(output);
}

std::vector<StatusInfo> StatusProcessor::extractStatusInfo(const std::string& output, const std::string& type) {
    std::vector<StatusInfo> status_list;
    std::istringstream iss(output);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = trim(line);
        if (!line.empty() && line.find("NAME") == std::string::npos) {
            StatusInfo info;
            
            // Extract name (first word)
            std::istringstream line_stream(line);
            line_stream >> info.name;
            
            // Extract status
            info.status = (containsIgnoreCase(line, "running") || containsIgnoreCase(line, "up")) ? "up" : "down";
            info.is_running = info.status == "up";
            
            info.type = type;
            status_list.push_back(info);
        }
    }
    
    return status_list;
}

std::string StatusProcessor::formatStatusMessage(const std::string& job_name, 
                                               const std::string& status, 
                                               const std::string& type) {
    bool is_up = status == "up";
    std::string status_icon = is_up ? "🟢" : "🔴";
    std::string status_text = is_up ? "running" : "stopped";
    
    std::ostringstream oss;
    if (is_up) {
        oss << status_icon << " " << rang::style::bold << job_name << rang::style::reset 
            << " (" << type << ") - " << rang::fg::green << status_text << rang::fg::reset;
    } else {
        oss << status_icon << " " << rang::style::bold << job_name << rang::style::reset 
            << " (" << type << ") - " << rang::fg::red << status_text << rang::fg::reset;
    }
    return oss.str();
}

std::string StatusProcessor::normalizeStatus(const std::string& status) {
    if (status.empty()) return "down";
    
    std::string normalized = toLower(status);
    
    if (normalized.find("up") != std::string::npos || 
        normalized.find("running") != std::string::npos) {
        return "up";
    } else if (normalized.find("down") != std::string::npos || 
               normalized.find("stopped") != std::string::npos ||
               normalized.find("exited") != std::string::npos) {
        return "down";
    }
    
    return "down";
}

bool StatusProcessor::isValidStatus(const std::string& status) {
    std::string normalized = toLower(status);
    return normalized == "up" || normalized == "down" || 
           normalized == "running" || normalized == "stopped";
}

bool StatusProcessor::shouldSkipForRun(const std::string& status) {
    return status == "up" || status == "running";
}

bool StatusProcessor::shouldSkipForStop(const std::string& status) {
    return status == "down" || status == "stopped";
}

} // namespace commands
