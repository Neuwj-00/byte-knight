#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <csignal>

namespace fs = std::filesystem;

const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string GREEN = "\033[32m";
const std::string BLUE = "\033[34m";
const std::string YELLOW = "\033[33m";
const std::string RED = "\033[31m";
const std::string CYAN = "\033[36m";

void clearScreen() {
    #ifdef _WIN32
    std::system("cls");
    #else
    std::system("clear");
    #endif
}

void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void waitForEnter() {
    std::cout << "\n" << CYAN << "Press [ENTER] to continue..." << RESET;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void showExitMessage() {
    clearScreen();
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << GREEN << "  Thank you for using Repo Forge - Nexus!\n" << RESET;
    std::cout << BOLD << CYAN << "  Architect: Neuwj\n" << RESET;
    std::cout << BOLD << CYAN << "  Contact:   neuwj@example.com\n" << RESET;
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << YELLOW << "  Stay safe.\n\n" << RESET;
}

void handleSigint(int sig) {
    showExitMessage();
    std::exit(0);
}

bool isDirEmpty(const std::string& path) {
    try {
        if (!fs::exists(path)) return true;
        return fs::directory_iterator(path) == fs::directory_iterator();
    } catch (...) {
        return true;
    }
}

std::string cleanPath(std::string path) {
    if (path.empty()) return path;

    size_t first = path.find_first_not_of(" \t\'\"");
    if (first == std::string::npos) return "";

    path.erase(0, first);
    path.erase(path.find_last_not_of(" \t\'\"") + 1);

    if (path.length() > 0 && path[0] == '~') {
        const char* home = std::getenv("HOME");
        if (!home) home = std::getenv("USERPROFILE");
        if (home) {
            path.replace(0, 1, std::string(home));
        }
    }

    while (path.length() > 1 && path.back() == '/') {
        path.pop_back();
    }

    return path;
}

void printBanner() {
    clearScreen();
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << YELLOW << "   .===.   " << RESET << BOLD << "REPO FORGE\n";
    std::cout << BOLD << YELLOW << "   | " << CYAN << "*" << YELLOW << " |   " << RESET << "Automated Repository Indexer | v0.0.8\n";
    std::cout << BOLD << YELLOW << "    \\ /    " << CYAN << "Made by Neuwj - neuwj@bk.ru\n";
    std::cout << BOLD << YELLOW << "     V     " << RESET << "\n";
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << CYAN << " System: APT (Debian/Ubuntu) & DNF (Fedora/RPM) Ready!\n\n" << RESET;
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

void showGuide() {
    clearScreen();
    std::cout << BOLD << CYAN << "============================================================\n" << RESET;
    std::cout << BOLD << CYAN << "                  REPO FORGE - USER GUIDE             \n" << RESET;
    std::cout << BOLD << CYAN << "============================================================\n\n" << RESET;

    std::cout << BOLD << GREEN << "[1] CREATE REPO SKELETON (Init)\n" << RESET;
    std::cout << "    Creates debian/pool/main and rpm/ subdirectories.\n\n";

    std::cout << BOLD << YELLOW << "    -> Where to drop your packages?\n" << RESET;
    std::cout << "       .deb: " << CYAN << "repo_folder/debian/pool/main\n" << RESET;
    std::cout << "       .rpm: " << CYAN << "repo_folder/rpm\n\n" << RESET;

    std::cout << BOLD << GREEN << "[2] SCAN AND INDEX REPOSITORY\n" << RESET;
    std::cout << "    Scans the folders and generates index files (Packages, repodata).\n\n";

    std::cout << BOLD << GREEN << "[3] INSTALL REQUIRED TOOLS\n" << RESET;
    std::cout << "    Installs 'dpkg-dev', 'apt-utils', and 'createrepo-c'.\n" ;
    std::cout << "    MUST be run if you get 'command not found' errors.\n\n";

    std::cout << BOLD << BLUE << "============================================================\n" << RESET;

    std::string input;
    while (true) {
        std::cout << YELLOW << "Type " << BOLD << RED << "'q'" << YELLOW << " and [ENTER] to return: " << RESET;
        std::getline(std::cin, input);
        if (input == "q" || input == "Q") break;
    }
}

std::string askInput(const std::string& question, bool isRequired = true, bool isPath = false) {
    std::string input;
    while (true) {
        std::cout << YELLOW << "[?] " << RESET << BOLD << question << ": " << RESET;
        std::getline(std::cin, input);

        if (isPath) {
            input = cleanPath(input);
        } else {
            size_t first = input.find_first_not_of(" \t");
            if (first != std::string::npos) {
                input.erase(0, first);
                input.erase(input.find_last_not_of(" \t") + 1);
            } else {
                input = "";
            }
        }

        if (input.empty()) {
            if (isRequired) {
                printError("Field cannot be empty. Please enter a valid value.");
                continue;
            } else {
                return "";
            }
        }

        return input;
    }
}

void initRepo() {
    std::cout << "\n" << BOLD << CYAN << "--- BUILDING REPOSITORY SKELETON ---" << RESET << "\n";
    std::string repoDir = askInput("Repo Root Directory (e.g., ~/myrepo)", true, true);

    try {
        bool debCreated = fs::create_directories(repoDir + "/debian/pool/main");
        bool rpmCreated = fs::create_directories(repoDir + "/rpm");
        bool keysCreated = fs::create_directories(repoDir + "/keys");

        std::ofstream gitignore(repoDir + "/.gitignore");
        if (gitignore.is_open()) {
            gitignore << "# Security\n*.sec\n*.sk\n*private*.key\n\n# Temp\n*.log\n*.tmp\n.repodata.old/\n";
            gitignore.close();
        }

        if (debCreated || rpmCreated || keysCreated) {
            printSuccess("Skeleton created at: " + repoDir);
        } else {
            printSuccess("Skeleton verified (directories already exist) at: " + repoDir);
        }
    } catch (const std::exception& e) {
        printError(std::string("File system error: ") + e.what());
    }
    waitForEnter();
}

void indexRepo() {
    std::cout << "\n" << BOLD << CYAN << "--- SCANNING & INDEXING ENGINE ---" << RESET << "\n";
    std::string repoDir = askInput("Repo Root Directory", true, true);

    if (!fs::exists(repoDir)) {
        printError("Directory not found!");
        waitForEnter();
        return;
    }

    std::string debianPool = repoDir + "/debian/pool/main";
    std::string rpmBase = repoDir + "/rpm";

    if (fs::exists(debianPool)) {
        if (isDirEmpty(debianPool)) {
            std::cout << YELLOW << "[!] Debian pool is empty, skipping indexing.\n" << RESET;
        } else {
            printStep("Indexing Debian packages");
            std::string codename = askInput("Debian Codename (Leave empty for 'stable')", false);
            if(codename.empty()) codename = "stable";

            std::string arch = askInput("Debian Architecture (Leave empty for 'amd64')", false);
            if(arch.empty()) arch = "amd64";

            std::string distsDir = repoDir + "/debian/dists/" + codename + "/main/binary-" + arch;
            fs::create_directories(distsDir);

            std::string cmd = "cd \"" + repoDir + "/debian\" && dpkg-scanpackages pool/main /dev/null > \"dists/" + codename + "/main/binary-" + arch + "/Packages\"";
            if (std::system((cmd + " 2>/dev/null").c_str()) == 0) {
                std::system(("gzip -k -f \"" + distsDir + "/Packages\"").c_str());

                std::string releaseCmd = "cd \"" + repoDir + "/debian\" && apt-ftparchive "
                "-o APT::FTPArchive::Release::Origin=\"RepoForge\" "
                "-o APT::FTPArchive::Release::Label=\"RepoForge\" "
                "-o APT::FTPArchive::Release::Suite=\"" + codename + "\" "
                "-o APT::FTPArchive::Release::Codename=\"" + codename + "\" "
                "-o APT::FTPArchive::Release::Architectures=\"" + arch + "\" "
                "release \"dists/" + codename + "\" > \"dists/" + codename + "/Release\"";
                std::system((releaseCmd + " 2>/dev/null").c_str());

                printSuccess("Debian index updated.");
            } else {
                printError("dpkg-scanpackages failed. Is 'dpkg-dev' installed? (Run Option 3)");
            }
        }
    }

    if (fs::exists(rpmBase)) {
        if (isDirEmpty(rpmBase)) {
            std::cout << YELLOW << "[!] RPM folder is empty, skipping indexing.\n" << RESET;
        } else {
            printStep("Indexing RPM packages");
            if (std::system(("createrepo_c \"" + rpmBase + "\" > /dev/null 2>&1 || createrepo \"" + rpmBase + "\" > /dev/null 2>&1").c_str()) == 0) {
                printSuccess("RPM index updated.");
            } else {
                printError("createrepo failed. Is 'createrepo-c' installed? (Run Option 3)");
            }
        }
    }

    waitForEnter();
}

void installTools() {
    std::cout << "\n" << BOLD << CYAN << "--- INSTALLING TOOLS ---" << RESET << "\n";

    bool hasApt = (std::system("command -v apt > /dev/null 2>&1") == 0) || (std::system("command -v apt-get > /dev/null 2>&1") == 0);
    bool hasDnf = (std::system("command -v dnf > /dev/null 2>&1") == 0);
    bool hasYum = (std::system("command -v yum > /dev/null 2>&1") == 0);

    if (hasApt) {
        printStep("Debian/Ubuntu detected. Installing via APT");
        if (std::system("sudo apt update && sudo apt install -y dpkg-dev apt-utils createrepo-c") == 0) {
            printSuccess("Tools successfully installed!");
        } else {
            printError("Installation failed. Please check your internet connection or sudo privileges.");
        }
    } else if (hasDnf) {
        printStep("Fedora/RHEL detected. Installing via DNF");
        if (std::system("sudo dnf install -y createrepo_c dpkg") == 0) {
            printSuccess("Tools successfully installed!");
        } else {
            printError("Installation failed.");
        }
    } else if (hasYum) {
        printStep("CentOS/Older RHEL detected. Installing via YUM");
        if (std::system("sudo yum install -y createrepo_c dpkg") == 0) {
            printSuccess("Tools successfully installed!");
        } else {
            printError("Installation failed.");
        }
    } else {
        printError("Supported Package Manager (APT, DNF, YUM) not found.");
        std::cout << YELLOW << "Please install 'dpkg-dev' and 'createrepo-c' manually for your system.\n" << RESET;
    }

    waitForEnter();
}

int main() {
    std::signal(SIGINT, handleSigint);
    while(true) {
        printBanner();
        std::cout << "  [1] Create Repo Skeleton (Init)\n";
        std::cout << "  [2] Scan and Index Repository\n";
        std::cout << "  [3] Install Required Tools\n";
        std::cout << "  [0] User Guide\n";
        std::cout << "  [q] Exit\n\n";

        std::string choice = askInput("Choice", true, false);
        if (choice == "1") initRepo();
        else if (choice == "2") indexRepo();
        else if (choice == "3") installTools();
        else if (choice == "0") showGuide();
        else if (choice == "q" || choice == "Q") {
            showExitMessage();
            break;
        } else {
            printError("Invalid choice.");
            sleepMs(1000);
        }
    }
    return 0;
}
