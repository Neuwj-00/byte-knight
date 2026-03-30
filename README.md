# Byte Knight 

Byte Knight is a user-friendly TUI (Terminal User Interface) application designed to easily install, configure, and manage the **Cloudflare WARP CLI**. 

This project was originally created by [Neuwj](https://github.com/Neuwj/byte-knight) and Forked by [Vniverse77](https://github.com/Vniverse77/) to simplify Cloudflare WARP management for Debian and RHEL-based systems. This fork extends the support and brings the ability to compile and run Byte Knight on **Arch Linux** and Arch-based distributions (like CachyOS).

---

##  Features

- **Easy Installation:** Simplifies the installation process of Cloudflare WARP.
- **TUI Experience:** Interactive terminal interface for user-friendly operation.
- **Cross-Distro Support:** Pre-built packages for Debian and RHEL, plus source compilation for Arch Linux.

---

##  Installation

### 1. Debian / Ubuntu Based Systems
Pre-built packages are located in the `debian` directory. You can install the `.deb` package using `dpkg` or `apt`.

### 2. RHEL / Fedora Based Systems
Pre-built packages are located in the `rpm` directory. You can install the `.rpm` package using `rpm` or `dnf`.

### 3. Arch Linux / CachyOS (Manual Compilation)
Since you are on Arch, We have recently added [AUR](https://aur.archlinux.org/packages/pacforge) repository to install easyly on your arch based deviceses!

#### Prerequisites
Open your terminal and install
yay -S pacforge
