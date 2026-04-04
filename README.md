<div align="center">

# Byte-knight — Arch Linux Fork

**Arch Linux package repository — Fork of [Neuwj](https://github.com/Neuwj-00)/byte-knight**

![GPL v3](https://img.shields.io/badge/License-GPL_v3-blue?style=for-the-badge&logo=gnu&logoColor=white)
![Arch Linux](https://img.shields.io/badge/Arch_Linux-1793D1?style=for-the-badge&logo=archlinux&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Rust](https://img.shields.io/badge/Rust-000000?style=for-the-badge&logo=rust&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

</div>

---

## Overview

This repository is a fork of [Neuwj-00/byte-knight](https://github.com/Neuwj-00/byte-knight), adapted specifically for **Arch Linux** and its derivatives (Manjaro, EndeavourOS, Garuda, etc.).

The original project provides a suite of high-performance C++ terminal utilities with packaging support for Debian (`.deb`) and Red Hat (`.rpm`) ecosystems. This fork focuses on bringing those tools to the Arch Linux ecosystem.

---

## Included Tools

- **pacForge** — Universal Linux packaging wizard. Automates the creation of `.deb` and `.rpm` packages with licensing, metadata, and directory structuring.
- **repoForge** — Specialized utility for creating, managing, and maintaining local or remote APT and RPM package repositories.
- **foldertree (ftr)** — Highly customizable CLI tool that visualizes and prints directory tree structures.
- **warp / warpcplus** — C++ utility engineered for network operations, tunneling, and connectivity management.

---

## Installation

### Arch Linux — Build from Source

```bash
# Clone the repository
git clone https://github.com/Vniverse77/byte-knight.git
cd byte-knight/src

# Compile with g++ (example: foldertree)
g++ -std=c++17 -O2 -Wl,-z,relro,-z,now -o ftr foldertree.cpp

# Install the compiled binary to your system path
sudo install -Dm755 ftr /usr/bin/ftr
```

### Other Distributions

For Debian/Ubuntu and Fedora/RHEL installation instructions, refer to the original project: [Neuwj-00/byte-knight](https://github.com/Neuwj-00/byte-knight)

---

## Repository Structure

```
byte-knight/
├── src/                   # Source code
│   ├── foldertree.cpp
│   ├── pacForge.cpp
│   ├── repoForge.cpp
│   ├── warp.cpp
│   ├── gitForge.rs
│   ├── uninstallerdeb.py
│   └── RepositoryInstaller.sh
├── keys/                  # GPG signing keys
├── index.html             # Web server entry point
├── LICENSE                # GNU GPLv3 License
└── README.md
```

---

## Contributing

Contributions, bug reports, and feature requests are welcome. Feel free to open an issue or submit a pull request on the [Issues](https://github.com/Vniverse77/byte-knight/issues) page.

---

## Contact

**Fork maintainer:**

| | |
|---|---|
| Developer | Vniverse77 |
| GitHub | [github.com/Vniverse77](https://github.com/Vniverse77) |
| Role | Arch Linux packaging and distribution |

**Original project:**

| | |
|---|---|
| Developer | Neuwj |
| GitHub | [github.com/Neuwj-00](https://github.com/Neuwj-00) |
| Website | [neuwj-00.github.io/byte-knight](https://neuwj-00.github.io/byte-knight/) |
| Email | neuwj@bk.ru |

---

## License

This project is open-source and licensed under the [GNU General Public License v3.0](LICENSE).

Originally created by [Neuwj](https://github.com/Neuwj-00). Forked and maintained for Arch Linux by [Vniverse77](https://github.com/Vniverse77).
