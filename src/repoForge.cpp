#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <csignal>
#include <array>
#include <memory>
#include <cstdio>
#include <atomic>
#include <map>
#include <future>
#include <mutex>
#include <sstream>
#include <algorithm>
#include <sys/wait.h>

namespace fs = std::filesystem;

const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string GREEN = "\033[32m";
const std::string BLUE = "\033[34m";
const std::string YELLOW = "\033[33m";
const std::string RED = "\033[31m";
const std::string CYAN = "\033[36m";

static std::atomic<bool> g_should_exit{false};
std::mutex g_print_mutex;

struct AppConfig {
    std::string last_repo_dir;
    std::string last_gpg_key;
    std::string last_aur_email;
};

AppConfig g_config;

std::string get_config_path() {
    const char* home = std::getenv("HOME");
    if (home) return (fs::path(home) / ".repoforge_config").string();
    return ".repoforge_config";
}

void load_config() {
    std::ifstream file(get_config_path());
    if (file.is_open()) {
        std::getline(file, g_config.last_repo_dir);
        std::getline(file, g_config.last_gpg_key);
        std::getline(file, g_config.last_aur_email);
    }
}

void save_config() {
    std::ofstream file(get_config_path());
    if (file.is_open()) {
        file << g_config.last_repo_dir << "\n";
        file << g_config.last_gpg_key << "\n";
        file << g_config.last_aur_email << "\n";
    }
}

void clear_screen() {
    std::system("clear");
}

void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void wait_for_enter() {
    std::cout << "\n" << CYAN << "Press [ENTER] to continue..." << RESET;
    std::string dummy;
    if (!std::getline(std::cin, dummy)) std::exit(0);
}

void show_exit_message() {
    save_config();
    clear_screen();
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << GREEN << "  Thank you for using Repo Forge\n" << RESET;
    std::cout << BOLD << CYAN << "  Developer: Neuwj\n" << RESET;
    std::cout << BOLD << CYAN << "  Contact:   neuwj@bk.ru\n" << RESET;
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << YELLOW << "  Stay safe.\n\n" << RESET;
}

void handle_sigint(int sig) {
    g_should_exit.store(true);
}

struct cmd_result {
    int exit_status;
    std::string output;
};

cmd_result execute_command(const std::string& cmd, bool mix_stderr = true) {
    std::array<char, 256> buffer;
    std::string result;
    std::string full_cmd = mix_stderr ? (cmd + " 2>&1") : cmd;

    FILE* pipe = popen(full_cmd.c_str(), "r");
    if (!pipe) return {-1, "popen() call failed!"};

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result += buffer.data();
    }

    int status = pclose(pipe);
    int exit_status = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

    if (!result.empty() && result.back() == '\n') result.pop_back();
    return {exit_status, result};
}

std::string clean_path(std::string path) {
    if (path.empty()) return path;
    size_t first = path.find_first_not_of(" \t\'\"");
    if (first == std::string::npos) return "";
    path.erase(0, first);
    path.erase(path.find_last_not_of(" \t\'\"") + 1);

    if (path.length() > 0 && path[0] == '~') {
        const char* home = std::getenv("HOME");
        if (home) {
            path.replace(0, 1, std::string(home));
        }
    }

    try {
        fs::path p(path);
        return p.lexically_normal().string();
    } catch (...) {
        return path;
    }
}

std::string escape_shell_arg(const std::string& arg) {
    std::string escaped = "'";
    for (char c : arg) {
        if (c == '\'') escaped += "'\\''";
        else escaped += c;
    }
    escaped += "'";
    return escaped;
}

std::string sanitize_pkg_field(const std::string& input) {
    std::string sanitized;
    for (char c : input) {
        if (c == '"' || c == '\'' || c == '`' || c == '$' ||
            c == '\\' || c == ';' || c == '&' || c == '|' ||
            c == '(' || c == ')' || c == '{' || c == '}' ||
            c == '<' || c == '>' || c == '\n' || c == '\r') {
            continue;
            }
            sanitized += c;
    }
    return sanitized;
}

bool is_valid_pkg_name(const std::string& name) {
    if (name.empty()) return false;
    for (char c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_' && c != '+' && c != '.') return false;
    }
    return true;
}

