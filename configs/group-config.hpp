#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include <cstdio>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <cstdlib>
#endif

namespace GroupConfig {
    #ifdef _WIN32
        inline static const std::string group_path = "C:/ProgramData/inframan/groups/";
    #else
        inline static const std::string group_path = "/var/inframan/groups/";
    #endif

    inline static const std::string registry_file = group_path + ".infra_info.json";

    
    enum class Providers {
        DOCKER,
        PODMAN,
        CONTAINERD,
        UNKNOWN
    };
    
    enum class Type {
        DOCKERFILE,    // Dockerfile-based builds
        COMPOSE,       // Docker Compose projects
        SERVICE,       // Long-running services
        TASK,          // One-time tasks
        NETWORK,       // Network configurations
        VOLUME,        // Volume management
        CUSTOM,        // Custom configurations
        UNKNOWN
    };
    
    inline std::string providerToString(Providers provider) {
        switch (provider) {
            case Providers::DOCKER:     return "docker";
            case Providers::PODMAN:     return "podman";
            case Providers::CONTAINERD: return "containerd";
            default:                    return "unknown";
        }
    }
    
    inline std::string typeToString(Type type) {
        switch (type) {
            case Type::DOCKERFILE: return "dockerfile";
            case Type::COMPOSE:    return "compose";
            case Type::SERVICE:    return "service";
            case Type::TASK:       return "task";
            case Type::NETWORK:    return "network";
            case Type::VOLUME:     return "volume";
            case Type::CUSTOM:     return "custom";
            default:               return "unknown";
        }
    }
    
    inline Providers providerFromString(const std::string& str) {
        static const std::unordered_map<std::string, Providers> map = {
            {"docker", Providers::DOCKER},
            {"podman", Providers::PODMAN},
            {"containerd", Providers::CONTAINERD}
        };
        auto it = map.find(str);
        return it != map.end() ? it->second : Providers::UNKNOWN;
    }
    
    inline Type typeFromString(const std::string& str) {
        static const std::unordered_map<std::string, Type> map = {
            {"dockerfile", Type::DOCKERFILE},
            {"compose", Type::COMPOSE},
            {"service", Type::SERVICE},
            {"task", Type::TASK},
            {"network", Type::NETWORK},
            {"volume", Type::VOLUME},
            {"custom", Type::CUSTOM}
        };
        auto it = map.find(str);
        return it != map.end() ? it->second : Type::UNKNOWN;
    }

    // Auto-detection functions for provider and type-based commands
    inline std::string autoDetectCommand(Providers provider, Type type, const std::string& job_name = "") {
        std::string provider_cmd = providerToString(provider);
        
        switch (type) {
            case Type::COMPOSE:
                return provider_cmd + "-compose up -d";
            case Type::DOCKERFILE:
                return provider_cmd + " build -t " + (job_name.empty() ? "app" : job_name) + " . && " + provider_cmd + " run -d " + (job_name.empty() ? "app" : job_name);
            case Type::SERVICE:
                return provider_cmd + " service start " + (job_name.empty() ? "service" : job_name);
            case Type::TASK:
                return provider_cmd + " run --rm " + (job_name.empty() ? "task" : job_name);
            case Type::NETWORK:
                return provider_cmd + " network create " + (job_name.empty() ? "network" : job_name);
            case Type::VOLUME:
                return provider_cmd + " volume create " + (job_name.empty() ? "volume" : job_name);
            case Type::CUSTOM:
            default:
                return "";
        }
    }

    inline std::string autoDetectFilePath(Type type, const std::string& job_name = "") {
        switch (type) {
            case Type::COMPOSE:
                return "docker-compose.yml";
            case Type::DOCKERFILE:
                return "Dockerfile";
            case Type::SERVICE:
            case Type::TASK:
            case Type::NETWORK:
            case Type::VOLUME:
            case Type::CUSTOM:
            default:
                return "";
        }
    }

    inline bool requiresFilePath(Type type) {
        return type == Type::COMPOSE || type == Type::DOCKERFILE;
    }

    inline bool requiresCommand(Type type) {
        return type == Type::CUSTOM;
    }

