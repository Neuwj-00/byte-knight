#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <cctype>
#include <stdexcept>
#include <algorithm>

namespace fs = std::filesystem;

const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string GREEN = "\033[32m";
const std::string BLUE = "\033[34m";
const std::string YELLOW = "\033[33m";
const std::string RED = "\033[31m";
const std::string CYAN = "\033[36m";

class AbortOperation : public std::exception {
public:
    const char* what() const noexcept override {
        return "Operation aborted by user.";
    }
};

struct AuthMeta {
    std::string license;
    std::string cert;
};

struct PackageData {
    std::string binaryPath;
    std::string binaryName;
    std::string pkgName;
    std::string version;
    std::string release;
    std::string arch;
    std::string maintainer;
    AuthMeta authMeta;
    std::string description;
    std::string installDir;
    std::string outputDir;
};

void clearScreen() {
    std::system("clear");
}

void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void waitForEnter() {
    std::cout << "\n" << CYAN << "Press [ENTER] to continue..." << RESET;
    std::string dummy;
    if (!std::getline(std::cin, dummy)) std::exit(0);
}

void printBanner() {
    clearScreen();
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << YELLOW << "   .===.   " << RESET << BOLD << "PACKAGE FORGE\n";
    std::cout << BOLD << YELLOW << "   | " << CYAN << "*" << YELLOW << " |   " << RESET << "C++ Packaging Wizard | v1.1.4\n";
    std::cout << BOLD << YELLOW << "    \\ /    " << CYAN << "Created by: Neuwj - neuwj@bk.ru\n";
    std::cout << BOLD << YELLOW << "     V     " << RESET << "\n";
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << CYAN << " Tip: Type '0' for Guide, or 'p' to Cancel!\n\n" << RESET;
}

void printError(const std::string& message) {
    std::cout << RED << "[-] ERROR: " << RESET << message << "\n";
}

void printStep(const std::string& message) {
    std::cout << BLUE << "[*] " << RESET << message << "...\n";
    sleepMs(400);
}

void printSuccess(const std::string& message) {
    std::cout << GREEN << "[+] " << RESET << BOLD << message << RESET << "\n";
}

bool isValidStrictFormat(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::islower(c) && !std::isdigit(c) && c != '-' && c != '.' && c != '_') {
            return false;
        }
    }
    return true;
}

std::string trimPath(std::string path) {
    if (path.empty()) return path;

    size_t start = path.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    path.erase(0, start);
    path.erase(path.find_last_not_of(" \t\r\n") + 1);

    if (path.empty()) return path;

    if (path.front() == '\'' || path.front() == '"') path.erase(0, 1);
    if (!path.empty() && (path.back() == '\'' || path.back() == '"')) path.pop_back();

    return path;
}

std::string expandTilde(std::string path) {
    path = trimPath(path);
    if (!path.empty() && path[0] == '~') {
        if (path.length() == 1 || path[1] == '/') {
            const char* home = std::getenv("HOME");
            if (home) {
                path.replace(0, 1, std::string(home));
            }
        }
    }
    return path;
}

std::string escapeShellArg(const std::string& arg) {
    std::string escaped = "'";
    for (char c : arg) {
        if (c == '\'') escaped += "'\\''";
        else escaped += c;
    }
    escaped += "'";
    return escaped;
}

