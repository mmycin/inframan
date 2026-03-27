#include "group-config.hpp"
#include "run-group.hpp"
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
        std::cout << "\n" << header << "\n";

        // Prefer active group context, otherwise prompt
        group_name = ContextManager::getActiveGroup();
        if (group_name.empty()) {
            selectGroup();
        } else {
            std::cout << "Using active group: " << group_name << "\n";
        }

        runAllJobs();

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
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

    std::cout << "\nSelect group (number): ";
    size_t choice;
    std::cin >> choice;

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

        std::string command   = job_data.value("command",   "");
        std::string file_path = job_data.value("file_path", "");

        if (command.empty()) {
            std::cerr << "\n[SKIP] Job '" << job_name << "' has no command — skipping.\n";
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
        if (status == 0)
            ++passed;
        else
            ++failed;
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

int RunGroup::runSingleJob(const std::string& job_name,
                           const std::string& command,
                           const std::string& /*file_path*/) {
    std::cout << "--- [" << job_name << "] Output Starts ---\n";
    int status = std::system(command.c_str());
    std::cout << "--- [" << job_name << "] Output Ends   ---\n";

    if (status != 0)
        std::cerr << "[FAIL] '" << job_name << "' exited with status " << status << "\n";
    else
        std::cout << "[OK]   '" << job_name << "' completed successfully.\n";

    return status;
}

} // namespace commands