const std::map<std::string, std::string> LICENSE_MAP = {
    {"1", "GPL"},     {"2", "GPL3"},    {"3", "MIT"},
    {"4", "Apache"},  {"5", "BSD"},     {"6", "ISC"},
    {"7", "Unlicense"}, {"8", "Python"}, {"9", "Ruby"},
    {"10", "Zlib"}
};

void print_banner() {
    clear_screen();
    std::cout << BOLD << BLUE << "=================================================\n" << RESET;
    std::cout << BOLD << YELLOW << "   .===.   " << RESET << BOLD << "repoForge\n";
    std::cout << BOLD << YELLOW << "   | " << CYAN << "*" << YELLOW << " |   " << RESET << "Automated Repository Indexer | v0.1.9\n";
    std::cout << BOLD << YELLOW << "    \\ /    " << CYAN << "Made by Neuwj - neuwj@bk.ru\n";
    std::cout << BOLD << YELLOW << "     V     " << RESET << "\n";
    std::cout << BOLD << BLUE << "=================================================\n" <<  RESET;
}

void print_error(const std::string& message) {
    std::cout << RED << "[-] ERROR: " << RESET << message << "\n";
}

void print_step(const std::string& message) {
    std::cout << BLUE << "[*] " << RESET << message << "...\n";
    sleep_ms(400);
}

void print_success(const std::string& message) {
    std::cout << GREEN << "[+] " << RESET << BOLD << message << RESET << "\n";
}

std::string ask_input(const std::string& question, bool is_required = true, bool is_path = false, const std::string& default_val = "") {
    std::string input;
    while (true) {
        if (!default_val.empty()) {
            std::cout << YELLOW << question << " [" << default_val << "]: " << RESET;
        } else {
            std::cout << YELLOW << question << ": " << RESET;
        }

        if (!std::getline(std::cin, input)) {
            std::exit(0);
        }

        if (input.empty() && !default_val.empty()) {
            input = default_val;
        }

        if (is_path) input = clean_path(input);
        else {
            size_t first = input.find_first_not_of(" \t");
            if (first != std::string::npos) {
                input.erase(0, first);
                input.erase(input.find_last_not_of(" \t") + 1);
            } else input = "";
        }

        if (input.empty() && is_required) {
            print_error("This field cannot be empty.");
            continue;
        }
        return input;
    }
}

