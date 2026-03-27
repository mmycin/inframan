# InfraMan

**InfraMan** (formerly InfraManager) is a fast, cross-platform command-line tool written in C++ for managing container infrastructure groups directly from your terminal. It simplifies and automates the process of working with Docker, Podman, and Containerd configurations locally and regionally.

## Features
- **Multi-Provider Support**: Support for Docker, Podman, and Containerd runtimes natively.
- **Group Management**: Segment and group related infrastructure components—such as Dockerfiles, Compose setups, services, networks, and persistent volumes.
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
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build --config Release
```

The compiled binary will be placed inside the `bin/` directory.

## Getting Started

InfraMan organizes container workloads into distinct "infrastructure groups". Once set up, you can interact with them rapidly from the CLI.

```bash
# Launch the interactive wizard to create a new infrastructure group
inframan cg

# List all registered infrastructure groups
inframan lg

# Designate an active group as your current context
inframan use <group_name>
```

## Configuration & Persistence
InfraMan stores group settings and registry data locally as JSON files. Specifically, it utilizes `.infra_info.json` for mapping out groups. Physical configurations reside at:
- **Windows**: `C:/ProgramData/inframan/groups/`
- **Linux**: `/var/inframan/groups/`

## Contributing
Contributions are extremely welcome! Please refer to the [Contributing Code](docs/CONTRIBUTE.md) guide for information on setting up your dev environment, coding standards, and submitting patches.

We also expect all community members to abide by our [Code of Conduct](docs/CODE_OF_CONDUCT.md).

## License
This software is provided under the [MIT License](LICENSE).