void openGuide() {
    clearScreen();
    std::cout << "\033[1;36m========================================\033[0m\n";
    std::cout << "\033[1;36m          PACKAGE FORGE GUIDE           \033[0m\n";
    std::cout << "\033[1;36m========================================\033[0m\n\n";
    std::cout << "\033[1;33m[?] Package Name:\033[0m myapp\n";
    std::cout << "    -> (Only lowercase letters, no spaces)\n\n";
    std::cout << "\033[1;33m[?] Version:\033[0m 1.0.0\n";
    std::cout << "\033[1;33m[?] Architecture:\033[0m amd64\n";
    std::cout << "    -> (Usually 'amd64' for 64-bit systems)\n\n";
    std::cout << "\033[1;33m[?] Release:\033[0m 1\n";
    std::cout << "    -> (RPM build release number, usually 1)\n\n";
    std::cout << "\033[1;33m[?] Maintainer:\033[0m Developer <dev@example.com>\n";
    std::cout << "    -> (Your name and email address)\n\n";
    std::cout << "\033[1;33m[?] Description:\033[0m A great terminal application\n";
    std::cout << "\033[1;33m[?] Binary Path:\033[0m /home/user/Documents/myapp\n";
    std::cout << "    -> (FULL path to your compiled binary, you can drag and drop)\n\n";
    std::cout << "\033[1;33m[?] Install Dir:\033[0m /usr/bin\n";
    std::cout << "    -> (Where it will be installed on the user's system)\n\n";
    std::cout << "\033[1;33m[?] Output Directory:\033[0m /home/user/Desktop\n";
    std::cout << "    -> (Where the final package will be saved)\n\n";
    std::cout << "\033[1;32mPress [ENTER] to close and continue...\033[0m\n";
    std::string dummy;
    std::getline(std::cin, dummy);
}

std::string askInput(const std::string& question, bool isRequired = true, bool strictFormat = false, bool ignoreGuideKey = false, bool checkExists = false) {
    std::string input;
    while (true) {
        std::cout << YELLOW << "[?] " << RESET << BOLD << question << ": " << RESET;
        std::getline(std::cin, input);

        input = trimPath(input);

        if (input == "p" || input == "P") {
            throw AbortOperation();
        }

        if (input == "0" && !ignoreGuideKey) {
            openGuide();
            continue;
        }

        if (input.empty()) {
            if (isRequired) {
                printError("This field cannot be empty. Please provide a value.");
                continue;
            } else {
                return "";
            }
        }

        input = expandTilde(input);

        if (strictFormat && !isValidStrictFormat(input)) {
            printError("Invalid format! Only lowercase letters, numbers, dashes (-), dots (.), and underscores (_). No spaces!");
            continue;
        }

        if (checkExists && !fs::is_regular_file(input)) {
            printError("Valid file not found! (Make sure you entered a file path, not a folder): " + input);
            continue;
        }

        return input;
    }
}

void promptMovePackage(const std::string& currentPath) {
    std::cout << "\n";
    std::string question = "Would you like to move the generated package (" + currentPath + ") to another location? [y/N]";
    std::string choice = askInput(question, false);

    if (choice == "y" || choice == "Y") {
        std::string newLocation = askInput("Enter the new destination directory path");
        if (!newLocation.empty()) {
            printStep("Moving package");
            try {
                fs::create_directories(newLocation);
                std::string fileName = fs::path(currentPath).filename().string();
                std::string newFilePath = newLocation + "/" + fileName;

                fs::copy_file(currentPath, newFilePath, fs::copy_options::overwrite_existing);
                fs::remove(currentPath);

                printSuccess("Package successfully moved to: " + newFilePath);
            } catch (const std::exception& e) {
                printError(std::string("Failed to move package: ") + e.what());
            }
        }
    }
}

