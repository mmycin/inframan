#include "group-config.hpp"
#include "run-group.hpp"
#include "group-registry.hpp"
#include "status-processor.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"
#include "libraries/rang.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdexcept>

namespace commands {

// Global status processor instance
static StatusProcessor statusProcessor;

// ─── Public ───────────────────────────────────────────────────────────────────

void RunGroup::execute() {
    RunGroup{}.run();
}

void RunGroup::run() {
    try {
        tabulate::Table header;
        header.add_row({"RUN ALL JOBS IN GROUP"});
        header[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::green)
            .border_top("=")
            .border_bottom("=")
            .border_left("")
            .border_right("")
            .corner("");
        header.column(0).format().width(54);
        std::cout << "\n" << rang::fg::green << header << rang::fg::reset << "\n";

        // Prefer active group context, otherwise prompt
        group_name = ContextManager::getActiveGroup();
        if (group_name.empty()) {
            selectGroup();
        } else {
            std::cout << "Using active group: " << rang::fg::yellow << group_name << rang::fg::reset << "\n";
        }

        runAllJobs();

    } catch (const std::exception& e) {
        std::cerr << "\n" << rang::fg::red << "Error: " << e.what() << rang::fg::reset << "\n";
    }
}

// ─── Group selection ──────────────────────────────────────────────────────────

void RunGroup::selectGroup() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto names = registry.listGroupNames();

    if (names.empty())
        throw std::runtime_error("No infrastructure groups found.");

    std::cout << "\nAvailable Groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << names[i] << "\n";

    std::cout << "\nSelect group (number): " << rang::style::bold;
    size_t choice;
    std::cin >> choice;
    std::cout << rang::style::reset;

    if (choice < 1 || choice > names.size())
        throw std::runtime_error("Invalid selection.");

    group_name = names[choice - 1];
}

// ─── Run all jobs sequentially ────────────────────────────────────────────────

