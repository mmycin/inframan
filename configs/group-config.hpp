#pragma once
#include <string>
#include <unordered_map>

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
}