AuthMeta askLicenseAndCert(const std::string& contextName) {
    std::cout << "\n" << BOLD << CYAN << "--- LICENSE AND CERTIFICATE FOR " << contextName << " ---" << RESET << "\n";
    std::cout << YELLOW << "Software Licenses:" << RESET << "\n";
    std::cout << "  [1] MIT            [6] AGPL v3.0      [11] ISC License\n";
    std::cout << "  [2] Apache 2.0     [7] BSD 3-Clause   [12] The Unlicense\n";
    std::cout << "  [3] GPL v3.0       [8] BSD 2-Clause   [13] CC BY\n";
    std::cout << "  [4] GPL v2.0       [9] MPL 2.0        [14] CC BY-SA\n";
    std::cout << "  [5] LGPL v3.0      [10] EPL 2.0       [15] CC0 (Zero)\n";
    std::cout << "  [" << GREEN << "c" << RESET << "] Enter Custom License Name\n";

    std::cout << YELLOW << "\nIT Certifications (Bonus):" << RESET << "\n";
    std::cout << "  [16] LPI Linux Ess.[21] Cisco CCNA    [26] CISSP\n";
    std::cout << "  [17] LPIC-1        [22] AWS Sol. Arch.[27] CEH\n";
    std::cout << "  [18] CompTIA A+    [23] Azure AZ-104  [28] CKA\n";
    std::cout << "  [19] CompTIA Net+  [24] Google IT Sup.[29] PMP\n";
    std::cout << "  [20] CompTIA Sec+  [25] RHCSA         [30] ITIL 4 Found.\n";
    std::cout << "  [" << GREEN << "c" << RESET << "] Enter Custom Certificate [" << RED << "0" << RESET << "] Skip Certification\n\n";

    AuthMeta meta;

    while (true) {
        std::string choice = askInput("Choose Software License for " + contextName + " (1-15 or 'c')", true, false);
        if (choice == "1") { meta.license = "MIT"; break; }
        else if (choice == "2") { meta.license = "Apache-2.0"; break; }
        else if (choice == "3") { meta.license = "GPL-3.0"; break; }
        else if (choice == "4") { meta.license = "GPL-2.0"; break; }
        else if (choice == "5") { meta.license = "LGPL-3.0"; break; }
        else if (choice == "6") { meta.license = "AGPL-3.0"; break; }
        else if (choice == "7") { meta.license = "BSD-3-Clause"; break; }
        else if (choice == "8") { meta.license = "BSD-2-Clause"; break; }
        else if (choice == "9") { meta.license = "MPL-2.0"; break; }
        else if (choice == "10") { meta.license = "EPL-2.0"; break; }
        else if (choice == "11") { meta.license = "ISC"; break; }
        else if (choice == "12") { meta.license = "Unlicense"; break; }
        else if (choice == "13") { meta.license = "CC-BY-4.0"; break; }
        else if (choice == "14") { meta.license = "CC-BY-SA-4.0"; break; }
        else if (choice == "15") { meta.license = "CC0-1.0"; break; }
        else if (choice == "c" || choice == "C") { meta.license = askInput("Enter Custom License Name"); break; }
        else { printError("Invalid selection! Please enter a number between 1-15 or 'c'."); }
    }

    while (true) {
        std::string choice = askInput("Choose IT Certificate for " + contextName + " (16-30, 'c', or '0')", true, false, true);
        if (choice == "0") { meta.cert = ""; break; }
        else if (choice == "16") { meta.cert = "LPI-Linux-Essentials"; break; }
        else if (choice == "17") { meta.cert = "LPIC-1"; break; }
        else if (choice == "18") { meta.cert = "CompTIA-A+"; break; }
        else if (choice == "19") { meta.cert = "CompTIA-Network+"; break; }
        else if (choice == "20") { meta.cert = "CompTIA-Security+"; break; }
        else if (choice == "21") { meta.cert = "Cisco-CCNA"; break; }
        else if (choice == "22") { meta.cert = "AWS-Solutions-Architect"; break; }
        else if (choice == "23") { meta.cert = "Azure-Administrator-AZ-104"; break; }
        else if (choice == "24") { meta.cert = "Google-IT-Support"; break; }
        else if (choice == "25") { meta.cert = "RHCSA"; break; }
        else if (choice == "26") { meta.cert = "CISSP"; break; }
        else if (choice == "27") { meta.cert = "CEH"; break; }
        else if (choice == "28") { meta.cert = "CKA"; break; }
        else if (choice == "29") { meta.cert = "PMP"; break; }
        else if (choice == "30") { meta.cert = "ITIL-4-Foundation"; break; }
        else if (choice == "c" || choice == "C") { meta.cert = askInput("Enter Custom Certificate Name"); break; }
        else { printError("Invalid selection! Please enter a number between 16-30, 'c', or '0'."); }
    }

    return meta;
}