void RunGroup::runAllJobs() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto jobs = registry.getJobs(group_name);

    if (jobs.empty())
        throw std::runtime_error("No jobs found in group '" + group_name + "'.");

    // Get group provider and type for auto-detection
    auto groups = registry.getGroups();
    auto group_data = groups[group_name];
    std::string provider_str = group_data.value("provider", "docker");
    std::string type_str = group_data.value("type", "compose");
    
    auto provider = GroupConfig::providerFromString(provider_str);
    auto type = GroupConfig::typeFromString(type_str);

    // Summary table
    tabulate::Table summary;
    summary.add_row({"GROUP", "JOBS TO RUN"});
    summary.add_row({group_name, std::to_string(jobs.size())});
    summary[0].format()
        .font_style({tabulate::FontStyle::bold})
        .font_color(tabulate::Color::cyan);
    std::cout << "\n" << summary << "\n";

    int passed = 0, failed = 0;

    for (auto it = jobs.begin(); it != jobs.end(); ++it) {
        const std::string& job_name  = it.key();
        const auto&        job_data  = it.value();
        
        // Get raw status output from container command
        std::string raw_status;
        if (type == GroupConfig::Type::COMPOSE) {
            raw_status = GroupConfig::checkComposeStatus(provider, job_data.value("file_path", ""));
        } else {
            raw_status = GroupConfig::checkContainerStatus(provider, job_name);
        }
        
        // Parse status using StatusProcessor
        std::string actual_status;
        if (type == GroupConfig::Type::COMPOSE) {
            actual_status = statusProcessor.parseComposeStatus(raw_status);
        } else {
            actual_status = statusProcessor.parseContainerStatus(raw_status);
        }

        // Skip jobs that are already "up"
        if (statusProcessor.shouldSkipForRun(actual_status)) {
            std::string formatted_msg = statusProcessor.formatStatusMessage(job_name, actual_status, GroupConfig::typeToString(type));
            std::cout << "\n" << rang::fg::yellow << "[SKIP] " << rang::fg::reset << formatted_msg << " - already running\n";
            continue;
        }

        // Use auto-detected command if it's not a custom type
        std::string command;
        if (type != GroupConfig::Type::CUSTOM) {
            command = GroupConfig::autoDetectCommand(provider, type, job_name);
        } else {
            command = job_data.value("command", "");
        }
        
        std::string file_path = job_data.value("file_path", "");

        if (command.empty()) {
            std::cerr << "\n" << rang::fg::yellow << "[SKIP] " << rang::fg::reset << "Job '" << job_name << "' has no command — " << rang::fg::yellow << "skipping" << rang::fg::reset << ".\n";
            ++failed;
            continue;
        }

        // Per-job info banner
        tabulate::Table job_info;
        job_info.add_row({"JOB", job_name});
        job_info.add_row({"COMMAND", command});
        if (!file_path.empty())
            job_info.add_row({"PATH", file_path});
        job_info[0][0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_color(tabulate::Color::yellow);
        std::cout << "\n" << job_info << "\n";

        int status = runSingleJob(job_name, command, file_path);
        if (status == 0) {
            ++passed;
            // Update job status to "up" on successful run
            registry.updateJobStatus(group_name, job_name, "up");
        } else {
            ++failed;
        }
    }

    // Final report
    tabulate::Table report;
    report.add_row({"RUN COMPLETE", ""});
    report.add_row({"Group",   group_name});
    report.add_row({"Passed",  std::to_string(passed)});
    report.add_row({"Failed",  std::to_string(failed)});
    report.add_row({"Total",   std::to_string(passed + failed)});

    report[0].format()
        .font_style({tabulate::FontStyle::bold})
        .font_align(tabulate::FontAlign::center)
        .font_color(failed == 0 ? tabulate::Color::green : tabulate::Color::red)
        .border_top("=")
        .border_bottom("=")
        .border_left("")
        .border_right("")
        .corner("");

    report[2][1].format().font_color(tabulate::Color::green)
                         .font_style({tabulate::FontStyle::bold});
    if (failed > 0)
        report[3][1].format().font_color(tabulate::Color::red)
                             .font_style({tabulate::FontStyle::bold});

    report.column(0).format().font_style({tabulate::FontStyle::bold}).width(12);
    report.column(1).format().width(30);

    std::cout << "\n" << report << "\n";
}

// ─── Single job runner ────────────────────────────────────────────────────────

#include <filesystem>

int RunGroup::runSingleJob(const std::string& job_name,
                           const std::string& command,
                           const std::string& file_path) {
    std::cout << rang::fg::gray << "--- [" << job_name << "] Output Starts ---" << rang::fg::reset << "\n";

    std::filesystem::path original_dir;
    bool dir_changed = false;

    if (!file_path.empty()) {
        try {
            std::filesystem::path target_dir = std::filesystem::path(file_path).parent_path();
            if (!target_dir.empty() && std::filesystem::exists(target_dir)) {
                original_dir = std::filesystem::current_path();
                std::filesystem::current_path(target_dir);
                dir_changed = true;
            }
        } catch (const std::exception& e) {
            std::cerr << rang::fg::yellow << "Warning: Could not change directory (" << e.what() << ")" << rang::fg::reset << "\n";
        }
    }

    int status = std::system(command.c_str());

    if (dir_changed) {
        try {
            std::filesystem::current_path(original_dir);
        } catch (...) {}
    }

    std::cout << rang::fg::gray << "--- [" << job_name << "] Output Ends   ---" << rang::fg::reset << "\n";

    if (status != 0)
        std::cerr << rang::fg::red << "[FAIL] '" << job_name << "' exited with status " << status << rang::fg::reset << "\n";
    else
        std::cout << rang::fg::green << "[OK]   " << rang::style::bold << "'" << job_name << "' completed successfully." << rang::style::reset << "\n";

    return status;
}

} // namespace commands
