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
#include <csignal>
#include <iomanip>
#include <cstdint>

namespace fs = std::filesystem;

const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string GREEN = "\033[32m";
const std::string BLUE = "\033[34m";
const std::string YELLOW = "\033[33m";
const std::string RED = "\033[31m";
const std::string CYAN = "\033[36m";

volatile sig_atomic_t gSignalStatus = 0;

void signalHandler(int signal) {
    gSignalStatus = signal;
}

class AbortOperation : public std::exception {
public:
    const char* what() const noexcept override {
        return "Operation aborted by user.";
    }
};

struct PackageData {
    std::string binaryPath;
    std::string binaryName;
    std::string pkgName;
    std::string version;
    std::string release;
    std::string arch;
    std::string maintainer;
    std::string license;
    std::string description;
    std::string installDir;
    std::string outputDir;
};

struct SessionCache {
    std::string maintainer;
    std::string outputDir;
    std::string installDir = "/usr/bin";
    std::string arch;
    std::string release = "1";
    std::string license;
};

// FIX: Strip trailing slashes to prevent // path corruption in RPM/DEB build.
void stripTrailingSlashes(std::string& str) {
    while (str.length() > 1 && str.back() == '/') {
        str.pop_back();
    }
}

// FIX: Check if directory is a standard Linux path to prevent RPM from taking ownership of system dirs.
bool isStandardDir(const std::string& dir) {
    std::vector<std::string> standardDirs = {
        "/usr/bin", "/usr/local/bin", "/bin", "/sbin", "/usr/lib", "/usr/lib64", "/opt", "/usr", "/usr/local", "/etc"
    };
    return std::find(standardDirs.begin(), standardDirs.end(), dir) != standardDirs.end();
}

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

void clearScreen() {
    std::system("clear");
}

void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void waitForEnter() {
    std::cout << "\n" << CYAN << "Press [ENTER] to continue..." << RESET;
    std::string dummy;
    gSignalStatus = 0;
    if (!std::getline(std::cin, dummy)) {
        if (gSignalStatus == SIGINT) {
            std::cin.clear();
            std::cout << "\n";
            gSignalStatus = 0;
            throw AbortOperation();
        }
    }
}

void printBanner() {
    clearScreen();
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << YELLOW << "   .===.   " << RESET << BOLD << "PACKAGE FORGE\n";
    std::cout << BOLD << YELLOW << "   | " << CYAN << "*" << YELLOW << " |   " << RESET << "C++ Packaging Wizard | v2.0.4\n";
    std::cout << BOLD << YELLOW << "    \\ /    " << CYAN << "Created by: Neuwj - neuwj@bk.ru\n";
    std::cout << BOLD << YELLOW << "     V     " << RESET << "\n";
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << CYAN << " Tip: Type '!guide' for Help, or press [Ctrl+C] to Abort anytime!\n\n" << RESET;
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
    std::cout << BOLD << RED << "[!] CRITICAL RULE: " << RESET << "This tool packages COMPILED BINARY FILES.\n";
    std::cout << "Do not use source codes (.cpp, .py). Compile your binary first!\n\n";
    std::cout << "\033[1;32mDRAG & DROP SYSTEM:\033[0m\n";
    std::cout << "You can drag multiple binary files into the terminal at once.\n";
    std::cout << "The program will parse them even if they contain spaces or backslashes.\n";
    std::cout << "Once done, type 'confirm' to start the configuration.\n\n";
    std::cout << "\033[1;33m[?] Tips:\033[0m\n";
    std::cout << "  -> Press [Ctrl+C] anytime to return to the main menu.\n";
    std::cout << "  -> Most fields are cached after the first entry for speed.\n\n";
    std::cout << "\033[1;32mPress [ENTER] to close and continue...\033[0m\n";
    std::string dummy;
    gSignalStatus = 0;
    if (!std::getline(std::cin, dummy)) {
        if (gSignalStatus == SIGINT) {
            std::cin.clear();
            std::cout << "\n";
            gSignalStatus = 0;
            throw AbortOperation();
        }
    }
}