PackageData gatherPackageInfo(const std::string& binaryPath, bool needDeb, bool needRpm) {
    PackageData data;
    data.binaryPath = binaryPath;
    data.binaryName = fs::path(binaryPath).filename().string();

    std::cout << "\n" << BOLD << CYAN << "=== ENTER INFORMATION: " << data.binaryName << " ===" << RESET << "\n";

    data.pkgName = askInput("Package Name for " + data.binaryName, true, true);
    data.version = askInput("Version for " + data.binaryName, true, true);

    if (needDeb) {
        data.arch = askInput("Architecture for " + data.binaryName + " (e.g., amd64)", true, true);
        if (data.arch == "x86_64") {
            std::cout << YELLOW << "[!] Warning: Debian uses 'amd64' instead of 'x86_64'. Auto-corrected." << RESET << "\n";
            data.arch = "amd64";
        }
    }
    if (needRpm) data.release = askInput("Release for " + data.binaryName + " (e.g., 1)", true, true);

    data.maintainer = askInput("Maintainer for " + data.binaryName + " (Name <email>)");

    if (data.maintainer.find('<') == std::string::npos || data.maintainer.find('>') == std::string::npos) {
        std::cout << YELLOW << "[!] Warning: Debian format requires a full email tag. Auto-correcting..." << RESET << "\n";
        data.maintainer += " <" + data.pkgName + "@localhost.localdomain>";
    }

    data.authMeta = askLicenseAndCert(data.binaryName);
    data.description = askInput("Description for " + data.binaryName, false);

    if (!data.authMeta.cert.empty()) {
        if (data.description.empty()) {
            data.description = "Packaged by certified professional: " + data.authMeta.cert;
        } else {
            data.description += " [Certified by: " + data.authMeta.cert + "]";
        }
    }

    data.installDir = askInput("Install Dir for " + data.binaryName + " (e.g., /usr/bin)");

    if (!data.installDir.empty() && data.installDir[0] != '/') {
        data.installDir = "/" + data.installDir;
    }
    if (data.installDir.length() > 1 && data.installDir.back() == '/') {
        data.installDir.pop_back();
    }

    data.outputDir = askInput("Output Directory for " + data.binaryName);

    return data;
}

void buildDebian(const PackageData& data, bool isBatch = false) {
    std::string safePkgName = data.pkgName;
    std::replace(safePkgName.begin(), safePkgName.end(), '_', '-');
    std::string safeVersion = data.version;
    std::replace(safeVersion.begin(), safeVersion.end(), '_', '-');

    std::cout << "\n" << BOLD << CYAN << "--- BUILDING DEBIAN PACKAGE (.deb) : " << safePkgName << " ---" << RESET << "\n";

    std::string relativeInstallDir = data.installDir;
    if (!relativeInstallDir.empty() && relativeInstallDir[0] == '/') relativeInstallDir.erase(0, 1);

    try {
        fs::create_directories(data.outputDir);
    } catch (const fs::filesystem_error& e) {
        printError("Failed to create output directory! Permission denied or invalid path: " + data.outputDir);
        return;
    }

    std::string buildDir = data.outputDir + "/" + safePkgName + "_" + safeVersion + "_" + data.arch;

    printStep("Creating directory hierarchy in " + data.outputDir);
    try {
        fs::create_directories(buildDir + "/DEBIAN");
        fs::create_directories(buildDir + "/" + relativeInstallDir);
    } catch (const fs::filesystem_error& e) {
        printError("Failed to create build hierarchy! Check permissions.");
        return;
    }

    printStep("Writing control file");
    std::ofstream controlFile(buildDir + "/DEBIAN/control");
    controlFile << "Package: " << safePkgName << "\n";
    controlFile << "Version: " << safeVersion << "\n";
    controlFile << "Architecture: " << data.arch << "\n";
    controlFile << "Maintainer: " << data.maintainer << "\n";
    if (!data.description.empty()) {
        controlFile << "Description: " << data.description << "\n";
    }
    controlFile << "\n";
    controlFile.close();

    printStep("Generating copyright/license file");
    std::string docDir = buildDir + "/usr/share/doc/" + safePkgName;
    fs::create_directories(docDir);

    std::ofstream copyrightFile(docDir + "/copyright");
    copyrightFile << "Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/\n";
    copyrightFile << "Upstream-Name: " << safePkgName << "\n\n";
    copyrightFile << "Files: *\n";
    copyrightFile << "Copyright: " << data.maintainer << "\n";
    copyrightFile << "License: " << data.authMeta.license << "\n";
    copyrightFile.close();

    printStep("Copying binary to target");
    try {
        std::string targetBinary = buildDir + "/" + relativeInstallDir + "/" + data.pkgName;
        fs::copy_file(data.binaryPath, targetBinary, fs::copy_options::overwrite_existing);
        fs::permissions(targetBinary, fs::perms::owner_all | fs::perms::group_read | fs::perms::group_exec | fs::perms::others_read | fs::perms::others_exec);
    } catch (const fs::filesystem_error& e) {
        printError("Failed to copy binary! Check the path: " + data.binaryPath);
        return;
    }

    printStep("Running dpkg-deb");
    std::string buildCmd = "dpkg-deb --build " + escapeShellArg(buildDir);
    int result = std::system(buildCmd.c_str());

    if (result == 0) {
        printStep("Cleaning up temporary files");
        fs::remove_all(buildDir);
        std::string finalDebPath = buildDir + ".deb";
        printSuccess("Debian package successfully built: " + finalDebPath);

        if (!isBatch) {
            promptMovePackage(finalDebPath);
        }
    } else {
        printError("dpkg-deb failed during package creation.");
        fs::remove_all(buildDir);
    }
}

