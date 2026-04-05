<div align="center">

# Byte-Knight

**A Modern Linux Distribution & System Management Ecosystem**

[![Sponsor](https://img.shields.io/badge/Sponsor-Neuwj--00-pink?style=for-the-badge&logo=github-sponsors)](https://github.com/sponsors/Neuwj-00)

![GPL v3](https://img.shields.io/badge/License-GPL_v3-blue?style=for-the-badge&logo=gnu&logoColor=white)
![Ubuntu](https://img.shields.io/badge/Ubuntu-E9430F?style=for-the-badge&logo=ubuntu&logoColor=white)
![Debian](https://img.shields.io/badge/Debian-D70A53?style=for-the-badge&logo=debian&logoColor=white)
![Fedora](https://img.shields.io/badge/Fedora-51A2DA?style=for-the-badge&logo=fedora&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Rust](https://img.shields.io/badge/Rust-000000?style=for-the-badge&logo=rust&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![Built For Linux](https://img.shields.io/badge/Built_for-Linux-2ea44f?style=for-the-badge&logo=linux&logoColor=white)

</div>

<br>

## Overview

**Byte-Knight** is an integrated collection of high-performance system utilities designed for Linux, serving as a comprehensive software distribution hub. Beyond being a simple code repository, it provides an end-to-end hybrid infrastructure covering every stage of the software lifecycle — from development and packaging to digital signing and native distribution.

<br>

## Core Pillars

**1. Hybrid Multi-Language Stack**

The project leverages C++ for maximum system-level speed, Rust for memory safety and modern workflows, and Python for flexible automation. This "right tool for the right job" approach ensures that Byte-Knight utilities run with native performance.

**2. Professional Packaging & Distribution**

Byte-Knight hosts its own fully-fledged software repository. Tools are transformed into professional-grade `.deb` (Debian/Ubuntu/MX Linux) and `.rpm` (Fedora/RHEL) packages, fully compatible with standard system package managers like APT and DNF.

**3. Security-First Architecture**

Every binary distributed via Byte-Knight is digitally signed using GPG (GNU Privacy Guard). The entire distribution pipeline is audited via GitHub Actions to maintain the highest security standards for the end-user.

<br>

## Tools

### The Forge Series — Backbone of the Pipeline

The "Forge" series consists of three core engines that drive the production and distribution cycles of the Byte-Knight ecosystem.

<table>
  <thead>
    <tr>
      <th>Tool</th>
      <th>Language</th>
      <th>Role</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>pacForge</code></td>
      <td align="center">C++</td>
      <td><b>The Builder</b></td>
      <td>Takes raw source code or binaries and converts them into professional, signed <code>.deb</code> and <code>.rpm</code> packages — the most optimized path from "code to package."</td>
    </tr>
    <tr>
      <td><code>repoForge</code></td>
      <td align="center">C++</td>
      <td><b>The Distributor</b></td>
      <td>Manages the repository hierarchy, generates APT/DNF metadata indexes (<code>Packages.gz</code>, <code>repomd.xml</code>), and handles GPG signing protocols.</td>
    </tr>
    <tr>
      <td><code>gitForge</code></td>
      <td align="center">Rust</td>
      <td><b>The Automator</b></td>
      <td>Automates development and deployment workflows — Git operations, versioning, and CI/CD pipelines — ensuring all Forge components work in perfect harmony.</td>
    </tr>
  </tbody>
</table>

### System & Maintenance Modules

<table>
  <thead>
    <tr>
      <th>Tool</th>
      <th>Language</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>warp</code></td>
      <td align="center">C++</td>
      <td>A performance-oriented system utility designed for low-level optimization and high-speed process transitions.</td>
    </tr>
    <tr>
      <td><code>foldertree</code> / <code>ftr</code></td>
      <td align="center">C++</td>
      <td>A high-speed analysis tool that scans complex file systems and generates visual maps (HTML/Tree formats).</td>
    </tr>
    <tr>
      <td><code>uninstallerdeb</code></td>
      <td align="center">Python</td>
      <td>Ensures clean and safe removal of packages, leaving no residues on the system.</td>
    </tr>
    <tr>
      <td><code>RepositoryInstaller</code></td>
      <td align="center">Shell</td>
      <td>Imports GPG keys and configures the repository on the target system with a single command.</td>
    </tr>
  </tbody>
</table>

<br>

## Installation

### 1. Debian-based Systems
> Debian · Ubuntu · MX Linux · and others

<details>
<summary><b>Option A — Manual Installation (Step-by-Step)</b></summary>
<br>

```bash
# 1. Import the GPG Public Key for package verification
wget -qO - https://neuwj-00.github.io/byte-knight/keys/public.key | sudo gpg --dearmor -o /usr/share/keyrings/byte-knight-archive-keyring.gpg

# 2. Add the Byte-Knight repository to your sources list
echo "deb [signed-by=/usr/share/keyrings/byte-knight-archive-keyring.gpg] https://neuwj-00.github.io/byte-knight/debian stable main" | sudo tee /etc/apt/sources.list.d/byte-knight.list

# 3. Update package lists and install
sudo apt update
sudo apt install pacforge repoforge
```

</details>

<details>
<summary><b>Option B — Automated Installation (Scripted)</b></summary>
<br>

```bash
wget https://neuwj-00.github.io/byte-knight/src/RepositoryInstaller-deb.sh
chmod +x RepositoryInstaller-deb.sh
sudo ./RepositoryInstaller-deb.sh
```

</details>

---

### 2. Fedora & RHEL-based Systems
> Fedora · CentOS · RHEL · and others

<details>
<summary><b>Option A — Manual Installation (Step-by-Step)</b></summary>
<br>

```bash
# 1. Create the repository configuration file
sudo tee /etc/yum.repos.d/byte-knight.repo <<EOF
[byte-knight]
name=Byte-Knight Official Repository
baseurl=https://neuwj-00.github.io/byte-knight/rpm/
enabled=1
gpgcheck=1
gpgkey=https://neuwj-00.github.io/byte-knight/keys/public.key
EOF

# 2. Install Byte-Knight utilities via DNF
sudo dnf install warpcplus
```

</details>

<details>
<summary><b>Option B — Automated Installation (Scripted)</b></summary>
<br>

```bash
wget https://neuwj-00.github.io/byte-knight/src/RepositoryInstaller-rhel.sh
chmod +x RepositoryInstaller-rhel.sh
sudo ./RepositoryInstaller-rhel.sh
```

</details>

---

### 3. Build from Source

```bash
# Clone the repository
git clone https://github.com/Neuwj-00/byte-knight.git
cd byte-knight/source

# Compile using g++ (example: foldertree)
g++ -std=c++17 -O2 -Wl,-z,relro,-z,now -o ftr foldertree.cpp

# Install the binary to your system path
sudo install -Dm755 ftr /usr/bin/ftr
```

---

### 4. Arch-based Distributions

For Arch-based distributions, please refer to the dedicated fork:
[Arch-byte-knight](https://github.com/Neuwj-00/byte-knight/tree/Vniverse77-Arch-patch)

<br>

## Installer Script Sources

<details>
<summary>src/RepositoryInstaller-deb.sh</summary>
<br>

```bash
#!/bin/bash
set -e
echo "Byte-Knight: Configuring APT Repository..."
wget -qO - https://neuwj-00.github.io/byte-knight/keys/public.key | sudo gpg --dearmor -o /usr/share/keyrings/byte-knight-archive-keyring.gpg
echo "deb [signed-by=/usr/share/keyrings/byte-knight-archive-keyring.gpg] https://neuwj-00.github.io/byte-knight/debian stable main" | sudo tee /etc/apt/sources.list.d/byte-knight.list
sudo apt update
echo "Byte-Knight DEB Repo is ready! You can now run: sudo apt install pacforge"
```

</details>

<details>
<summary>src/RepositoryInstaller-rhel.sh</summary>
<br>

```bash
#!/bin/bash
set -e
echo "Byte-Knight: Configuring DNF/YUM Repository..."
sudo tee /etc/yum.repos.d/byte-knight.repo <<EOF
[byte-knight]
name=Byte-Knight Official Repository
baseurl=https://neuwj-00.github.io/byte-knight/rpm/
enabled=1
gpgcheck=1
gpgkey=https://neuwj-00.github.io/byte-knight/keys/public.key
EOF
echo "Byte-Knight RHEL Repo is ready! You can now run: sudo dnf install warpcplus"
```

</details>

<br>

## Repository Structure

```
byte-knight/
├── debian/             # APT repository files & pre-built .deb packages
├── rpm/                # DNF/YUM repository files & pre-built .rpm packages
├── source/             # C++ source files
│   ├── foldertree.cpp
│   ├── pacForge.cpp
│   ├── repoForge.cpp
│   └── warp.cpp
├── keys/               # GPG signing keys (public.key)
├── index.html          # Web server entry point
├── LICENSE             # GNU GPLv3 License
└── README.md
```

<br>

## Contributing

Contributions, bug reports, and feature requests are always welcome.
Feel free to open an issue or submit a pull request on the [Issues page](https://github.com/Neuwj-00/byte-knight/issues).

<br>

## 📫 Contact

| | |
|:---|:---|
| **Developer** | Neuwj |
| **GitHub** | [Neuwj-00](https://github.com/Neuwj-00) |
| **Website** | [neuwj-00.github.io/byte-knight](https://neuwj-00.github.io/byte-knight/) |
| **Email** | neuwj@bk.ru |

<br>

## License

This project is open-source and licensed under the **GNU General Public License v3.0**.

Created by [Neuwj](https://github.com/Neuwj-00).