std::string askInput(const std::string& question, bool isRequired = true, bool strictFormat = false, bool ignoreGuideKey = false, bool checkExists = false, const std::string& defaultValue = "") {
    std::string input;
    while (true) {
        if (!defaultValue.empty()) {
            std::cout << YELLOW << "[?] " << RESET << BOLD << question << " (Default: " << defaultValue << "): " << RESET;
        } else {
            std::cout << YELLOW << "[?] " << RESET << BOLD << question << ": " << RESET;
        }

        gSignalStatus = 0;
        if (!std::getline(std::cin, input)) {
            if (gSignalStatus == SIGINT) {
                std::cin.clear();
                std::cout << "\n";
                gSignalStatus = 0;
                throw AbortOperation();
            }
            std::exit(0);
        }

        input = trimPath(input);
        std::string loweredInput = toLower(input);

        if (loweredInput == "!cancel") throw AbortOperation();
        if (loweredInput == "!guide" && !ignoreGuideKey) { openGuide(); continue; }

        if (input.empty()) {
            if (!defaultValue.empty()) return defaultValue;
            if (isRequired) { printError("Field required."); continue; }
            return "";
        }

        input = expandTilde(input);
        if (strictFormat && !isValidStrictFormat(input)) {
            printError("Invalid format! No spaces, lowercase only.");
            continue;
        }
        if (checkExists && !fs::is_regular_file(input)) {
            printError("File not found: " + input);
            continue;
        }
        return input;
    }
}

std::string askLicense(const std::string& contextName, SessionCache& cache) {
    std::cout << "\n" << BOLD << CYAN << "--- LICENSE FOR " << contextName << " ---" << RESET << "\n";

    const char* licenses[30] = {
        "MIT", "Apache-2.0", "GPL-3.0-only", "GPL-3.0-or-later", "GPL-2.0-only",
        "LGPL-3.0-only", "LGPL-2.1-only", "AGPL-3.0-only", "BSD-3-Clause", "BSD-2-Clause",
        "MPL-2.0", "EPL-2.0", "Unlicense", "CC0-1.0", "CC-BY-4.0",
        "CC-BY-SA-4.0", "ISC", "Zlib", "Artistic-2.0", "BSL-1.0",
        "CDDL-1.0", "EUPL-1.2", "OFL-1.1", "OSL-3.0", "PostgreSQL",
        "MS-PL", "MS-RL", "CERN-OHL-S-2.0", "Proprietary", "Unknown"
    };

    for (int i = 0; i < 30; i += 3) {
        std::cout << " " << std::right << std::setw(2) << i + 1 << ") " << std::left << std::setw(17) << licenses[i]
        << " " << std::right << std::setw(2) << i + 2 << ") " << std::left << std::setw(17) << licenses[i+1]
        << " " << std::right << std::setw(2) << i + 3 << ") " << std::left << std::setw(17) << licenses[i+2] << "\n";
    }
    std::cout << "  c) Custom\n\n";

    while (true) {
        std::string choice = askInput("Choose License (1-30 or 'c')", true, false, true, false, cache.license);
        std::string lowerChoice = toLower(choice);

        if (!cache.license.empty() && choice == cache.license) {
            return cache.license;
        }

        if (lowerChoice == "c") {
            cache.license = askInput("License Name");
            return cache.license;
        }

        try {
            int idx = std::stoi(choice);
            if (idx >= 1 && idx <= 30) {
                cache.license = licenses[idx - 1];
                return cache.license;
            }
        } catch (...) {}

        printError("Invalid choice.");
    }
}

PackageData gatherPackageInfo(const std::string& binaryPath, bool needDeb, bool needRpm, SessionCache& cache) {
    PackageData data;
    data.binaryPath = binaryPath;
    data.binaryName = fs::path(binaryPath).filename().string();
    std::cout << "\n" << BOLD << CYAN << "=== DATA ENTRY: " << data.binaryName << " ===" << RESET << "\n";

    // FIX: Debian rules require package names to start with an alphanumeric character.
    while (true) {
        data.pkgName = askInput("Package Name", true, true);
        std::replace(data.pkgName.begin(), data.pkgName.end(), '_', '-');
        if (data.pkgName.empty() || !std::isalpha(data.pkgName[0]) || data.pkgName.length() < 2) {
            printError("Package name must start with a letter and be at least 2 characters long.");
            continue;
        }
        break;
    }

    // FIX: Debian rules require version numbers to start with a digit.
    while (true) {
        data.version = askInput("Version", true, true);
        std::replace(data.version.begin(), data.version.end(), '_', '.');
        std::replace(data.version.begin(), data.version.end(), '-', '.');
        if (data.version.empty() || !std::isdigit(data.version[0])) {
            printError("Version must start with a digit (e.g., 1.0.0).");
            continue;
        }
        break;
    }

    if (needDeb) {
        data.arch = askInput("Architecture", true, true, false, false, cache.arch);
        if (data.arch == "x86_64") data.arch = "amd64";
        else if (data.arch == "aarch64") data.arch = "arm64"; // FIX: Map aarch64 to arm64 for Debian compatibility
        cache.arch = data.arch;
    }
    if (needRpm) {
        data.release = askInput("RPM Release", true, true, false, false, cache.release);
        cache.release = data.release;
    }
    data.maintainer = askInput("Maintainer (Name <email>)", true, false, false, false, cache.maintainer);
    cache.maintainer = data.maintainer;
    data.license = askLicense(data.binaryName, cache);
    data.description = askInput("Description", false);

    if (data.description.empty()) {
        data.description = "No description provided.";
    }

    data.installDir = askInput("Install Dir", true, false, false, false, cache.installDir);
    stripTrailingSlashes(data.installDir);
    cache.installDir = data.installDir;
    if (!data.installDir.empty() && data.installDir[0] != '/') data.installDir = "/" + data.installDir;

    data.outputDir = askInput("Output Dir", true, false, false, false, cache.outputDir);
    stripTrailingSlashes(data.outputDir);
    cache.outputDir = data.outputDir;

    return data;
}

