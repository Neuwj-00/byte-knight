#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <cctype>

namespace fs = std::filesystem;

const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string GREEN = "\033[32m";
const std::string BLUE = "\033[34m";
const std::string YELLOW = "\033[33m";
const std::string RED = "\033[31m";
const std::string CYAN = "\033[36m";

void clearScreen() {
    std::system("clear");
}

void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void printBanner() {
    clearScreen();
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << YELLOW << "   .===.   " << RESET << BOLD << "PACKAGE FORGE\n";
    std::cout << BOLD << YELLOW << "   | " << CYAN << "+" << YELLOW << " |   " << RESET << "C++ Packaging Wizard | v0.1.0\n";
    std::cout << BOLD << YELLOW << "    \\ /    " << CYAN << "Created by Neuwj - neuwj@bk.ru\n";
    std::cout << BOLD << YELLOW << "     V     " << RESET << "\n";
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << CYAN << " Tip: Type '0' anytime to open the Example Guide!\n\n" << RESET;
}

void printError(const std::string& message) {
    std::cout << RED << "[-] ERROR: " << RESET << message << "\n";
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

void openGuide() {
    std::ofstream guideFile("forge_guide.sh");
    guideFile << "#!/bin/bash\n"
    << "echo -e \"\\033[1;36m========================================\\033[0m\"\n"
    << "echo -e \"\\033[1;36m          PACKAGE FORGE GUIDE           \\033[0m\"\n"
    << "echo -e \"\\033[1;36m========================================\\033[0m\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;33m[?] Package Name:\\033[0m myapp\"\n"
    << "echo -e \"    -> (Only lowercase letters, no spaces)\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;33m[?] Version:\\033[0m 1.0.0\"\n"
    << "echo -e \"    -> (Version number of your app)\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;33m[?] Architecture:\\033[0m amd64\"\n"
    << "echo -e \"    -> (Usually 'amd64' for 64-bit Debian systems)\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;33m[?] Release:\\033[0m 1\"\n"
    << "echo -e \"    -> (RPM build release number, usually 1)\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;33m[?] Maintainer:\\033[0m Developer <dev@example.com>\"\n"
    << "echo -e \"    -> (Your name and email)\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;33m[?] Description:\\033[0m A great terminal application\"\n"
    << "echo -e \"    -> (Short info about your package, optional)\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;33m[?] Binary Path:\\033[0m /home/user/Documents/myapp\"\n"
    << "echo -e \"    -> (Full path to your compiled binary on YOUR system)\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;33m[?] Install Dir:\\033[0m /usr/bin\"\n"
    << "echo -e \"    -> (Where it should be installed on the USER's system)\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;33m[?] Output Directory:\\033[0m /home/user/Desktop\"\n"
    << "echo -e \"    -> (Where the final package should be saved)\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;35m--- SOFTWARE LICENSES ---\\033[0m\"\n"
    << "echo -e \"  1. MIT: Most popular, simplest. \\\"Do what you want\\\".\"\n"
    << "echo -e \"  2. Apache 2.0: Corporate favorite. Protects patents.\"\n"
    << "echo -e \"  3. GNU GPL v3.0: Copyleft: If you use it, yours must be open.\"\n"
    << "echo -e \"  4. GNU GPL v2.0: Linux kernel license. Industry standard.\"\n"
    << "echo -e \"  5. GNU LGPL v3.0: Ideal for libs, allows commercial linking.\"\n"
    << "echo -e \"  6. GNU AGPL v3.0: Cloud/Web; requires network source sharing.\"\n"
    << "echo -e \"  7. BSD 3-Clause: Like MIT but protects name/endorsement.\"\n"
    << "echo -e \"  8. BSD 2-Clause: Simpler BSD version (FreeBSD choice).\"\n"
    << "echo -e \"  9. Mozilla (MPL 2.0): File-based copyleft.\"\n"
    << "echo -e \" 10. Eclipse (EPL 2.0): Used in enterprise Java projects.\"\n"
    << "echo -e \" 11. ISC License: BSD/MIT hybrid, very short and clear.\"\n"
    << "echo -e \" 12. The Unlicense: Declares code completely public domain.\"\n"
    << "echo -e \" 13. Creative Commons BY: \\\"Give attribution\\\" license.\"\n"
    << "echo -e \" 14. CC BY-SA: Share alike (Wikipedia style).\"\n"
    << "echo -e \" 15. CC0 (Zero): No copyright claimed at all.\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;35m--- IT CERTIFICATIONS (Bonus) ---\\033[0m\"\n"
    << "echo -e \" 16. LPI Linux Essentials: Official entry ticket to Linux.\"\n"
    << "echo -e \" 17. LPIC-1: Proof that you are a Linux Administrator.\"\n"
    << "echo -e \" 18. CompTIA A+: The \\\"driver's license\\\" for IT basics.\"\n"
    << "echo -e \" 19. CompTIA Network+: Starting point for networking.\"\n"
    << "echo -e \" 20. CompTIA Security+: Respected entry to cybersecurity.\"\n"
    << "echo -e \" 21. Cisco CCNA: Most recognized network certification.\"\n"
    << "echo -e \" 22. AWS Solutions Architect: World leader in cloud.\"\n"
    << "echo -e \" 23. Azure Administrator (AZ-104): Corporate cloud pref.\"\n"
    << "echo -e \" 24. Google IT Support Certificate: Great for industry entry.\"\n"
    << "echo -e \" 25. Red Hat Certified Admin (RHCSA): Hard & highly respected.\"\n"
    << "echo -e \" 26. CISSP: Highest level professional security license.\"\n"
    << "echo -e \" 27. CEH (Ethical Hacker): The \\\"white hat\\\" hacker license.\"\n"
    << "echo -e \" 28. CKA (Kubernetes Admin): Popular modern DevOps cert.\"\n"
    << "echo -e \" 29. PMP: Project management license (for large teams).\"\n"
    << "echo -e \" 30. ITIL 4 Foundation: Standard for IT service processes.\"\n"
    << "echo -e \"\"\n"
    << "echo -e \"\\033[1;32mPress [ENTER] to close this guide window...\\033[0m\"\n"
    << "read\n"
    << "rm -- \"$0\"\n";
    guideFile.close();

    std::system("chmod +x forge_guide.sh");
    std::system("(x-terminal-emulator -e ./forge_guide.sh || xfce4-terminal -e ./forge_guide.sh || gnome-terminal -- ./forge_guide.sh || konsole -e ./forge_guide.sh) 2>/dev/null &");
}

std::string askInput(const std::string& question, bool isRequired = true, bool strictFormat = false, bool ignoreGuideKey = false) {
    std::string input;
    while (true) {
        std::cout << YELLOW << "[?] " << RESET << BOLD << question << ": " << RESET;
        std::getline(std::cin, input);

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

        if (strictFormat && !isValidStrictFormat(input)) {
            printError("Invalid format! Only lowercase letters, numbers, dashes (-), dots (.) and underscores (_) are allowed. No spaces!");
            continue;
        }

        return input;
    }
}

void printStep(const std::string& message) {
    std::cout << BLUE << "[*] " << RESET << message << "...\n";
    sleepMs(400);
}

void printSuccess(const std::string& message) {
    std::cout << GREEN << "[+] " << RESET << BOLD << message << RESET << "\n";
}

void promptMovePackage(const std::string& currentPath) {
    std::cout << "\n";
    std::string question = "Would you like to move the generated package from (" + currentPath + ") to another location? [y/N]";
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

struct AuthMeta {
    std::string license;
    std::string cert;
};

AuthMeta askLicenseAndCert() {
    std::cout << "\n" << BOLD << CYAN << "--- SELECT A LICENSE AND CERTIFICATE ---" << RESET << "\n";
    std::cout << YELLOW << "Software Licenses:" << RESET << "\n";
    std::cout << "  [1] MIT            [6] AGPL v3.0      [11] ISC License\n";
    std::cout << "  [2] Apache 2.0     [7] BSD 3-Clause   [12] The Unlicense\n";
    std::cout << "  [3] GPL v3.0       [8] BSD 2-Clause   [13] CC BY\n";
    std::cout << "  [4] GPL v2.0       [9] MPL 2.0        [14] CC BY-SA\n";
    std::cout << "  [5] LGPL v3.0      [10] EPL 2.0       [15] CC0 (Zero)\n";
    std::cout << "  [" << GREEN << "c" << RESET << "] Custom (Type your own manually)\n";

    std::cout << YELLOW << "\nIT Certifications (Bonus):" << RESET << "\n";
    std::cout << "  [16] LPI Linux Ess.[21] Cisco CCNA    [26] CISSP\n";
    std::cout << "  [17] LPIC-1        [22] AWS Sol. Arch.[27] CEH\n";
    std::cout << "  [18] CompTIA A+    [23] Azure AZ-104  [28] CKA\n";
    std::cout << "  [19] CompTIA Net+  [24] Google IT Sup.[29] PMP\n";
    std::cout << "  [20] CompTIA Sec+  [25] RHCSA         [30] ITIL 4 Found.\n";
    std::cout << "  [" << GREEN << "c" << RESET << "] Custom             [" << RED << "0" << RESET << "] Skip Certification\n\n";

    AuthMeta meta;

    while (true) {
        std::string choice = askInput("Choose Software License (1-15 or 'c')", true, false);
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
        else if (choice == "c" || choice == "C") { meta.license = askInput("Enter Custom License Name (e.g., Proprietary)"); break; }
        else printError("Invalid selection! Please enter a number between 1-15 or 'c'.");
    }

    while (true) {
        std::string choice = askInput("Choose IT Certificate (16-30, 'c', or '0' to skip)", true, false, true);
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
        else printError("Invalid selection! Please enter a number between 16-30, 'c', or '0'.");
    }

    return meta;
}

void buildDebian() {
    std::cout << "\n" << BOLD << CYAN << "--- BUILDING DEBIAN PACKAGE (.deb) ---" << RESET << "\n";

    std::string pkgName = askInput("Package Name", true, true);
    std::string version = askInput("Version", true, true);
    std::string arch = askInput("Architecture", true, true);

    std::string maintainer = askInput("Maintainer");

    AuthMeta authMeta = askLicenseAndCert();

    std::string description = askInput("Description", false);

    if (!authMeta.cert.empty()) {
        if (description.empty()) {
            description = "Packaged by a certified professional: " + authMeta.cert;
        } else {
            description += " [Certified by: " + authMeta.cert + "]";
        }
    }

    std::string binaryPath = askInput("Binary Path");
    std::string installDir = askInput("Install Dir");
    std::string outputDir = askInput("Output Directory");

    fs::create_directories(outputDir);
    std::string buildDir = outputDir + "/" + pkgName + "_" + version + "_" + arch;

    printStep("Creating directory hierarchy in " + outputDir);
    fs::create_directories(buildDir + "/DEBIAN");

    std::string relativeInstallDir = installDir;
    if (relativeInstallDir[0] == '/') relativeInstallDir.erase(0, 1);
    fs::create_directories(buildDir + "/" + relativeInstallDir);

    printStep("Writing control file");
    std::ofstream controlFile(buildDir + "/DEBIAN/control");
    controlFile << "Package: " << pkgName << "\n";
    controlFile << "Version: " << version << "\n";
    controlFile << "Architecture: " << arch << "\n";
    controlFile << "Maintainer: " << maintainer << "\n";
    if (!description.empty()) {
        controlFile << "Description: " << description << "\n";
    }
    controlFile.close();

    printStep("Generating copyright/license file");
    std::string docDir = buildDir + "/usr/share/doc/" + pkgName;
    fs::create_directories(docDir);
    std::ofstream copyrightFile(docDir + "/copyright");
    copyrightFile << "Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/\n";
    copyrightFile << "Upstream-Name: " << pkgName << "\n\n";
    copyrightFile << "Files: *\n";
    copyrightFile << "Copyright: " << maintainer << "\n";
    copyrightFile << "License: " << authMeta.license << "\n";
    copyrightFile.close();

    fs::permissions(docDir + "/copyright", fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read | fs::perms::others_read);

    printStep("Copying binary to target");
    try {
        std::string targetBinary = buildDir + "/" + relativeInstallDir + "/" + pkgName;
        fs::copy_file(binaryPath, targetBinary, fs::copy_options::overwrite_existing);

        fs::permissions(targetBinary, fs::perms::owner_all | fs::perms::group_read | fs::perms::group_exec | fs::perms::others_read | fs::perms::others_exec);
    } catch (const fs::filesystem_error& e) {
        printError("Failed to copy binary! Check the path: " + binaryPath);
        return;
    }

    printStep("Running dpkg-deb");
    std::string buildCmd = "dpkg-deb --build " + buildDir;
    int result = std::system(buildCmd.c_str());

    if (result == 0) {
        printStep("Cleaning up temporary forge files");
        fs::remove_all(buildDir);

        std::string finalDebPath = buildDir + ".deb";
        printSuccess("Debian package successfully built!");

        promptMovePackage(finalDebPath);
    } else {
        printError("dpkg-deb failed during package creation.");
    }
}

void buildRPM() {
    std::cout << "\n" << BOLD << YELLOW << "--- BUILDING FEDORA/RHEL PACKAGE (.rpm) ---" << RESET << "\n";

    std::string pkgName = askInput("Package Name", true, true);
    std::string version = askInput("Version", true, true);
    std::string release = askInput("Release", true, true);

    std::string maintainer = askInput("Maintainer");

    AuthMeta authMeta = askLicenseAndCert();

    std::string description = askInput("Description", false);

    if (!authMeta.cert.empty()) {
        if (description.empty()) {
            description = "Packaged by a certified professional: " + authMeta.cert;
        } else {
            description += " [Certified by: " + authMeta.cert + "]";
        }
    }

    std::string binaryPath = askInput("Binary Path");
    std::string installDir = askInput("Install Dir");
    std::string outputDir = askInput("Output Directory");

    fs::create_directories(outputDir);
    std::string workspace = outputDir + "/" + pkgName + "_rpm_workspace";

    printStep("Preparing RPM workspace in " + outputDir);
    fs::create_directories(workspace + "/BUILD");
    fs::create_directories(workspace + "/RPMS");
    fs::create_directories(workspace + "/SOURCES");
    fs::create_directories(workspace + "/SPECS");
    fs::create_directories(workspace + "/SRPMS");

    printStep("Copying source binary to SOURCES");
    try {
        fs::copy_file(binaryPath, workspace + "/SOURCES/" + pkgName, fs::copy_options::overwrite_existing);
    } catch(const fs::filesystem_error& e) {
        printError("Failed to copy binary to SOURCES! Check the path: " + binaryPath);
        return;
    }

    std::string specPath = workspace + "/SPECS/" + pkgName + ".spec";
    printStep("Writing SPEC file");

    std::ofstream specFile(specPath);
    specFile << "Name:           " << pkgName << "\n";
    specFile << "Version:        " << version << "\n";
    specFile << "Release:        " << release << "%{?dist}\n";
    specFile << "Source0:        %{name}\n";

    if (!description.empty()) {
        specFile << "Summary:        " << description << "\n\n";
    } else {
        specFile << "Summary:        " << pkgName << " package\n\n";
    }

    specFile << "License:        " << authMeta.license << "\n";
    specFile << "Packager:       " << maintainer << "\n\n";

    if (!description.empty()) {
        specFile << "%description\n" << description << "\n\n";
    } else {
        specFile << "%description\n" << pkgName << " package\n\n";
    }

    specFile << "%prep\n";
    specFile << "cp %{SOURCE0} .\n\n";

    specFile << "%build\n";
    specFile << "# Nothing to compile. Pre-compiled binary.\n\n";

    specFile << "%install\n";
    specFile << "mkdir -p %{buildroot}" << installDir << "\n";
    specFile << "install -m 755 %{name} %{buildroot}" << installDir << "/" << pkgName << "\n\n";

    specFile << "%files\n";
    specFile << installDir << "/" << pkgName << "\n";
    specFile.close();

    printStep("Running rpmbuild");
    std::string currentPath = fs::absolute(workspace).string();
    std::string buildCmd = "rpmbuild --define '_topdir " + currentPath + "' -bb " + specPath;

    int result = std::system(buildCmd.c_str());

    if (result == 0) {
        printStep("Extracting RPM and cleaning up workspace");
        bool rpmFound = false;
        std::string finalRpmPath = "";

        if (fs::exists(workspace + "/RPMS")) {
            for (const auto& entry : fs::recursive_directory_iterator(workspace + "/RPMS")) {
                if (entry.is_regular_file() && entry.path().extension() == ".rpm") {
                    try {
                        finalRpmPath = outputDir + "/" + entry.path().filename().string();
                        fs::copy_file(entry.path(), finalRpmPath, fs::copy_options::overwrite_existing);
                        rpmFound = true;
                    } catch (const std::exception& e) {
                        printError(std::string("Failed to copy RPM: ") + e.what());
                    }
                }
            }
        }

        fs::remove_all(workspace);

        if (rpmFound) {
            printSuccess("RPM package successfully built!");
            promptMovePackage(finalRpmPath);
        } else {
            printError("RPM build finished but no .rpm file was found!");
        }
    } else {
        printError("rpmbuild failed. Ensure 'rpm-build' is installed.");
    }
}

void printOutro() {
    std::cout << "\n" << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << GREEN << "  Thank you for using Package Forge!\n" << RESET;
    std::cout << CYAN << "  Made by Neuwj\n" << RESET;
    std::cout << BOLD << BLUE << "=================================================\n\n" << RESET;
}

int main() {
    printBanner();

    std::cout << BOLD << "Which armor (package) do you want to forge, knight?\n" << RESET;
    std::cout << "  [" << GREEN << "1" << RESET << "] Debian / Ubuntu  (" << GREEN << ".deb" << RESET << ")\n";
    std::cout << "  [" << YELLOW << "2" << RESET << "] Fedora / RedHat (" << YELLOW << ".rpm" << RESET << ")\n";
    std::cout << "  [" << CYAN << "0" << RESET << "] Open Example Guide (New Terminal)\n";
    std::cout << "  [" << RED << "q" << RESET << "] Exit\n\n";

    std::string choice;
    while(true) {
        choice = askInput("Choice");
        if (choice == "1") {
            buildDebian();
            break;
        } else if (choice == "2") {
            buildRPM();
            break;
        } else if (choice == "q" || choice == "Q") {
            std::cout << "\nClosing the forge. See you later!\n";
            break;
        } else {
            printError("Invalid choice! Please enter 1, 2, or q.");
        }
    }

    printOutro();
    return 0;
}