void buildRPM(const PackageData& data, bool isBatch = false) {
    std::string safeVersion = data.version;
    std::replace(safeVersion.begin(), safeVersion.end(), '-', '_');
    std::string safeRelease = data.release;
    std::replace(safeRelease.begin(), safeRelease.end(), '-', '_');

    std::cout << "\n" << BOLD << YELLOW << "--- BUILDING RPM PACKAGE (.rpm) : " << data.pkgName << " ---" << RESET << "\n";

    try {
        fs::create_directories(data.outputDir);
    } catch (const fs::filesystem_error& e) {
        printError("Failed to create output directory!");
        return;
    }

    std::string workspace = data.outputDir + "/" + data.pkgName + "_rpm_workspace";

    printStep("Preparing RPM workspace");
    try {
        fs::create_directories(workspace + "/BUILD");
        fs::create_directories(workspace + "/RPMS");
        fs::create_directories(workspace + "/SOURCES");
        fs::create_directories(workspace + "/SPECS");
        fs::create_directories(workspace + "/SRPMS");
    } catch (const fs::filesystem_error& e) {
        printError("Failed to create RPM workspace hierarchy!");
        return;
    }

    printStep("Copying source binary to SOURCES");
    fs::copy_file(data.binaryPath, workspace + "/SOURCES/" + data.binaryName, fs::copy_options::overwrite_existing);

    std::string specPath = workspace + "/SPECS/" + data.pkgName + ".spec";
    printStep("Writing SPEC file");

    std::ofstream specFile(specPath);
    specFile << "Name:           " << data.pkgName << "\n";
    specFile << "Version:        " << safeVersion << "\n";
    specFile << "Release:        " << safeRelease << "%{?dist}\n";
    specFile << "Source0:        " << data.binaryName << "\n";
    specFile << "Summary:        " << (data.description.empty() ? data.pkgName + " package" : data.description) << "\n\n";
    specFile << "License:        " << data.authMeta.license << "\n";
    specFile << "Packager:       " << data.maintainer << "\n\n";
    specFile << "%description\n" << (data.description.empty() ? data.pkgName + " package" : data.description) << "\n\n";

    specFile << "%prep\n";
    specFile << "cp \"%{SOURCE0}\" .\n\n";
    specFile << "%build\n\n";
    specFile << "%install\n";
    specFile << "mkdir -p \"%{buildroot}" << data.installDir << "\"\n";
    specFile << "install -m 755 \"" << data.binaryName << "\" \"%{buildroot}" << data.installDir << "/" << data.pkgName << "\"\n\n";
    specFile << "%files\n";
    specFile << "\"" << data.installDir << "/" << data.pkgName << "\"\n\n";
    specFile.close();

    printStep("Running rpmbuild");
    std::string currentPath = fs::absolute(workspace).string();
    std::string buildCmd = "rpmbuild --define '_topdir " + currentPath + "' -bb " + escapeShellArg(specPath);

    int result = std::system(buildCmd.c_str());

    if (result == 0) {
        printStep("Extracting RPM and cleaning up workspace");
        bool rpmFound = false;
        std::string finalRpmPath = "";

        for (const auto& entry : fs::recursive_directory_iterator(workspace + "/RPMS")) {
            if (entry.is_regular_file() && entry.path().extension() == ".rpm") {
                finalRpmPath = data.outputDir + "/" + entry.path().filename().string();
                fs::copy_file(entry.path(), finalRpmPath, fs::copy_options::overwrite_existing);
                rpmFound = true;
            }
        }
        fs::remove_all(workspace);

        if (rpmFound) {
            printSuccess("RPM package successfully built!");
            if (!isBatch) {
                promptMovePackage(finalRpmPath);
            }
        } else {
            printError("RPM build finished but no .rpm file was found!");
        }
    } else {
        printError("rpmbuild failed.");
        fs::remove_all(workspace);
    }
}