void handle_aur() {
    clear_screen();
    std::cout << BOLD << CYAN << "============================================================\n" << RESET;
    std::cout << BOLD << CYAN << "                 AUR AUTOMATION ENGINE                  \n" << RESET;
    std::cout << BOLD << CYAN << "============================================================\n\n" << RESET;

    const char* home = std::getenv("HOME");
    if (!home) {
        print_error("HOME environment variable not found!");
        wait_for_enter();
        return;
    }

    std::string ssh_dir = (fs::path(home) / ".ssh").string();
    std::string pub_key_path = "";

    print_step("Scanning for SSH Keys");
    if (fs::exists(fs::path(ssh_dir) / "id_ed25519.pub")) pub_key_path = (fs::path(ssh_dir) / "id_ed25519.pub").string();
    else if (fs::exists(fs::path(ssh_dir) / "id_rsa.pub")) pub_key_path = (fs::path(ssh_dir) / "id_rsa.pub").string();

    if (pub_key_path.empty()) {
        print_error("No suitable SSH key found on your system.");
        std::string new_email = ask_input("Enter your email to generate an SSH key", true, false, g_config.last_aur_email);
        g_config.last_aur_email = new_email;

        std::cout << YELLOW << "\nPlease run the following command in another terminal:\n" << RESET;
        std::cout << "  " << BOLD << "ssh-keygen -t ed25519 -C \"" << new_email << "\"\n" << RESET;
        std::cout << YELLOW << "Then add your public key to your AUR profile and run this engine again.\n" << RESET;
        wait_for_enter();
        return;
    }

    std::cout << GREEN << "[+] Key Found: " << pub_key_path << RESET << "\n";
    std::cout << "\n" << BOLD << BLUE << "--- AUR ACCOUNT NOTIFICATION ---\n" << RESET;
    cmd_result key_read = execute_command("cat " + escape_shell_arg(pub_key_path));
    std::cout << "Please copy the following key and add it to your AUR (My Account) page:\n\n";
    std::cout << BOLD << key_read.output << RESET << "\n\n";
    ask_input(CYAN + "Press [ENTER] if you have added it or it's already present" + RESET, false);

    print_step("Testing AUR connection");
    cmd_result ssh_test = execute_command("ssh -o StrictHostKeyChecking=no -T aur@aur.archlinux.org");
    if (ssh_test.output.find("successfully authenticated") == std::string::npos) {
        std::cout << YELLOW << "[!] Warning: Connection could not be fully verified, but proceeding...\n" << RESET;
    }

    std::cout << "\n" << BOLD << BLUE << "--- PACKAGE METADATA ENTRY ---\n" << RESET;
    std::string pkgname = ask_input("Package Name (pkgname)");
    if (!is_valid_pkg_name(pkgname)) {
        print_error("Invalid package name! Only alphanumeric characters, '-', '_', '+', and '.' are allowed.");
        wait_for_enter();
        return;
    }

    print_step("Cloning AUR repository for " + pkgname);
    std::string clone_cmd = "git clone ssh://aur@aur.archlinux.org/" + escape_shell_arg(pkgname + ".git");
    cmd_result clone_res = execute_command(clone_cmd);
    if (clone_res.exit_status != 0) {
        print_error("Git clone failed! Check your internet connection or SSH permissions.\nDetails: " + clone_res.output);
        wait_for_enter();
        return;
    }

    std::string pkgver = ask_input("Version (pkgver)");
    std::string pkgdesc = ask_input("Short Description (pkgdesc)");
    std::string url = ask_input("Project Homepage (url)");
    std::string depends_raw = ask_input("Dependencies (depends) [e.g., 'gcc-libs glibc' - separate with spaces]", false);
    std::string source = ask_input("Source Code Link (.tar.gz/.zip etc.) (source)");

    std::vector<std::string> dep_list;
    std::stringstream ss(depends_raw);
    std::string dep;
    while(ss >> dep) {
        dep.erase(std::remove(dep.begin(), dep.end(), '\''), dep.end());
        dep.erase(std::remove(dep.begin(), dep.end(), '\"'), dep.end());
        if(!dep.empty()) dep_list.push_back(dep);
    }

    std::cout << "\n" << BOLD << CYAN << "--- ARCHITECTURE SELECTION ---\n" << RESET;
    std::cout << "  [1] x86_64  (64-bit, most common)\n";
    std::cout << "  [2] any     (Platform independent — scripts, data files)\n";
    std::cout << "  [3] aarch64 (ARM 64-bit)\n";
    std::string arch_choice = ask_input("Architecture");
    std::string arch = "x86_64";
    if (arch_choice == "2") arch = "any";
    else if (arch_choice == "3") arch = "aarch64";

    std::cout << "\n" << BOLD << CYAN << "--- LICENSE SELECTION ---\n" << RESET;
    std::vector<std::string> licenses = {"GPL", "GPL3", "MIT", "Apache", "BSD", "ISC", "Unlicense", "Python", "Ruby", "Zlib"};
    for (size_t i = 0; i < licenses.size(); ++i) {
        std::cout << "  [" << i + 1 << "] " << licenses[i] << "  ";
        if ((i + 1) % 5 == 0) std::cout << "\n";
    }
    std::cout << "\n";
    std::string license_choice = ask_input("License (enter number 1-10 or custom name)");
    std::string license = (LICENSE_MAP.count(license_choice) ? LICENSE_MAP.at(license_choice) : license_choice);

    std::cout << "\n" << BOLD << CYAN << "--- PROJECT TYPE SELECTION ---\n" << RESET;
    std::cout << "  [1] C/C++ (Build from source with g++)\n";
    std::cout << "  [2] Python (Script copying)\n";
    std::cout << "  [3] Precompiled / Binary (Simply move binary to destination)\n";
    std::string build_type = ask_input("Project Type");

    std::string safe_pkgdesc = sanitize_pkg_field(pkgdesc);
    std::string safe_url = sanitize_pkg_field(url);
    std::string safe_license = sanitize_pkg_field(license);

    print_step("Downloading source and calculating SHA-256");
    std::string temp_file = pkgname + "_temp";
    cmd_result wget_res = execute_command("wget -qO " + escape_shell_arg(temp_file) + " " + escape_shell_arg(source));

    std::string sha256 = "SKIP";
    if (wget_res.exit_status != 0) {
        print_error("Failed to download source from: " + source);
        std::cout << YELLOW << "[!] SHA-256 will be set to 'SKIP' — package integrity won't be verified.\n" << RESET;
        std::string proceed = ask_input("Continue anyway? (y/N)", false);
        if (proceed != "y" && proceed != "Y") {
            fs::remove(temp_file);
            wait_for_enter();
            return;
        }
    } else {
        cmd_result sum_res = execute_command("sha256sum " + escape_shell_arg(temp_file) + " | awk '{print $1}'");
        if (!sum_res.output.empty()) sha256 = sum_res.output;
    }
    fs::remove(temp_file);

    std::string source_file = ask_input("Main file / Directory (e.g., repoForge.cpp or main.py)");
    std::string binary_name = ask_input("Command name in system (e.g., repoforge)");

    print_step("Generating PKGBUILD");
    std::ofstream pkgbuild((fs::path(pkgname) / "PKGBUILD").string());
    pkgbuild << "# Maintainer: Auto-generated by Repo Forge\n";
    pkgbuild << "pkgname=" << pkgname << "\n";
    pkgbuild << "pkgver=" << sanitize_pkg_field(pkgver) << "\n";
    pkgbuild << "pkgrel=1\n";
    pkgbuild << "pkgdesc=\"" << safe_pkgdesc << "\"\n";
    pkgbuild << "arch=('" << arch << "')\n";
    pkgbuild << "url=\"" << safe_url << "\"\n";
    pkgbuild << "license=('" << safe_license << "')\n";

    if (!dep_list.empty()) {
        pkgbuild << "depends=(";
        for (size_t i = 0; i < dep_list.size(); ++i) {
            pkgbuild << "'" << dep_list[i] << "'";
            if (i < dep_list.size() - 1) pkgbuild << " ";
        }
        pkgbuild << ")\n";
    }

    if (build_type == "1") pkgbuild << "makedepends=('gcc')\n";

    pkgbuild << "source=(\"" << source << "\")\n";
    pkgbuild << "sha256sums=('" << sha256 << "')\n\n";

    if (build_type == "1") {
        pkgbuild << "build() {\n";
        pkgbuild << "    cd \"$srcdir\"\n";
        pkgbuild << "    g++ -std=c++17 -O2 -Wl,-z,relro,-z,now -o " << binary_name << " " << source_file << " -lpthread\n";
        pkgbuild << "}\n\n";
    }

    pkgbuild << "package() {\n";
    pkgbuild << "    cd \"$srcdir\"\n";

    if (build_type == "1" || build_type == "3") {
        pkgbuild << "    install -Dm755 " << (build_type == "3" ? source_file : binary_name)
        << " \"$pkgdir/usr/bin/" << binary_name << "\"\n";
    } else if (build_type == "2") {
        pkgbuild << "    install -Dm755 " << source_file << " \"$pkgdir/usr/bin/" << binary_name << "\"\n";
    }

    pkgbuild << "    if [ -f LICENSE ]; then\n";
    pkgbuild << "        install -Dm644 LICENSE \"$pkgdir/usr/share/licenses/$pkgname/LICENSE\"\n";
    pkgbuild << "    fi\n";
    pkgbuild << "}\n";
    pkgbuild.close();

    print_step("Generating .SRCINFO natively");
    std::ofstream srcinfo((fs::path(pkgname) / ".SRCINFO").string());
    srcinfo << "pkgbase = " << pkgname << "\n";
    srcinfo << "\tpkgdesc = " << safe_pkgdesc << "\n";
    srcinfo << "\tpkgver = " << sanitize_pkg_field(pkgver) << "\n";
    srcinfo << "\tpkgrel = 1\n";
    srcinfo << "\turl = " << safe_url << "\n";
    srcinfo << "\tarch = " << arch << "\n";
    srcinfo << "\tlicense = " << safe_license << "\n";
    if (build_type == "1") srcinfo << "\tmakedepends = gcc\n";

    for (const auto& d : dep_list) {
        srcinfo << "\tdepends = " << d << "\n";
    }

    srcinfo << "\tsource = " << source << "\n";
    srcinfo << "\tsha256sums = " << sha256 << "\n\n";
    srcinfo << "pkgname = " << pkgname << "\n";
    srcinfo.close();
    print_success(".SRCINFO generated successfully.");

    if (ask_input("Push to AUR? (Y/n)", false) != "n") {
        std::string push_cmd = "cd " + escape_shell_arg(pkgname) + " && git add PKGBUILD .SRCINFO && git diff --cached --quiet || (git commit -m 'Automated push by Repo Forge' && git push origin master)";
        cmd_result push_res = execute_command(push_cmd);
        if (push_res.exit_status == 0) {
            print_success("Package is live on AUR!");
        } else {
            print_error("Git push failed! Check your SSH key and AUR permissions.\nDetails: " + push_res.output);
        }
    }
    wait_for_enter();
}