void buildDebian(const PackageData& data) {
    std::string safePkgName = data.pkgName;
    std::string safeVersion = data.version;
    std::cout << "\n" << BOLD << CYAN << "--- BUILDING DEB: " << safePkgName << " ---" << RESET << "\n";

    // FIX: Ensure Output Directory actually exists before doing anything!
    try { fs::create_directories(data.outputDir); } catch (...) {}

    std::string relativeInstallDir = data.installDir;
    if (!relativeInstallDir.empty() && relativeInstallDir[0] == '/') relativeInstallDir.erase(0, 1);
    std::string buildDir = data.outputDir + "/" + safePkgName + "_" + safeVersion + "_" + data.arch;
    try {
        fs::create_directories(buildDir + "/DEBIAN");
        fs::create_directories(buildDir + "/" + relativeInstallDir);

        // FIX: Calculate Installed-Size for APT package manager metadata.
        std::uintmax_t fileSize = fs::file_size(data.binaryPath);
        std::uintmax_t installedSizeKB = (fileSize / 1024) + 1;

        std::ofstream control(buildDir + "/DEBIAN/control");
        // Added double newline at the end of control file to prevent EOF parsing bugs.
        control << "Package: " << safePkgName << "\nVersion: " << safeVersion << "\nArchitecture: " << data.arch
        << "\nMaintainer: " << data.maintainer << "\nInstalled-Size: " << installedSizeKB
        << "\nDescription: " << data.description << "\n\n";
        control.close();

        std::string target = buildDir + "/" + relativeInstallDir + "/" + data.pkgName;
        fs::copy_file(data.binaryPath, target, fs::copy_options::overwrite_existing);
        fs::permissions(target, fs::perms::owner_all | fs::perms::group_read | fs::perms::group_exec | fs::perms::others_read | fs::perms::others_exec);
        std::string finalPath = data.outputDir + "/" + safePkgName + "_" + safeVersion + "_" + data.arch + ".deb";

        int ret = std::system(("dpkg-deb --build " + escapeShellArg(buildDir) + " " + escapeShellArg(finalPath)).c_str());
        fs::remove_all(buildDir);

        if (ret == 0) {
            printSuccess("DEB Created: " + finalPath);
        } else {
            printError("DEB build failed. Check if dpkg-deb is installed.");
        }
    } catch (...) { printError("DEB build failed."); fs::remove_all(buildDir); }
}

