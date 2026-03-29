// correct_completer.hpp
#pragma once
#include "linenoise.hpp"
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

class PathCompleter {
private:
    // Find longest common prefix of all strings in vector
    static std::string longest_common_prefix(const std::vector<std::string>& strings) {
        if (strings.empty()) return "";
        
        std::string prefix = strings[0];
        for (size_t i = 1; i < strings.size(); i++) {
            size_t j = 0;
            while (j < prefix.length() && j < strings[i].length() && prefix[j] == strings[i][j]) {
                j++;
            }
            prefix = prefix.substr(0, j);
            if (prefix.empty()) break;
        }
        return prefix;
    }
    
    static bool is_directory(const std::string& path) {
        try {
            return fs::is_directory(path);
        } catch (...) {
            return false;
        }
    }
    
public:
    static void setup() {
        linenoise::SetCompletionCallback([](const char* editbuf, std::vector<std::string>& completions) {
            std::string input(editbuf);
            
            // Get the last part of the path (after last slash)
            size_t last_slash = input.find_last_of("/\\");
            std::string dir = (last_slash != std::string::npos) ? input.substr(0, last_slash + 1) : "";
            std::string fragment = (last_slash != std::string::npos) ? input.substr(last_slash + 1) : input;
            
            // Handle Windows drive (C:)
            if (dir.empty() && input.length() >= 2 && input[1] == ':') {
                dir = input.substr(0, 2) + "\\";
                fragment = "";
            }
            
            // Default to current directory
            if (dir.empty()) dir = ".\\";
            
            std::vector<std::string> matches;
            std::vector<bool> is_dir_match;
            
            try {
                for (const auto& entry : fs::directory_iterator(dir)) {
                    std::string name = entry.path().filename().string();
                    
                    // Skip . and ..
                    if (name == "." || name == "..") continue;
                    
                    // Case-insensitive matching
                    std::string name_lower = name;
                    std::string frag_lower = fragment;
                    std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
                    std::transform(frag_lower.begin(), frag_lower.end(), frag_lower.begin(), ::tolower);
                    
                    if (name_lower.find(frag_lower) == 0) {
                        matches.push_back(name);
                        is_dir_match.push_back(entry.is_directory());
                    }
                }
            } catch (...) {}
            
            if (matches.empty()) {
                return; // No matches
            }
            
            // Sort matches
            std::sort(matches.begin(), matches.end());
            
            // Find longest common prefix
            std::string common = longest_common_prefix(matches);
            
            if (common.length() > fragment.length()) {
                // There's a common prefix longer than what user typed
                std::string completion = dir + common;
                
                // Check if this common prefix is a complete directory name
                // (meaning all matches share this exact directory as full name)
                bool all_are_dirs = true;
                bool exact_match_exists = false;
                
                for (size_t i = 0; i < matches.size(); i++) {
                    if (matches[i] == common) {
                        exact_match_exists = true;
                        if (!is_dir_match[i]) all_are_dirs = false;
                    }
                    if (matches[i].length() > common.length()) {
                        all_are_dirs = false;
                    }
                }
                
                // If we have an exact match and it's a directory, add slash
                if (exact_match_exists && all_are_dirs) {
                    completion += "/";
                }
                // If common prefix is actually a directory in the filesystem
                else if (is_directory(completion)) {
                    completion += "/";
                }
                
                completions.push_back(completion);
            } 
            else if (matches.size() == 1) {
                // Only one match - add trailing slash if it's a directory
                std::string completion = dir + matches[0];
                if (is_dir_match[0]) {
                    completion += "/";
                }
                completions.push_back(completion);
            } 
            else {
                // Multiple matches with no common prefix beyond what user typed
                // Show all matches as completions (will display as list)
                for (size_t i = 0; i < matches.size(); i++) {
                    std::string completion = dir + matches[i];
                    if (is_dir_match[i]) {
                        completion += "/";
                    }
                    completions.push_back(completion);
                }
            }
        });
    }
};