void batchPackagingMenu() {
    clearScreen();
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << GREEN << "             BATCH PACKAGING MODE\n" << RESET;
    std::cout << BOLD << BLUE << "=================================================\n\n" << RESET;

    std::cout << BOLD << "Which package types do you want to create?\n" << RESET;
    std::cout << "  [" << GREEN << "1" << RESET << "] Only DEB\n";
    std::cout << "  [" << YELLOW << "2" << RESET << "] Only RPM\n";
    std::cout << "  [" << CYAN << "3" << RESET << "] Both (DEB + RPM)\n";

    std::string typeChoice = askInput("Your choice", true, false, true);
    bool buildDeb = (typeChoice == "1" || typeChoice == "3");
    bool buildRpm = (typeChoice == "2" || typeChoice == "3");

    if (!buildDeb && !buildRpm) {
        printError("Invalid choice. Batch packaging aborted.");
        return;
    }

    std::vector<std::string> binaryPaths;

    while (true) {
        clearScreen();
        std::cout << BOLD << CYAN << "--- ADDED FILES ---\n" << RESET;
        if (binaryPaths.empty()) {
            std::cout << YELLOW << "  No files added yet.\n" << RESET;
        } else {
            for (size_t i = 0; i < binaryPaths.size(); ++i) {
                std::cout << "  " << GREEN << (i + 1) << ". " << RESET << binaryPaths[i] << "\n";
            }
        }
        std::cout << "\n" << BOLD << "Please drag and drop the binary file here or enter its full path.\n";
        std::cout << "Type '" << GREEN << "confirm" << RESET << "' and press ENTER to confirm and proceed to questions.\n" << RESET;

        std::string input = askInput("File Path or 'confirm'", true, false, true);

        if (input == "confirm" || input == "CONFIRM") {
            if (binaryPaths.empty()) {
                printError("You haven't added any files! Type 'p' to cancel.");
                sleepMs(1500);
                continue;
            }
            break;
        }

        std::string expandedPath = expandTilde(input);
        if (fs::is_regular_file(expandedPath)) {
            if (std::find(binaryPaths.begin(), binaryPaths.end(), expandedPath) != binaryPaths.end()) {
                printError("This file is already added to the list!");
                sleepMs(1500);
            } else {
                binaryPaths.push_back(expandedPath);
                printSuccess("File added to the list!");
                sleepMs(600);
            }
        } else {
            printError("File not found! Check the path.");
            sleepMs(1500);
        }
    }

    std::vector<PackageData> packagesToBuild;
    for (const auto& path : binaryPaths) {
        packagesToBuild.push_back(gatherPackageInfo(path, buildDeb, buildRpm));
    }

    std::cout << "\n" << BOLD << GREEN << ">>> ALL INFORMATION RECEIVED. STARTING BUILD... <<<" << RESET << "\n";
    sleepMs(1000);

    for (const auto& data : packagesToBuild) {
        if (buildDeb) buildDebian(data, true);
        if (buildRpm) buildRPM(data, true);
    }

    printSuccess("Batch packaging process completed!");
}