void buildRPM(const PackageData& data) {
    std::cout << "\n" << BOLD << YELLOW << "--- BUILDING RPM: " << data.pkgName << " ---" << RESET << "\n";

    // FIX: Ensure Output Directory actually exists!
    try { fs::create_directories(data.outputDir); } catch (...) {}

    // FIX: Use system's true temp directory instead of hardcoded /tmp for maximum compatibility/security.
    std::string timestamp = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    std::string workspace = (fs::temp_directory_path() / (data.pkgName + "_rpm_workspace_" + timestamp)).string();

    try {
        fs::create_directories(workspace + "/SOURCES");
        fs::create_directories(workspace + "/SPECS");
        fs::copy_file(data.binaryPath, workspace + "/SOURCES/" + data.binaryName, fs::copy_options::overwrite_existing);
        std::string spec = workspace + "/SPECS/" + data.pkgName + ".spec";
        std::ofstream specF(spec);

        specF << "Name: " << data.pkgName << "\nVersion: " << data.version << "\nRelease: " << data.release
        << "\nSummary: " << data.description << "\nLicense: " << data.license << "\nPackager: " << data.maintainer
        << "\n%description\n" << data.description << "\n%install\nmkdir -p \"%{buildroot}" << data.installDir
        << "\"\ninstall -m 755 \"%{_sourcedir}/" << data.binaryName << "\" \"%{buildroot}" << data.installDir
        << "/" << data.pkgName << "\"\n%files\n";

        // FIX: Prevent orphan directories on uninstall by taking ownership of custom install directories.
        if (!isStandardDir(data.installDir)) {
            specF << "%dir \"" << data.installDir << "\"\n";
        }

        specF << "\"" << data.installDir << "/" << data.pkgName << "\"\n\n";
        specF.close();

        std::string defineArg = "_topdir " + (std::string)fs::absolute(workspace);
        if (std::system(("rpmbuild --define " + escapeShellArg(defineArg) + " -bb " + escapeShellArg(spec)).c_str()) == 0) {
            for (const auto& entry : fs::recursive_directory_iterator(workspace + "/RPMS")) {
                if (entry.path().extension() == ".rpm") {
                    fs::copy_file(entry.path(), data.outputDir + "/" + entry.path().filename().string(), fs::copy_options::overwrite_existing);
                }
            }
            printSuccess("RPM Created in: " + data.outputDir);
        } else {
            printError("RPM build failed. Check spec or rpmbuild tools.");
        }
        fs::remove_all(workspace);
    } catch (...) { printError("RPM build failed."); fs::remove_all(workspace); }
}

std::vector<std::string> extractPaths(const std::string& input) {
    std::vector<std::string> paths;
    std::string current = "";
    bool inQuotes = false, escaped = false;
    for (char c : input) {
        if (escaped) { current += c; escaped = false; }
        else if (c == '\\') escaped = true;
        else if (c == '"' || c == '\'') inQuotes = !inQuotes;
        else if (!inQuotes && std::isspace(c)) { if (!current.empty()) { paths.push_back(current); current = ""; } }
        else current += c;
    }
    if (!current.empty()) paths.push_back(current);
    return paths;
}

void forgePackages() {
    clearScreen();
    std::cout << BOLD << BLUE << "=================================================\n" << "      FORGE PACKAGES (DRAG & DROP)\n" << "=================================================\n\n" << RESET;

    std::string type;
    while (true) {
        type = askInput("Type [1:DEB, 2:RPM, 3:Both]", true);
        if (type == "1" || type == "2" || type == "3") break;
        printError("Invalid choice. Please enter 1, 2, or 3.");
    }

    bool bDeb = (type == "1" || type == "3"), bRpm = (type == "2" || type == "3");
    std::vector<std::string> binaryPaths;
    while (true) {
        clearScreen();
        std::cout << BOLD << CYAN << "--- FILES ---\033[40G" << RED << "(!cancel to return to main menu)\n" << RESET;
        for (const auto& p : binaryPaths) std::cout << "  - " << p << "\n";
        std::string input = askInput("\nDrag file(s) or type 'confirm'", true);
        if (toLower(input) == "confirm") { if (binaryPaths.empty()) continue; break; }
        for (std::string p : extractPaths(input)) {
            p = expandTilde(trimPath(p));
            if (fs::is_regular_file(p)) binaryPaths.push_back(p);
            else printError("Invalid file: " + p);
        }
        sleepMs(500);
    }
    SessionCache cache;
    std::vector<PackageData> packages;
    for (const auto& p : binaryPaths) packages.push_back(gatherPackageInfo(p, bDeb, bRpm, cache));
    for (const auto& d : packages) { if (bDeb) buildDebian(d); if (bRpm) buildRPM(d); }
    printSuccess("Forge complete!");
}

int main() {
    std::signal(SIGINT, signalHandler);
    while(true) {
        try {
            printBanner();
            std::cout << "  [1] Forge Packages (Drag & Drop)\n  [2] Install Tools\n  [0] Guide\n  [q] Quit\n\n";
            std::string choice = toLower(askInput("Your Choice", true));
            if (choice == "1") forgePackages();
            else if (choice == "2") std::system("sudo apt update && sudo apt install -y dpkg rpm rpm-build");
            else if (choice == "0") openGuide();
            else if (choice == "q") break;
            waitForEnter();
        } catch (const AbortOperation&) {
            std::cout << "\n" << RED << "[!] Aborted. Back to menu..." << RESET << "\n";
            sleepMs(1000);
        }
    }
    return 0;
}
