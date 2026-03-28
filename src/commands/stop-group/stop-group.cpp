#include "group-config.hpp"
#include "stop-group.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdexcept>

namespace commands {

// ─── Public ───────────────────────────────────────────────────────────────────

void StopGroup::execute() {
    StopGroup{}.run();
}

void StopGroup::run() {
    try {
        tabulate::Table header;
        header.add_row({"STOP ALL JOBS IN GROUP"});
        header[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::red)
            .border_top("=")
            .border_bottom("=")
            .border_left("")
            .border_right("")
            .corner("");
        header.column(0).format().width(54);
        std::cout << "\n" << header << "\n";

        // Prefer active group context, otherwise prompt
        group_name = ContextManager::getActiveGroup();
        if (group_name.empty()) {
            selectGroup();
        } else {
            std::cout << "Using active group: " << group_name << "\n";
        }

        stopAllJobs();

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
    }
}

// ─── Group selection ──────────────────────────────────────────────────────────

void StopGroup::selectGroup() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto names = registry.listGroupNames();

    if (names.empty())
        throw std::runtime_error("No infrastructure groups found.");

    std::cout << "\nAvailable Groups:\n";
    for (size_t i = 0; i < names.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << names[i] << "\n";

    std::cout << "\nSelect group (number): ";
    size_t choice;
    std::cin >> choice;

    if (choice < 1 || choice > names.size())
        throw std::runtime_error("Invalid selection.");

    group_name = names[choice - 1];
}

// ─── Stop all jobs sequentially ────────────────────────────────────────────────

void StopGroup::stopAllJobs() {
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
    summary.add_row({"GROUP", "JOBS TO STOP"});
    summary.add_row({group_name, std::to_string(jobs.size())});
    summary[0].format()
        .font_style({tabulate::FontStyle::bold})
        .font_color(tabulate::Color::cyan);
    std::cout << "\n" << summary << "\n";

    int passed = 0, failed = 0;

    for (auto it = jobs.begin(); it != jobs.end(); ++it) {
        const std::string& job_name  = it.key();
        const auto&        job_data  = it.value();
        std::string job_status = job_data.value("status", "down");

        // Skip jobs that are already "down"
        if (job_status == "down") {
            std::cout << "\n[SKIP] Job '" << job_name << "' is already stopped (status: down).\n";
            continue;
        }

        // Use auto-detected stop command if it's not a custom type
        std::string command;
        if (type != GroupConfig::Type::CUSTOM) {
            command = GroupConfig::autoDetectStopCommand(provider, type, job_name);
        } else {
            // For custom types, we can't auto-detect stop commands
            std::cerr << "\n[SKIP] Job '" << job_name << "' is CUSTOM type - stop command not auto-detected.\n";
            ++failed;
            continue;
        }
        
        std::string file_path = job_data.value("file_path", "");

        if (command.empty()) {
            std::cerr << "\n[SKIP] Job '" << job_name << "' has no stop command — skipping.\n";
            ++failed;
            continue;
        }

        // Per-job info banner
        tabulate::Table job_info;
        job_info.add_row({"JOB", job_name});
        job_info.add_row({"STOP COMMAND", command});
        if (!file_path.empty())
            job_info.add_row({"PATH", file_path});
        job_info[0][0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_color(tabulate::Color::yellow);
        std::cout << "\n" << job_info << "\n";

        int status = stopSingleJob(job_name, command, file_path);
        if (status == 0) {
            ++passed;
            // Update job status to "down" on successful stop
            GroupRegistry registry(GroupConfig::registry_file);
            registry.updateJobStatus(group_name, job_name, "down");
        } else {
            ++failed;
        }
    }

    // Final report
    tabulate::Table report;
    report.add_row({"STOP COMPLETE", ""});
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

// ─── Single job stopper ────────────────────────────────────────────────────────

#include <filesystem>

int StopGroup::stopSingleJob(const std::string& job_name,
                             const std::string& command,
                             const std::string& file_path) {
    std::cout << "--- [" << job_name << "] Stop Output Starts ---\n";

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
            std::cerr << "Warning: Could not change directory (" << e.what() << ")\n";
        }
    }

    int status = std::system(command.c_str());

    if (dir_changed) {
        try {
            std::filesystem::current_path(original_dir);
        } catch (...) {}
    }

    std::cout << "--- [" << job_name << "] Stop Output Ends   ---\n";

    if (status != 0)
        std::cerr << "[FAIL] '" << job_name << "' stop exited with status " << status << "\n";
    else
        std::cout << "[OK]   '" << job_name << "' stopped successfully.\n";

    return status;
}

} // namespace commands
