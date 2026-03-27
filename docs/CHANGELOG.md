# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2026-03-27

### Added
- Configured native Windows compilation strategies using MSVC (`_fseeki64` instead of POSIX `fseeko`).
- Interactive command-line creation of infrastructure groups.
- Detailed output table formatting using `tabulate`.
- Multi-provider container support built-in (`docker`, `podman`, `containerd`).
- Comprehensive open-source documentation including README, licensing, codes of conduct, and contribution guidelines.

### Changed
- **[Major Refactor]** Renamed the project and corresponding namespace strings from `inframanager` to `inframan`.
- Streamlined `GroupRegistry` persistence so that mapping files automatically populate in absolute `std::filesystem` environments securely (`C:/ProgramData/inframan/` or `/var/inframan/`).

### Fixed
- Addressed CWD data persistence bug: infrastructure mappings are now strictly bound to persistent system configurations rather than floating relative files.