    // Auto-detection functions for stop commands
    inline std::string autoDetectStopCommand(Providers provider, Type type, const std::string& job_name = "") {
        std::string provider_cmd = providerToString(provider);
        
        switch (type) {
            case Type::COMPOSE:
                return provider_cmd + "-compose down";
            case Type::DOCKERFILE:
                return provider_cmd + " stop " + (job_name.empty() ? "app" : job_name) + " && " + provider_cmd + " rm " + (job_name.empty() ? "app" : job_name);
            case Type::SERVICE:
                return provider_cmd + " service stop " + (job_name.empty() ? "service" : job_name);
            case Type::TASK:
                return provider_cmd + " stop " + (job_name.empty() ? "task" : job_name);
            case Type::NETWORK:
                return provider_cmd + " network rm " + (job_name.empty() ? "network" : job_name);
            case Type::VOLUME:
                return provider_cmd + " volume rm " + (job_name.empty() ? "volume" : job_name);
            case Type::CUSTOM:
            default:
                return "";
        }
    }

    // Forward declaration for StatusProcessor (implemented in commands namespace)
    class StatusProcessor;
    
    // Container status checking functions using Lua
    inline std::string checkContainerStatus(Providers provider, const std::string& job_name) {
        // This will be replaced with Lua-based processing
        // For now, keep the original implementation as fallback
        std::string provider_cmd = providerToString(provider);
        std::string command = provider_cmd + " ps -a --filter \"name=" + job_name + "\" --format \"{{.Status}}\"";
        
        // Execute command and get output
        std::string result = "";
#ifdef _WIN32
        FILE* pipe = _popen(command.c_str(), "r");
        if (pipe) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            _pclose(pipe);
        }
#else
        FILE* pipe = popen(command.c_str(), "r");
        if (pipe) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            pclose(pipe);
        }
#endif
        
        // Trim whitespace and newlines
        result.erase(0, result.find_first_not_of(" \t\n\r"));
        result.erase(result.find_last_not_of(" \t\n\r") + 1);
        
        return result.empty() ? "down" : result;
    }

    inline std::string checkComposeStatus(Providers provider, const std::string& file_path) {
        // This will be replaced with Lua-based processing
        // For now, keep the original implementation as fallback
        std::string provider_cmd = providerToString(provider);
        std::filesystem::path original_dir;
        bool dir_changed = false;
        
        // Change to compose file directory if it exists
        if (!file_path.empty()) {
            try {
                std::filesystem::path target_dir = std::filesystem::path(file_path).parent_path();
                if (!target_dir.empty() && std::filesystem::exists(target_dir)) {
                    original_dir = std::filesystem::current_path();
                    std::filesystem::current_path(target_dir);
                    dir_changed = true;
                }
            } catch (...) {}
        }
        
        std::string command = provider_cmd + "-compose ps --services --format \"table {{.Name}}\t{{.Status}}\"";
        
        // Execute command and get output
        std::string result = "";
#ifdef _WIN32
        FILE* pipe = _popen(command.c_str(), "r");
        if (pipe) {
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            _pclose(pipe);
        }
#else
        FILE* pipe = popen(command.c_str(), "r");
        if (pipe) {
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            pclose(pipe);
        }
#endif
        
        // Restore original directory
        if (dir_changed) {
            try {
                std::filesystem::current_path(original_dir);
            } catch (...) {}
        }
        
        // Parse output to find running services
        if (result.find("running") != std::string::npos) {
            return "up";
        }
        
        return "down";
    }

    inline std::string getActualJobStatus(Providers provider, Type type, const std::string& job_name, const std::string& file_path = "") {
        switch (type) {
            case Type::COMPOSE:
                return checkComposeStatus(provider, file_path);
            case Type::DOCKERFILE:
            case Type::SERVICE:
            case Type::TASK:
                return checkContainerStatus(provider, job_name);
            case Type::NETWORK:
            case Type::VOLUME:
            case Type::CUSTOM:
            default:
                return "down"; // Networks and volumes don't have "running" status in the same way
        }
    }
}