void init_repo() {
    std::cout << "\n" << BOLD << CYAN << "--- BUILDING REPOSITORY SKELETON ---" << RESET << "\n";
    std::string repo_dir = ask_input("Repository Root Directory (e.g., ~/myrepo)", true, true, g_config.last_repo_dir);
    g_config.last_repo_dir = repo_dir;

    try {
        fs::create_directories(fs::path(repo_dir) / "debian" / "pool" / "main");
        fs::create_directories(fs::path(repo_dir) / "rpm");
        fs::create_directories(fs::path(repo_dir) / "keys");

        std::ofstream gitignore((fs::path(repo_dir) / ".gitignore").string());
        gitignore << "# Security\n*.sec\n*.sk\n\n# Temp\n*.log\n.repodata.old/\n";
        print_success("Skeleton ready at: " + repo_dir);
    } catch (const std::exception& e) { print_error(e.what()); }
    wait_for_enter();
}

bool verify_package(const std::string& filepath) {
    std::string ext = fs::path(filepath).extension().string();
    cmd_result res;

    if (ext == ".deb") {
        res = execute_command("dpkg-deb -I " + escape_shell_arg(filepath));
    } else if (ext == ".rpm") {
        res = execute_command("rpm -qp " + escape_shell_arg(filepath));
    } else {
        return true;
    }

    std::lock_guard<std::mutex> lock(g_print_mutex);
    if (res.exit_status == 0) {
        std::cout << GREEN << "  [OK]   " << RESET << filepath << "\n";
        return true;
    } else {
        std::cout << RED << "  [FAIL] " << RESET << filepath << "\n";
        return false;
    }
}

