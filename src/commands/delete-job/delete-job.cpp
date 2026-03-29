#include "group-config.hpp"
#include "delete-job.hpp"
#include "group-registry.hpp"
#include "libraries/tabulate.hpp"
#include "context-manager.hpp"
#include "rang.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#ifdef _WIN32
#undef DeleteJob
#endif

namespace commands {

static const std::string REGISTRY = GroupConfig::registry_file;

void DeleteJob::execute() {
    DeleteJob{}.run();
}

void DeleteJob::run() {
    try {
        tabulate::Table header;
        header.add_row({"DELETE INFRASTRUCTURE JOB"});
        header[0].format()
            .font_style({tabulate::FontStyle::bold})
            .font_align(tabulate::FontAlign::center)
            .font_color(tabulate::Color::red)
            .border_top("=")
            .border_bottom("=")
            .border_left("")
            .border_right("")
            .corner("");
        header.column(0).format().width(50);
        std::cout << "\n" << rang::fg::red << header << rang::fg::reset << "\n";

        // Use active group context if available
        group_name = ContextManager::getActiveGroup();
        if (group_name.empty()) {
            selectGroup();
        } else {
            std::cout << "Using active group: " << rang::fg::yellow << group_name << rang::fg::reset << "\n";
        }

        selectJob();
        confirmAndDelete();

    } catch (const std::exception& e) {
        std::cerr << "\n" << rang::fg::red << "Error: " << e.what() << rang::fg::reset << "\n";
    }
}

void DeleteJob::selectGroup() {
    GroupRegistry registry(REGISTRY);
    auto groups = registry.listGroupNames();

    if (groups.empty())
        throw std::runtime_error("No infrastructure groups found.");

    std::cout << "\nAvailable Groups:\n";
    for (size_t i = 0; i < groups.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << groups[i] << "\n";
    }

    std::cout << "\nSelect group (number): " << rang::style::bold;
    size_t choice;
    std::cin >> choice;
    std::cout << rang::style::reset;

    if (choice < 1 || choice > groups.size())
        throw std::runtime_error("Invalid selection.");

    group_name = groups[choice - 1];
}

void DeleteJob::selectJob() {
    GroupRegistry registry(REGISTRY);
    auto jobs = registry.listJobNames(group_name);

    if (jobs.empty())
        throw std::runtime_error("No jobs found in group '" + group_name + "'.");

    std::cout << "\nJobs in group '" << group_name << "':\n";
    for (size_t i = 0; i < jobs.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << jobs[i] << "\n";
    }

    std::cout << "\nSelect job to delete (number): " << rang::style::bold;
    size_t choice;
    std::cin >> choice;
    std::cout << rang::style::reset;

    if (choice < 1 || choice > jobs.size())
        throw std::runtime_error("Invalid selection.");

    job_name = jobs[choice - 1];
}

void DeleteJob::confirmAndDelete() {
    std::cout << "\n" << rang::fg::yellow << "Are you sure you want to delete job '" << job_name << "' from group '" << group_name << "'?" << rang::fg::reset << " (y/n): " << rang::style::bold;
    char confirm;
    std::cin >> confirm;
    std::cout << rang::style::reset;

    if (confirm == 'y' || confirm == 'Y') {
        GroupRegistry registry(REGISTRY);
        registry.deleteJob(group_name, job_name);

        tabulate::Table success;
        success.add_row({"SUCCESS", "Job '" + job_name + "' deleted."});
        success[0][0].format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold});
        std::cout << "\n" << rang::fg::green << success << rang::fg::reset << "\n";
    } else {
        std::cout << "\n" << rang::fg::yellow << "Deletion cancelled." << rang::fg::reset << "\n";
    }
}

} // namespace commands
