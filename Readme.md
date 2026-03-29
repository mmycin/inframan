# InfraMan

**InfraMan** is a fast, cross-platform command-line tool written in C++ for managing container infrastructure groups directly from your terminal. It simplifies and automates the process of working with Docker, Podman, and Containerd configurations locally and regionally.

## Features
- **Multi-Provider Support**: Support for Docker, Podman, and Containerd runtimes natively.
- **Group Management**: Segment and group related infrastructure components—such as Dockerfiles, Compose setups, services, networks, and persistent volumes.
- **Auto-Detection**: Automatically generates appropriate commands based on provider and infrastructure type (COMPOSE, DOCKERFILE, SERVICE, TASK, NETWORK, VOLUME).
- **Concurrent Execution**: Run multiple infrastructure jobs simultaneously for improved performance.
- **Interactive UI**: Tab-completion for file paths and commands (powered by `linenoise`).
- **Enhanced Visuals**: Rich, modern terminal styling across Windows and Linux (powered by `rang`).
- **Fast and Native**: Built in modern C++20 for exceptional performance and minimal footprint.
- **Cross-Platform**: Fully compatible with Linux and Windows environments out-of-the-box.

## Installation

### Prerequisites
- [CMake 3.20](https://cmake.org/) or later
- C++20 compatible compiler (GCC 10+, Clang 10+, or MSVC 19.30+)
- A supported container runtime (Docker, Podman, or Containerd)
- [Just](https://github.com/casey/just) 

### Building from Source

```bash
git clone https://github.com/mmycin/inframan.git
cd inframan

# Configure the project
just release-preset  # or cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the project
just release-build    # or cmake --build build --config Release
```

The compiled binary will be placed inside the `bin/` directory.

## Getting Started

InfraMan organizes container workloads into distinct "infrastructure groups" with predefined providers and types. Once set up, you can interact with them rapidly from the CLI.

```bash
# Launch the interactive wizard to create a new infrastructure group
inframan cg

# List all registered infrastructure groups
inframan lg

# Designate an active group as your current context
inframan use <group_name>
```

## Infrastructure Types

InfraMan supports several infrastructure types with auto-detected commands:

- **COMPOSE**: Auto-detects `{provider}-compose up -d` for start and `{provider}-compose down` for stop, looks for `docker-compose.yml`
- **DOCKERFILE**: Auto-detects `{provider} build -t {job_name} . && {provider} run -d {job_name}` for start and `{provider} stop {job_name} && {provider} rm {job_name}` for stop, looks for `Dockerfile`
- **SERVICE**: Auto-detects `{provider} service start {job_name}` for start and `{provider} service stop {job_name}` for stop
- **TASK**: Auto-detects `{provider} run --rm {job_name}` for start and `{provider} stop {job_name}` for stop
- **NETWORK**: Auto-detects `{provider} network create {job_name}` for start and `{provider} network rm {job_name}` for stop
- **VOLUME**: Auto-detects `{provider} volume create {job_name}` for start and `{provider} volume rm {job_name}` for stop
- **CUSTOM**: Requires manual command specification (stop commands not auto-detected)

## Usage Examples

```bash
# Create a compose-based group with Docker provider
inframan cg
# Follow prompts: group name="webapps", provider="docker", type="compose"

# Add jobs to the group (auto-detects docker-compose.yml and command)
inframan aj
# Enter job name: frontend
# Auto-detected command: docker-compose up -d

# Run all jobs in the group concurrently
inframan rg

# Stop all jobs in the group
inframan sg

# Check status of all jobs
inframan st

# Read all jobs with their configurations
inframan rj
```

## Commands

| Command | Alias | Description |
|---------|-------|-------------|
| `create-group` | `cg` | Create a new infrastructure group |
| `list-group` | `lg` | List all infrastructure groups |
| `use-group` | `use` | Set active group context |
| `add-job` | `aj` | Add a job to a group |
| `run-group` | `rg` | Run all jobs in a group (concurrently) |
| `stop-group` | `sg` | Stop all jobs in a group |
| `status` | `st` | Show status of all jobs in a group |
| `read-job` | `rj` | Read jobs of a group |
| `update-job` | `uj` | Update a job in a group |
| `delete-job` | `dj` | Delete a job from a group |
| `update-group` | `ug` | Update group metadata |
| `delete-group` | `dg` | Delete a group |
| `execute-job` | `execute` | Execute a single job |

## Configuration & Persistence
InfraMan stores group settings and registry data locally as JSON files. Specifically, it utilizes `.infra_info.json` for mapping out groups. Physical configurations reside at:
- **Windows**: `C:/ProgramData/inframan/groups/`
- **Linux**: `/var/inframan/groups/`

## Libraries Used
InfraMan relies on a curated set of lightweight, header-only C++ dependencies, which are bundled directly inside the `include/libraries` folder for a seamless build process:
- **[CLI11](https://github.com/CLIUtils/CLI11)** - A command line parser for C++11 and beyond.
- **[nlohmann/json](https://github.com/nlohmann/json)** - JSON for Modern C++, used for group configurations.
- **[tabulate](https://github.com/p-ranav/tabulate)** - A table making library for C++ to cleanly render CLI metrics.
- **[rang](https://github.com/agauniyal/rang)** - A minimalist, header-only C++ library for terminal colors.
- **[linenoise-cpp](https://github.com/yhirose/cpp-linenoise)** - A small, self-contained line editing library.
    - **Note**: Heavily modified for this project to resolve linker conflicts, fix Linux LF handling, and correct critical return-logic inversions.
- **[tfile](https://github.com/andrew-d/tfile)** - A simple wrapper over standard I/O for safer cross-platform reads/writes.

## Contributing
Contributions are extremely welcome! Please refer to the [Contributing Code](docs/CONTRIBUTE.md) guide for information on setting up your dev environment, coding standards, and submitting patches.

We also expect all community members to abide by our [Code of Conduct](docs/CODE_OF_CONDUCT.md).

## License
This software is provided under the [MIT License](LICENSE).
