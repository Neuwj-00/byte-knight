<div align="center">

# Byte Knight

**A Collection of Powerful CLI & TUI Tools for Linux**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/Vniverse77/byte-knight/blob/main/LICENSE)
[![AUR](https://img.shields.io/aur/version/pacforge?label=AUR%20%7C%20pacforge&color=1793d1)](https://aur.archlinux.org/packages/pacforge)

*Originally created by [Neuwj](https://github.com/Neuwj) • Forked & maintained by [Vniverse77](https://github.com/Vniverse77)*

</div>

---

## Overview

Byte Knight is a curated suite of C++ terminal tools designed to simplify everyday Linux system tasks. Each tool is built for performance, simplicity, and cross-distribution compatibility.

| Tool | Description | Status |
|------|-------------|--------|
| **pacForge** | Universal Linux packaging wizard (.deb & .rpm) | ✅ Stable |
| **warp-tui** | TUI manager for Cloudflare WARP CLI | ✅ Stable |

---

## pacForge

A high-performance packaging wizard that automates the creation of production-ready `.deb` and `.rpm` packages. Provide your binary — pacForge handles licensing, certification, directory structure, and final assembly.

### Features

- **Dual Distribution Support** — Build packages for both Debian-based and Red Hat-based systems
- **Automated Licensing** — Choose from 15+ open-source licenses (MIT, GPL, Apache, BSD, etc.)
- **Certification Metadata** — Optionally embed professional certification info into your packages
- **Interactive TUI** — Step-by-step guided workflow with built-in validation
- **Built-in Guide** — Press `0` anytime to open a reference guide in a new terminal

### Installation

**Arch Linux (AUR):**
```bash
yay -S pacforge
```

**Debian / Ubuntu:**
```bash
# Add the GPG key
curl -fsSL https://neuwj-00.github.io/pacForge/KEY.gpg | sudo gpg --dearmor -o /usr/share/keyrings/pacforge-archive-keyring.gpg

# Add the official repository
echo "deb [signed-by=/usr/share/keyrings/pacforge-archive-keyring.gpg] https://neuwj-00.github.io/pacForge/debian /" | sudo tee /etc/apt/sources.list.d/pacforge.list

# Update and install
sudo apt update && sudo apt install pacforge
```

**Fedora / RHEL:**
```bash
sudo tee /etc/yum.repos.d/pacforge.repo <<EOF
[pacforge]
name=pacForge Official Repository
baseurl=https://neuwj-00.github.io/pacForge/rhel/
enabled=1
gpgcheck=1
gpgkey=https://neuwj-00.github.io/pacForge/KEY.gpg
EOF

sudo dnf install pacforge
```

**Build from source:**
```bash
g++ -std=c++17 -O2 -Wl,-z,relro,-z,now -o pacforge source/pacForge.cpp -lpthread
sudo install -Dm755 pacforge /usr/bin/pacforge
```

---

##  warp-tui

A user-friendly TUI wrapper for managing the Cloudflare WARP CLI. Connect, disconnect, switch modes, configure DNS filtering, and run diagnostics — all from an interactive terminal interface.

### Features

- **One-Click Connect/Disconnect** — Manage your WARP connection instantly
- **Mode Switching** — Toggle between WARP, DNS-only, WARP+DoH, and Proxy (SOCKS5) modes
- **Protocol Selection** — Switch between WireGuard and MASQUE protocols
- **DNS Families & Filters** — Configure malware and adult content filtering
- **Teams / Zero Trust** — Enroll into Cloudflare Zero Trust organizations
- **Diagnostics** — Run `warp-diag` and view registration info directly from the TUI

### Prerequisites

- [Cloudflare WARP CLI](https://developers.cloudflare.com/warp-client/get-started/linux/) must be installed and registered
- On Arch Linux: install `cloudflare-warp-bin` from the AUR

### Installation

**Build from source:**
```bash
g++ -std=c++17 -O2 -Wl,-z,relro,-z,now -o warp-tui source/warp.cpp -lpthread
sudo install -Dm755 warp-tui /usr/bin/warp-tui
```

---

## Repository Structure

```
byte-knight/
├── source/
│   ├── pacForge.cpp       # Package builder source
│   ├── warp.cpp           # WARP TUI source
│   └── ...
├── debian/                # Pre-built .deb packages
├── rpm/                   # Pre-built .rpm packages
├── keys/                  # GPG signing keys
├── LICENSE
└── README.md
```

---

## Building from Source

All tools require a C++17 compatible compiler.

```bash
# Clone the repository
git clone https://github.com/Vniverse77/byte-knight.git
cd byte-knight

# Build pacForge
g++ -std=c++17 -O2 -Wl,-z,relro,-z,now -o pacforge source/pacForge.cpp -lpthread

# Build warp-tui
g++ -std=c++17 -O2 -Wl,-z,relro,-z,now -o warp-tui source/warp.cpp -lpthread
```

---

## Contributing

Contributions, issues, and feature requests are welcome! Feel free to check the [Issues](https://github.com/Vniverse77/byte-knight/issues) page.

---

## Contact

- **Developer:** Neuwj
- **Email:** neuwj@bk.ru
- **Fork Maintainer:** [Vniverse77](https://github.com/Vniverse77)

---

## License

This project is licensed under the [MIT License](LICENSE).
