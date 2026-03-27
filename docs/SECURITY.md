# Security Policy

At **InfraMan**, we believe that infrastructure tools should operate with transparency and respect your operational privacy. We prioritize your system's data security by ensuring exactly zero unauthorized access, extraction, or tracking.

## Operational Privacy Guarantee: Zero Data Collection
InfraMan does **not** collect telemetry, user data, server metrics, logs, or file contents. We will absolutely never phone home or transmit your configuration setups (such as Dockerfiles, environments, or passwords/tokens) to third-party endpoints.

- **Air-Gapped Compatible**: Since there is no telemetry processing or external verification systems embedded in our binary, InfraMan can run smoothly in offline, air-gapped data centers.
- **Local Persistence**: Any configuration states (e.g. `infragroups.json` or `.infra_info.json`) created are retained strictly in your secure `C:/ProgramData/` or `/var/` OS directories and parsed locally at runtime.

No user tracking, no hidden backdoors—we aren't stealing your data.

## Supported Versions

Updates and vulnerability coverage map sequentially alongside semantic tags.

| Version | Supported          |
| ------- | ------------------ |
| v0.1.x    | ✅ |

## Reporting a Vulnerability

Since InfraMan operates directly on host machine kernels via Docker/Podman APIs or `tfile.hpp` file seek bindings, any arbitrary arbitrary code execution vectors or filesystem path traversals found during testing are treated with the highest severity.

If you discover a security vulnerability, we strongly appreciate your help in disclosing it privately over public submission. 

Please email the developer core contact at:
**[mycin.mit@gmail.com](mailto:mycin.mit@gmail.com)**

Include a description of the issue, any context (OS architecture compilation vs runtime), and reproduction instructions. You will receive a response within 48 hours to coordinate a patched release schedule.

Thank you for helping keep the open source infrastructure ecosystem safe and fully democratized.