void installTools() {
    std::cout << "\n" << BOLD << CYAN << "--- INSTALLING REQUIRED TOOLS ---" << RESET << "\n";
    bool hasApt = (std::system("command -v apt > /dev/null 2>&1") == 0);
    bool hasDnf = (std::system("command -v dnf > /dev/null 2>&1") == 0);
    bool hasPacman = (std::system("command -v pacman > /dev/null 2>&1") == 0);

    int result = -1;

    if (hasApt) {
        printStep("Debian/Ubuntu/MX Linux detected");
        result = std::system("sudo apt update && sudo apt install -y dpkg rpm");
    } else if (hasDnf) {
        printStep("Fedora/RHEL detected");
        result = std::system("sudo dnf install -y rpm-build dpkg");
    } else if (hasPacman) {
        printStep("Arch Linux detected");
        result = std::system("sudo pacman -Sy --noconfirm rpm-tools dpkg");
    } else {
        printError("No supported package manager found.");
        return;
    }

    if (result == 0) {
        printSuccess("All required tools installed successfully!");
    } else {
        printError("Some packages may have failed to install.");
    }
}

void printOutro() {
    std::cout << "\n" << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << GREEN << "  Thank you for using Package Forge!\n" << RESET;
    std::cout << CYAN << "  Created by: Neuwj\n" << RESET;
    std::cout << BOLD << BLUE << "=================================================\n\n" << RESET;
}

int main() {
    while(true) {
        try {
            printBanner();

            std::cout << BOLD << "Which armor (package) do you want to forge, knight?\n" << RESET;
            std::cout << "  [" << GREEN << "1" << RESET << "] Debian / Ubuntu  (" << GREEN << ".deb" << RESET << ")\n";
            std::cout << "  [" << YELLOW << "2" << RESET << "] Fedora / RedHat (" << YELLOW << ".rpm" << RESET << ")\n";
            std::cout << "  [" << CYAN << "3" << RESET << "] Batch Packaging Mode (" << BOLD << "NEW" << RESET << ")\n";
            std::cout << "  [" << BLUE << "4" << RESET << "] Install Required Tools (dpkg, rpmbuild, etc.)\n";
            std::cout << "  [" << YELLOW << "0" << RESET << "] Open Example Guide\n";
            std::cout << "  [" << RED << "q" << RESET << "] Exit\n\n";

            std::string choice = askInput("Choice", true, false, true);

            if (choice == "1") {
                std::string binPath = askInput("Binary Path", true, false, false, true);
                PackageData data = gatherPackageInfo(binPath, true, false);
                buildDebian(data);
                waitForEnter();
            } else if (choice == "2") {
                std::string binPath = askInput("Binary Path", true, false, false, true);
                PackageData data = gatherPackageInfo(binPath, false, true);
                buildRPM(data);
                waitForEnter();
            } else if (choice == "3") {
                batchPackagingMenu();
                waitForEnter();
            } else if (choice == "4") {
                installTools();
                waitForEnter();
            } else if (choice == "0") {
                openGuide();
            } else if (choice == "q" || choice == "Q") {
                std::cout << "\nClosing the forge. See you later!\n";
                break;
            } else {
                printError("Invalid choice! Please enter one of the numbers from the menu.");
                sleepMs(1000);
            }

        } catch (const AbortOperation&) {
            std::cout << "\n" << RED << BOLD << "[!] Operation Cancelled. Returning to the main menu..." << RESET << "\n";
            sleepMs(1200);
        }
    }

    printOutro();
    return 0;
}
