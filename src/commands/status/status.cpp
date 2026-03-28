#include "group-config.hpp"
#include "status.hpp"
#include "group-registry.hpp"
#include "status-processor.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace commands {

// Global status processor instance
static StatusProcessor statusProcessor;

// ─── Public ───────────────────────────────────────────────────────────────────

void Status::execute() {
    Status{}.run();
}

void Status::run() {
    try {
        std::cout << "\n=== JOB STATUS OVERVIEW ===\n\n";

        // Prefer active group context, otherwise prompt
        group_name = ContextManager::getActiveGroup();
        if (group_name.empty()) {
            selectGroup();
        } else {
            std::cout << "Using active group: " << group_name << "\n";
        }

        showAllJobsStatus();

    } catch (const std::exception& e) {
        std::cerr << "Error showing status: " << e.what() << "\n";
    }
}

// ─── Group selection ──────────────────────────────────────────────────────────

void Status::selectGroup() {
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

// ─── Status display ───────────────────────────────────────────────────────────

void Status::showAllJobsStatus() {
    GroupRegistry registry(GroupConfig::registry_file);
    auto jobs = registry.getJobs(group_name);

    if (jobs.empty()) {
        std::cout << "\nNo jobs found in group '" << group_name << "'.\n";
        return;
    }

    // Get group provider and type
    auto groups = registry.getGroups();
    auto group_data = groups[group_name];
    std::string provider_str = group_data.value("provider", "docker");
    std::string type_str = group_data.value("type", "compose");
    
    auto provider = GroupConfig::providerFromString(provider_str);
    auto type = GroupConfig::typeFromString(type_str);

    // Create status table
    tabulate::Table status_table;
    status_table.add_row({"JOB", "TYPE", "STATUS", "COMMAND", "FILE PATH"});
    
    // Format header row
    status_table[0].format()
        .font_style({tabulate::FontStyle::bold})
        .font_color(tabulate::Color::cyan);

    int running_count = 0;
    int stopped_count = 0;
    int total_jobs = 0;

    for (auto it = jobs.begin(); it != jobs.end(); ++it) {
        const std::string& job_name = it.key();
        const auto& job_data = it.value();
        
        std::string file_path = job_data.value("file_path", "");
        std::string command = job_data.value("command", "");
        
        // Get actual status
        std::string actual_status = getJobStatus(job_name, provider, type, file_path);
        
        // Count status
        if (actual_status == "up") {
            running_count++;
        } else {
            stopped_count++;
        }
        total_jobs++;

        // Format status with icon
        std::string status_icon = actual_status == "up" ? "🟢" : "🔴";
        std::string status_text = actual_status == "up" ? "Running" : "Stopped";
        std::string formatted_status = status_icon + " " + status_text;

        // Add row to table
        status_table.add_row({
            job_name,
            GroupConfig::typeToString(type),
            formatted_status,
            command.empty() ? "N/A" : command,
            file_path.empty() ? "N/A" : file_path
        });
        
        // Color code the status cell
        auto& status_cell = status_table[total_jobs][2];
        if (actual_status == "up") {
            status_cell.format().font_color(tabulate::Color::green);
        } else {
            status_cell.format().font_color(tabulate::Color::red);
        }
    }

    // Format table
    status_table.column(0).format().width(15);  // JOB
    status_table.column(1).format().width(10);  // TYPE
    status_table.column(2).format().width(12);  // STATUS
    status_table.column(3).format().width(20);  // COMMAND
    status_table.column(4).format().width(25);  // FILE PATH

    std::cout << "\n" << status_table << "\n";

    // Summary statistics
    tabulate::Table summary;
    summary.add_row({"SUMMARY", ""});
    summary.add_row({"Group", group_name});
    summary.add_row({"Total Jobs", std::to_string(total_jobs)});
    summary.add_row({"Running", std::to_string(running_count)});
    summary.add_row({"Stopped", std::to_string(stopped_count)});
    
    summary[0].format()
        .font_style({tabulate::FontStyle::bold})
        .font_align(tabulate::FontAlign::center)
        .font_color(tabulate::Color::blue)
        .border_top("=")
        .border_bottom("=")
        .border_left("")
        .border_right("")
        .corner("");
    
    summary[3][1].format().font_color(tabulate::Color::green)
                         .font_style({tabulate::FontStyle::bold});
    summary[4][1].format().font_color(tabulate::Color::red)
                         .font_style({tabulate::FontStyle::bold});

    summary.column(0).format().font_style({tabulate::FontStyle::bold}).width(12);
    summary.column(1).format().width(20);

    std::cout << "\n" << summary << "\n";
}

std::string Status::getJobStatus(const std::string& job_name, 
                                 GroupConfig::Providers provider, 
                                 GroupConfig::Type type, 
                                 const std::string& file_path) {
    // Get raw status output from container command
    std::string raw_status;
    if (type == GroupConfig::Type::COMPOSE) {
        raw_status = GroupConfig::checkComposeStatus(provider, file_path);
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
    
    return actual_status;
}

} // namespace commands
