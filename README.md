<div align="center">

📦 Byte-knight

A Custom Linux Package Repository by Neuwj

</div>

Overview

Byte Knight: High-Performance Linux Utility Suite & Distribution Repository

This repository serves as the official source and distribution hub for the Byte Knight suite—an integrated collection of high-performance system utilities developed using a multi-language stack, including C++, Rust, Python, and Shell.

Designed for efficiency and scalability, the suite provides essential tools for advanced Linux system management. This repository is fully structured as a production-grade package distribution center, offering natively signed and verified installations for both Debian-based (.deb) and Red Hat-based (.rpm) Linux environments.

Security & Integrity: All binary distributions undergo automated security auditing via VirusTotal and GitHub Actions to ensure software integrity and system safety.

🛠️ Included Tools

pacForge (C++) — The Universal Linux Packaging Wizard. Automates the creation of .deb and .rpm packages, fully handling licensing, metadata generation, and directory structuring.

repoForge (C++) — The Repository Architect. A specialized utility for creating, managing, and maintaining local or remote APT and RPM package repositories.

foldertree / ftr (C++) — The Directory Visualizer. A highly customizable CLI tool that visualizes and prints the tree structure of specified directories with precision.

warp / warpcplus (C++) — The Network Forger. An advanced utility engineered for network operations, secure tunneling, and connectivity management.

gitForge (Rust) — A memory-safe, high-speed utility designed for Git repository tracking and automation.

uninstallerdeb (Python) — A flexible script for safely and cleanly removing installed Debian packages.

RepositoryInstaller (Shell) — The automated deployment script that seamlessly configures the Byte Knight APT/RPM repositories on your local machine.

Installation

Debian / Ubuntu
```bash
# Download the script from the Releases section and run it with this command
chmod +x "RepositoryInstaller.sh" && ./"RepositoryInstaller.sh"


# Manual install
# Add the GPG key for secure package verification
curl -fsSL [https://neuwj-00.github.io/byte-knight/keys/public.key](https://neuwj-00.github.io/byte-knight/keys/public.key) | sudo gpg --dearmor -o /usr/share/keyrings/byte-knight-keyring.gpg

# Add the official repository to your APT sources
echo "deb [signed-by=/usr/share/keyrings/byte-knight-keyring.gpg] [https://neuwj-00.github.io/byte-knight/debian](https://neuwj-00.github.io/byte-knight/debian) stable main" | sudo tee /etc/apt/sources.list.d/byte-knight.list

# Update package lists and install your desired tools
sudo apt update
sudo apt install pacforge repoforge ftr warpcplus
```

Fedora / RHEL
```bash
# Download the script from the Releases section and run it with this command
chmod +x "RepositoryInstaller.sh" && ./"RepositoryInstaller.sh"


# Manual install
# Add the repository configuration
sudo tee /etc/yum.repos.d/byte-knight.repo <<EOF
[byte-knight]
name=byte-knight Official Repository
baseurl=[https://neuwj-00.github.io/byte-knight/rpm/](https://neuwj-00.github.io/byte-knight/rpm/)
enabled=1
gpgcheck=0
EOF

# Install the packages
sudo dnf install pacforge repoforge warpcplus
```
Build from Source

# Clone the repository
git clone [https://github.com/Neuwj-00/byte-knight.git](https://github.com/Neuwj-00/byte-knight.git)
cd byte-knight/src

# Compile the tools using g++ (Example for foldertree)
g++ -std=c++17 -O2 -Wl,-z,relro,-z,now -o ftr foldertree.cpp


Arch distributions

For Arch-based distributions, please refer to the following fork: Arch-byte-knight.

# Install the compiled binary to your system path
sudo install -Dm755 ftr /usr/bin/ftr


📁 Repository Structure

byte-knight/
├── debian/                # APT repository files & pre-built .deb packages
├── rpm/                   # DNF/YUM repository files & pre-built .rpm packages
├── src/                   # Source codes and scripts
│   ├── foldertree.cpp     # Directory visualizer C++ source
│   ├── gitForge.rs        # Git repository tracking utility Rust source
│   ├── pacForge.cpp       # Packaging wizard C++ source
│   ├── repoForge.cpp      # Repository architect C++ source
│   ├── warp.cpp           # Network operations utility C++ source
│   ├── uninstallerdeb.py  # Debian package removal Python script
│   └── RepositoryInstaller.sh # Automated deployment script
├── keys/                  # GPG signing keys (public.key)
├── index.html             # Web server entry point
├── tree.html              # Web view for directory tree
├── sponsor.json           # Sponsorship configurations
├── LICENSE                # GNU GPLv3 License
└── README.md              # Documentation


🤝 Contributing

Contributions, bug reports, and feature requests are always welcome! Feel free to open an issue or submit a pull request on the Issues page.

📫 Contact

Developer: Neuwj

GitHub: Neuwj-00

Website: neuwj-00.github.io/byte-knight

Email: neuwj@bk.ru

📄 !!License!!

This project is open-source and licensed under the GNU General Public License v3.0.

Created by Neuwj
