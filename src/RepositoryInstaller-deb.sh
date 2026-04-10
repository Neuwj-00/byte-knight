#!/bin/bash

echo "=================================================="
echo "       Byte Knight Repository Setup"
echo "=================================================="
echo "[*] Initialization started..."
echo ""

echo "[*] Step 1: Downloading and adding GPG key..."
sudo install -m 0755 -d /etc/apt/keyrings
if ! wget -qO- https://neuwj-00.github.io/byte-knight/keys/public.key | sudo gpg --dearmor --yes -o /etc/apt/keyrings/byte-knight.gpg; then
    echo ""
    echo "[-] ERROR: Failed to download or add the GPG key!"
    echo "[-] Please check your internet connection and try the steps manually."
    sleep 5
    exit 1
fi

echo ""

echo "[*] Step 2: Adding the repository to sources list..."
if ! echo "deb [arch=amd64 signed-by=/etc/apt/keyrings/byte-knight.gpg] https://neuwj-00.github.io/byte-knight/debian stable main" | sudo tee /etc/apt/sources.list.d/byte-knight.list > /dev/null; then
    echo ""
    echo "[-] ERROR: Failed to add the repository to the sources list!"
    echo "[-] Please check your permissions and try manually."
    sleep 5
    exit 1
fi

echo ""

echo "[*] Step 3: Updating package lists..."
if ! sudo apt update; then
    echo ""
    echo "[-] ERROR: Failed to update package lists!"
    echo "[-] The repository might be unreachable. Please try running 'sudo apt update' manually."
    sleep 5
    exit 1
fi

echo ""
echo "=================================================="
echo "[+] Process completed successfully!"
echo "[+] Repository added and package lists updated."
echo "=================================================="
sleep 3
