#include "group-config.hpp"
#include "add-job.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"
#include "libraries/rang.hpp"
#include "libraries/linenoise.hpp"
#include "path-completer.hpp"

#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

#include <unistd.h>
#include <errno.h>
#include <cstring>

namespace commands {

void AddJob::execute() {
    AddJob{}.run();
}

void AddJob::run() {
    try {
        tabulate::Table header;
        header.add_row({"ADD NEW JOB"});
        header[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::green)
            .border_top("=")
            .border_bottom("=")
            .border_left("")
            .border_right("")
            .corner("");
        header.column(0).format().width(50);
        std::cout << "\n" << header << "\n";
        std::cout.flush();

        std::string group_name = ContextManager::getActiveGroup();
        if (group_name.empty()) {
            std::string prompt = "Enter infrastructure group name: ";
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            if (!linenoise::Readline(prompt.c_str(), group_name)) {
                throw std::runtime_error("Input cancelled (Group Name)");
            }
            if (group_name.empty()) throw std::runtime_error("Group name cannot be empty");
        } else {
            std::cout << "Using active group: " << rang::fg::yellow << group_name << rang::fg::reset << "\n";
            std::cout.flush();
        }

        std::string job_name, file_path, command;
        std::string name_prompt = "Enter job name: ";
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        
        // Diagnostic: Check TTY before linenoise
        bool is_tty = isatty(0);
        if (!is_tty) {
            std::cout << "[system] Warning: isatty(0) is FALSE\n";
            std::cout.flush();
        }

        if (!linenoise::Readline(name_prompt.c_str(), job_name)) {
            std::string err_msg = "Input cancelled (Job Name). System errno: " + std::string(strerror(errno));
            throw std::runtime_error(err_msg);
        }
        if (job_name.empty()) throw std::runtime_error("Job name cannot be empty");
        
        // Get group info to determine if file_path and command are required
        GroupRegistry registry(GroupConfig::registry_file);
        auto groups = registry.getGroups();
        if (!groups.contains(group_name)) {
            throw std::runtime_error("Group '" + group_name + "' not found.");
        }
        auto group_data = groups[group_name];
        std::string provider_str = group_data.value("provider", "docker");
        std::string type_str = group_data.value("type", "compose");
        
        auto provider = GroupConfig::providerFromString(provider_str);
        auto type = GroupConfig::typeFromString(type_str);
        
        // Only prompt for file_path if required by type
        if (GroupConfig::requiresFilePath(type)) {
            std::string prompt = "Enter file path [" + GroupConfig::autoDetectFilePath(type, job_name) + "]: ";
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            if (!linenoise::Readline(prompt.c_str(), file_path)) {
                throw std::runtime_error("Input cancelled (File Path)");
            }
            if (file_path.empty()) {
                file_path = GroupConfig::autoDetectFilePath(type, job_name);
            }
        } else {
            file_path = GroupConfig::autoDetectFilePath(type, job_name);
        }
        
        // Only prompt for command if it's a custom type
        if (GroupConfig::requiresCommand(type)) {
            std::string default_cmd = GroupConfig::autoDetectCommand(provider, type, job_name);
            std::string cmd_prompt = "Enter shell command [" + default_cmd + "]: ";
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            if (!linenoise::Readline(cmd_prompt.c_str(), command)) {
                throw std::runtime_error("Input cancelled (Command)");
            }
            if (command.empty()) command = default_cmd;
        } else {
            command = GroupConfig::autoDetectCommand(provider, type, job_name);
            std::cout << "Auto-detected command: " << rang::fg::cyan << command << rang::fg::reset << "\n";
            std::cout.flush();
        }

        registry.addOrUpdateJob(group_name, job_name, file_path, command, "down");

        tabulate::Table success;
        success.add_row({"SUCCESS", "Job added to group '" + group_name + "'"});
        success[0][0].format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold});
        std::cout << "\n" << rang::fg::green << success << rang::fg::reset << "\n";
        std::cout.flush();

    } catch (const std::exception& e) {
        std::cerr << "\n" << rang::fg::red << "FATAL ERROR: " << e.what() << rang::fg::reset << "\n";
        std::cerr << rang::fg::gray << "Please check if any [linenoise] markers appeared above." << rang::fg::reset << "\n";
    }
}

} // namespace commands
