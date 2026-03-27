# Contributing to InfraMan

First off, thank you for considering contributing to InfraMan! It's people like you that make InfraMan such a great tool. 

## Code of Conduct
This project and everyone participating in it is governed by the [InfraMan Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

## Getting Started

### Local Development Setup
1. **Fork & Clone**: Fork the repository on GitHub and clone your fork locally.
   ```bash
   git clone https://github.com/<your-username>/inframan.git
   cd inframan
   ```
2. **Build the Tool**: InfraMan is managed via CMake.
   ```bash
   cmake -B build
   cmake --build build
   ```
3. **Running the Application**:
   You can run the compiled binary directly.
   ```bash
   ./bin/app
   ```
   *Note: In future versions, we aim to unify the internal cmake targets with the public `inframan` CLI naming. Currently, the target is called `app` internally.*

## How Can I Contribute?

### Reporting Bugs
This section guides you through submitting a bug report for InfraMan. Following these guidelines helps maintainers understand your report, reproduce the behavior, and find related reports.

- Use a clear and descriptive title for the issue.
- Describe the exact steps to reproduce the problem.
- Provide specific examples to demonstrate the steps, such as CLI invocation arguments and output logs.

### Suggesting Enhancements
Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, please:
- Use a clear and descriptive title for the issue.
- Provide a step-by-step description of the suggested enhancement.
- Explain why this enhancement would be useful to most InfraMan users.

### Pull Requests
- Fill in the required template.
- Make sure that all tests pass before submitting your PR.
- Follow our C++ styleguide (we use standard modern C++ guidelines with mostly LLVM style naming conventions).
- Document new code based on the project's documentation standards.
- End files with a newline.

## C++ Coding Standards
- We enforce **C++20**.
- Keep dependencies contained and cross-platform friendly (especially between Windows and POSIX equivalents, such as `tfile.hpp` file seek/IO wrappers).
- Use `std::string` and `std::string_view` for textual representations.
- Employ the bundled library versions (`nlohmann::json`, `tabulate`, etc.) for consistency.

Once again, thank you for your contribution to the codebase!
