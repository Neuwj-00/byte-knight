#!/bin/bash

echo "=================================================="
echo "       Byte Knight Repository Setup (Fedora/RHEL)"
echo "=================================================="
echo "[*] Initialization started..."
echo ""


echo "[*] Step 1: Importing GPG key..."
# RPM tabanlı sistemlerde GPG anahtarı doğrudan rpm --import ile sisteme tanıtılır
if ! sudo rpm --import https://neuwj-00.github.io/byte-knight/keys/public.key; then
    echo ""
    echo "[-] ERROR: Failed to download or import the GPG key!"
    echo "[-] Please check your internet connection and try the steps manually."
    sleep 5
    exit 1
fi

echo ""

echo "[*] Step 2: Adding the repository to yum.repos.d..."
# .repo formatında depo yapılandırmasını oluşturup /etc/yum.repos.d/ içine yazarız
# NOT: 'baseurl' kısmındaki /rpm dizinini GitHub Pages üzerinde oluşturduğunuzu varsayıyorum.
REPO_CONTENT="[byte-knight]
name=Byte Knight Repository
baseurl=https://neuwj-00.github.io/byte-knight/rpm
enabled=1
gpgcheck=1
repo_gpgcheck=0
gpgkey=https://neuwj-00.github.io/byte-knight/keys/public.key"

if ! echo "$REPO_CONTENT" | sudo tee /etc/yum.repos.d/byte-knight.repo > /dev/null; then
    echo ""
    echo "[-] ERROR: Failed to add the repository to the sources list!"
    echo "[-] Please check your permissions and try manually."
    sleep 5
    exit 1
fi

echo ""

echo "[*] Step 3: Updating package cache..."
# Fedora/RHEL sistemlerde apt update karşılığı dnf makecache'dir
if ! sudo dnf makecache; then
    echo ""
    echo "[-] ERROR: Failed to update package cache!"
    echo "[-] The repository might be unreachable. Please try running 'sudo dnf makecache' manually."
    sleep 5
    exit 1
fi

echo ""
echo "=================================================="
echo "[+] Process completed successfully!"
echo "[+] Repository added and package cache updated."
echo "=================================================="
sleep 3