void index_repo() {
    std::cout << "\n" << BOLD << CYAN << "--- SCANNING & INDEXING ENGINE ---" << RESET << "\n";
    std::string repo_dir = ask_input("Repository Root Directory", true, true, g_config.last_repo_dir);
    g_config.last_repo_dir = repo_dir;

    if (!fs::exists(repo_dir)) { print_error("Directory not found!"); wait_for_enter(); return; }

    print_step("Verifying packages using Thread Pooling (Hardware limits applied)");

    std::vector<std::string> deb_packages;
    std::vector<std::string> rpm_packages;
    std::vector<std::string> target_packages;

    try {
        auto options = fs::directory_options::skip_permission_denied;
        for (const auto& entry : fs::recursive_directory_iterator(repo_dir, options)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".deb") {
                    deb_packages.push_back(entry.path().string());
                    target_packages.push_back(entry.path().string());
                } else if (ext == ".rpm") {
                    rpm_packages.push_back(entry.path().string());
                    target_packages.push_back(entry.path().string());
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        print_error(std::string("Filesystem iteration error: ") + e.what());
    }

    if (target_packages.empty()) {
        std::cout << YELLOW << "\nNo .deb or .rpm packages found in " << repo_dir << ".\n" << RESET;
        wait_for_enter();
        return;
    }

    unsigned int max_threads = std::thread::hardware_concurrency();
    if (max_threads == 0) max_threads = 4;

    int valid_count = 0, error_count = 0;

    for (size_t i = 0; i < target_packages.size(); i += max_threads) {
        std::vector<std::future<bool>> futures;
        size_t chunk_end = std::min(i + max_threads, target_packages.size());

        for (size_t j = i; j < chunk_end; ++j) {
            futures.push_back(std::async(std::launch::async, verify_package, target_packages[j]));
        }

        for (auto& f : futures) {
            if (f.get()) valid_count++;
            else error_count++;
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_print_mutex);
        std::cout << "\n" << YELLOW << "Verification Summary: " << valid_count << " valid, " << error_count << " invalid packages found.\n" << RESET;
    }

    if (error_count > 0) {
        std::string proceed = ask_input("Corrupt packages detected. Continue indexing anyway? (y/N)", false);
        if (proceed != "y" && proceed != "Y") return;
    }

    if (!deb_packages.empty()) {
        print_step("Indexing Debian packages");
        std::string codename = ask_input("Codename (leave empty for 'stable')", false); if(codename.empty()) codename = "stable";
        std::string deb_arch = ask_input("Architecture (leave empty for 'amd64')", false); if(deb_arch.empty()) deb_arch = "amd64";

        fs::path dists_base = fs::path(repo_dir) / "debian" / "dists" / codename;
        fs::path dists_dir = dists_base / "main" / ("binary-" + deb_arch);
        fs::create_directories(dists_dir);

        std::string packages_file = (dists_dir / "Packages").string();

        std::string scan_cmd = "cd " + escape_shell_arg((fs::path(repo_dir) / "debian").string()) +
        " && dpkg-scanpackages pool/main /dev/null 2>/dev/null";
        cmd_result deb_res = execute_command(scan_cmd, false);

        if (deb_res.exit_status == 0) {
            std::ofstream out(packages_file);
            out << deb_res.output << "\n";
            out.close();

            execute_command("gzip -k -f " + escape_shell_arg(packages_file));
            print_success("Debian Packages index updated.");

            print_step("Generating Release file");

            std::string release_file = (dists_base / "Release").string();
            std::string rel_cmd = "cd " + escape_shell_arg((fs::path(repo_dir) / "debian").string()) +
            " && apt-ftparchive " +
            "-o APT::FTPArchive::Release::Origin=\"RepoForge\" " +
            "-o APT::FTPArchive::Release::Label=\"RepoForge\" " +
            "-o APT::FTPArchive::Release::Suite=" + escape_shell_arg(codename) + " " +
            "-o APT::FTPArchive::Release::Codename=" + escape_shell_arg(codename) + " " +
            "-o APT::FTPArchive::Release::Architectures=" + escape_shell_arg(deb_arch) + " " +
            "release " + escape_shell_arg("dists/" + codename) + " 2>/dev/null";

            cmd_result rel_res = execute_command(rel_cmd, false);
            std::ofstream rel_out(release_file);
            rel_out << rel_res.output << "\n";
            rel_out.close();

            std::string sign_choice = ask_input("Do you want to sign the repository with GPG? (Y/n)", false);
            if (sign_choice != "n" && sign_choice != "N") {
                std::string gpg_key = ask_input("Enter your GPG Key ID/Email", true, false, g_config.last_gpg_key);
                g_config.last_gpg_key = gpg_key;

                print_step("Signing Release file");
                std::string inrelease_abs = (dists_base / "InRelease").string();
                std::string releasegpg_abs = (dists_base / "Release.gpg").string();

                execute_command("rm -f " + escape_shell_arg(inrelease_abs) + " " + escape_shell_arg(releasegpg_abs));

                std::string sign_cmd1 = "gpg --yes --default-key " + escape_shell_arg(gpg_key) + " -abs -o " + escape_shell_arg(releasegpg_abs) + " " + escape_shell_arg(release_file);
                std::string sign_cmd2 = "gpg --yes --default-key " + escape_shell_arg(gpg_key) + " --clearsign -o " + escape_shell_arg(inrelease_abs) + " " + escape_shell_arg(release_file);

                cmd_result sign1_res = execute_command(sign_cmd1);
                cmd_result sign2_res = execute_command(sign_cmd2);

                if (sign1_res.exit_status == 0 && sign2_res.exit_status == 0) {
                    print_success("Repository strictly signed with GPG.");
                } else {
                    print_error("GPG signing failed! Make sure your key is correct and pinentry is working.\nDetails: " + sign1_res.output + "\n" + sign2_res.output);
                }
            }
        } else {
            print_error("Debian indexing failed! (is dpkg-dev installed?)\nDetails: " + deb_res.output);
        }
    }

    if (!rpm_packages.empty()) {
        print_step("Indexing RPM packages");
        std::string rpm_dir = (fs::path(repo_dir) / "rpm").string();
        std::string rpm_cmd = "createrepo_c " + escape_shell_arg(rpm_dir);
        cmd_result rpm_res = execute_command(rpm_cmd);

        if (rpm_res.exit_status != 0) {
            rpm_cmd = "createrepo " + escape_shell_arg(rpm_dir);
            rpm_res = execute_command(rpm_cmd);
            if (rpm_res.exit_status == 0) {
                print_success("RPM index updated (using fallback 'createrepo').");
            } else {
                print_error("RPM indexing failed! (is createrepo or createrepo_c installed?)\nDetails: " + rpm_res.output);
            }
        } else {
            print_success("RPM index updated.");
        }
    }

    wait_for_enter();
}

void install_tools() {
    std::cout << "\n" << BOLD << CYAN << "--- INSTALLING REQUIRED TOOLS ---" << RESET << "\n";
    cmd_result apt_check = execute_command("command -v apt");
    cmd_result dnf_check = execute_command("command -v dnf");
    cmd_result pacman_check = execute_command("command -v pacman");

    cmd_result result{-1, ""};

    if (apt_check.exit_status == 0) {
        print_step("Debian/Ubuntu/MX Linux detected");
        result = execute_command("sudo apt update && sudo apt install -y dpkg-dev apt-utils createrepo-c git openssh-client gnupg");
    } else if (dnf_check.exit_status == 0) {
        print_step("Fedora/RHEL detected");
        result = execute_command("sudo dnf install -y createrepo_c dpkg git openssh-clients gnupg2");
    } else if (pacman_check.exit_status == 0) {
        print_step("Arch Linux detected");
        result = execute_command("sudo pacman -Sy --noconfirm dpkg createrepo_c git openssh gnupg");
    } else {
        print_error("No supported package manager found (apt, dnf, or pacman).");
        wait_for_enter();
        return;
    }

    if (result.exit_status == 0) {
        print_success("All tools installed successfully.");
    } else {
        print_error("Failed to install some packages. Details:\n" + result.output);
    }
    wait_for_enter();
}

void show_guide() {
    clear_screen();
    std::cout << BOLD << CYAN << "--- USER GUIDE ---\n" << RESET;
    std::cout << "1. Init: Automatically creates repository subdirectories (debian/pool, rpm, etc.).\n";
    std::cout << "2. Index: Verifies and indexes .deb and .rpm packages using Multithreading limits.\n";
    std::cout << "3. AUR: Prepares and pushes Arch packages natively via C++.\n";
    std::cout << "4. Install: Installs necessary development tools for your system.\n";
    ask_input("Press [ENTER] to return", false);
}

int main() {
    load_config();
    std::signal(SIGINT, handle_sigint);

    while(true) {
        if (g_should_exit.load()) {
            show_exit_message();
            break;
        }

        print_banner();
        std::cout << "  [1] Create Repo Skeleton\n  [2] Verify and Index Repository\n  [3] AUR Automation Engine\n  [4] Install Required Tools\n  [0] User Guide\n  [q] Exit\n\n";
        std::string choice = ask_input("Your choice", true, false);
        if (choice == "1") init_repo();
        else if (choice == "2") index_repo();
        else if (choice == "3") handle_aur();
        else if (choice == "4") install_tools();
        else if (choice == "0") show_guide();
        else if (choice == "q" || choice == "Q") { show_exit_message(); break; }
    }
    return 0;
}
