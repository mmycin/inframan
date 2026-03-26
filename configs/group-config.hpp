#pragma once
#include <string>
#include <unordered_map>

namespace GroupConfig {
    inline static const std::string group_path = "";
    
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
}