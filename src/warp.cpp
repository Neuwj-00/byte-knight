#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <algorithm>
#include <string_view>
#include <sstream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <future>

constexpr std::string_view CF_ORANGE = "\033[38;2;243;128;32m";
constexpr std::string_view CF_YELLOW = "\033[38;2;250;173;63m";
constexpr std::string_view CF_BRIGHT = "\033[38;2;255;213;128m";
constexpr std::string_view GREEN     = "\033[1;32m";
constexpr std::string_view RED       = "\033[1;31m";
constexpr std::string_view CYAN      = "\033[1;36m";
constexpr std::string_view RESET     = "\033[0m";
constexpr std::string_view BOLD      = "\033[1m";

struct PcloseDeleter {
    void operator()(FILE* f) const {
        if (f) pclose(f);
    }
};

// Returns current terminal width
int get_terminal_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) return 80;
    return w.ws_col;
}

// Creates a dynamic separator line based on current terminal width
std::string get_separator() {
    return std::string(get_terminal_width(), '-');
}

void clear_screen() {
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

void pause_screen() {
    std::cout << "\n  " << CF_YELLOW << "Press Enter to continue..." << RESET;
    std::string dummy;
    std::getline(std::cin, dummy);
}

// Formats shell outputs with a simple 2-space left margin
void print_output(const std::string& output) {
    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {
        if (!line.empty()) {
            std::cout << "  " << line << "\n";
        }
    }
}

bool is_safe_input(const std::string& input) {
    const std::string forbidden_chars = ";&|`$<>*?()[]{}'\"\\";
    for (char c : input) {
        if (forbidden_chars.find(c) != std::string::npos) {
            return false;
        }
    }
    return true;
}

std::string run_shell(const std::string& arg) {
    std::array<char, 128> buffer;
    std::string result;
    std::string shell_command = "warp-cli " + arg + " 2>&1";

    std::unique_ptr<FILE, PcloseDeleter> pipe(popen(shell_command.c_str(), "r"));

    if (!pipe) {
        return "error: Failed to execute system shell.";
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool check_warp_installed() {
    int result = std::system("command -v warp-cli > /dev/null 2>&1");
    return (result == 0);
}

void install_warp() {
    std::cout << "\n  " << BOLD << CYAN << "--- INSTALLING CLOUDFLARE WARP ---" << RESET << "\n";

    bool has_apt = (std::system("command -v apt-get > /dev/null 2>&1") == 0);
    bool has_yum = (std::system("command -v yum > /dev/null 2>&1") == 0);
    bool has_pacman = (std::system("command -v pacman > /dev/null 2>&1") == 0);

    int result = -1;

    if (has_apt) {
        std::cout << "  Debian/Ubuntu/Derivatives detected.\n";
        std::string apt_script =
        "curl -fsSL https://pkg.cloudflareclient.com/pubkey.gpg | sudo gpg --yes --dearmor --output /usr/share/keyrings/cloudflare-warp-archive-keyring.gpg && "
        "CODENAME=$(. /etc/os-release && echo \"${UBUNTU_CODENAME:-$VERSION_CODENAME}\"); "
        "if [ -z \"$CODENAME\" ]; then CODENAME=$(lsb_release -cs); fi; "
        "if ! curl -s -f -m 5 \"https://pkg.cloudflareclient.com/dists/$CODENAME/Release\" > /dev/null; then "
        "  if grep -qi \"ubuntu\" /etc/os-release 2>/dev/null; then CODENAME=\"jammy\"; else CODENAME=\"bookworm\"; fi; "
        "fi; "
        "echo \"deb [signed-by=/usr/share/keyrings/cloudflare-warp-archive-keyring.gpg] https://pkg.cloudflareclient.com/ $CODENAME main\" | sudo tee /etc/apt/sources.list.d/cloudflare-client.list && "
        "sudo apt-get update && sudo apt-get install cloudflare-warp -y";
        result = std::system(apt_script.c_str());
    } else if (has_yum) {
        std::cout << "  Red Hat/CentOS/Fedora detected.\n";
        result = std::system("sudo rpm -e 'gpg-pubkey(4fa1c3ba-61abda35)' ; sudo rpm --import https://pkg.cloudflareclient.com/pubkey.gpg && "
        "curl -fsSl https://pkg.cloudflareclient.com/cloudflare-warp-ascii.repo | sudo tee /etc/yum.repos.d/cloudflare-warp.repo && "
        "sudo yum update -y && sudo yum install cloudflare-warp -y");
    } else if (has_pacman) {
        std::cout << "  Arch Linux detected.\n";
        result = std::system("yay -S cloudflare-warp-bin --noconfirm");
    } else {
        std::cout << "  " << RED << "No supported package manager found." << RESET << "\n";
        pause_screen();
        return;
    }

    if (result == 0) {
        std::cout << "  " << GREEN << "Installation completed successfully!" << RESET << "\n";
    } else {
        std::cout << "  " << RED << "Installation failed or was interrupted." << RESET << "\n";
    }
    pause_screen();
}

bool check_and_start_service() {
    int status = -1;

    if (std::system("command -v systemctl > /dev/null 2>&1") == 0) {
        status = std::system("systemctl is-active --quiet warp-svc");
    } else {
        status = std::system("service warp-svc status > /dev/null 2>&1");
    }

    if (status != 0) {
        clear_screen();
        std::cout << "\n  " << RED << BOLD << "WARP Service is currently NOT running!" << RESET << "\n";
        std::cout << "  The background daemon 'warp-svc' must be active.\n";
        std::cout << "  Would you like to start it now? (Y/n): ";
        std::string ans;
        std::getline(std::cin, ans);

        if (ans.empty() || ans == "Y" || ans == "y") {
            std::cout << "  " << CF_ORANGE << "Starting service..." << RESET << "\n";
            int start_status;
            if (std::system("command -v systemctl > /dev/null 2>&1") == 0) {
                start_status = std::system("sudo systemctl start warp-svc");
            } else {
                start_status = std::system("sudo service warp-svc start");
            }

            if (start_status == 0) {
                std::cout << "  " << GREEN << "Service started successfully!" << RESET << "\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return true;
            } else {
                std::cout << "  " << RED << "Failed to start service. Please check your privileges." << RESET << "\n";
                pause_screen();
                return false;
            }
        }
        return false;
    }
    return true;
}

void get_status_info(std::string& status_text, std::string& status_color) {
    std::string output = run_shell("status");
    std::string lower_output = output;
    std::transform(lower_output.begin(), lower_output.end(), lower_output.begin(), ::tolower);

    if (lower_output.find("connected") != std::string::npos && lower_output.find("disconnected") == std::string::npos) {
        status_text = "CONNECTED";
        status_color = std::string(GREEN);
    } else if (lower_output.find("disconnected") != std::string::npos) {
        status_text = "DISCONNECTED";
        status_color = std::string(CF_YELLOW);
    } else if (lower_output.find("connecting") != std::string::npos) {
        status_text = "CONNECTING...";
        status_color = std::string(CYAN);
    } else {
        status_text = "OFFLINE / ERROR";
        status_color = std::string(RED);
    }
}

void print_header() {
    std::cout << "\n";
    std::cout << "  " << BOLD << CF_ORANGE << "☁️  CLOUD"
    << CF_YELLOW << "FLARE "
    << CF_BRIGHT << "WARP+ " << RESET << "v1.1.2\n";
    std::cout << "  " << CF_YELLOW << "made by Neuwj - neuwj@bk.ru\n" << RESET;
    std::cout << CF_ORANGE << get_separator() << "\n" << RESET;
}

void dns_filters_menu() {
    std::string choice;
    while (true) {
        clear_screen();
        print_header();

        std::cout << "  " << BOLD << "🛡️  DNS Families & Filters" << RESET << "\n\n";
        std::cout << "  " << CF_BRIGHT << "[1]" << RESET << " Off (No Filtering)\n";
        std::cout << "  " << CF_BRIGHT << "[2]" << RESET << " Malware Only\n";
        std::cout << "  " << CF_BRIGHT << "[3]" << RESET << " Full (Malware + Adult Content)\n";
        std::cout << "  " << CF_BRIGHT << "[4]" << RESET << " Back to Main Menu\n";
        std::cout << "\n  Select (1-4): ";

        if (!std::getline(std::cin, choice)) break;
        if (choice == "4") break;

        std::cout << "\n";
        if (choice == "1") {
            print_output(run_shell("dns families off"));
        } else if (choice == "2") {
            print_output(run_shell("dns families malware"));
        } else if (choice == "3") {
            print_output(run_shell("dns families full"));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }
}

void advanced_menu() {
    std::string choice;
    while (true) {
        clear_screen();
        print_header();

        std::cout << "  " << BOLD << "🔧  Advanced & Diagnostics" << RESET << "\n\n";
        std::cout << "  " << CF_BRIGHT << "[1]" << RESET << " Show Registration / Account Info\n";
        std::cout << "  " << CF_BRIGHT << "[2]" << RESET << " Show Current Settings\n";
        std::cout << "  " << CF_BRIGHT << "[3]" << RESET << " Exclude IP or Domain\n";
        std::cout << "  " << CF_BRIGHT << "[4]" << RESET << " Show Excluded List\n";
        std::cout << "  " << CF_BRIGHT << "[5]" << RESET << " Run Diagnostics (warp-diag)\n";
        std::cout << "  " << CF_BRIGHT << "[6]" << RESET << " Back to Main Menu\n";
        std::cout << "\n  Select (1-6): ";

        if (!std::getline(std::cin, choice)) break;
        if (choice == "6") break;

        std::cout << "\n";
        if (choice == "1") {
            print_output(run_shell("registration show"));
            pause_screen();
        } else if (choice == "2") {
            print_output(run_shell("settings"));
            pause_screen();
        } else if (choice == "3") {
            std::string target;
            std::cout << "  " << BOLD << "Enter IP or Domain to exclude: " << RESET;
            std::getline(std::cin, target);

            if (!target.empty()) {
                if (is_safe_input(target)) {
                    print_output(run_shell("exclude add " + target));
                } else {
                    std::cout << "  " << RED << "ERROR: Invalid or dangerous characters detected!" << RESET << "\n";
                }
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        } else if (choice == "4") {
            print_output(run_shell("exclude list"));
            pause_screen();
        } else if (choice == "5") {
            std::cout << "  " << BOLD << CF_ORANGE << ">> Running diagnostics tool... This might take a while." << RESET << "\n";
            std::system("warp-diag");
            pause_screen();
        }
    }
}

void settings_menu() {
    std::string choice;
    while (true) {
        clear_screen();
        print_header();

        std::cout << "  " << BOLD << "⚙️  Setup & Settings" << RESET << "\n\n";
        std::cout << "  " << CF_BRIGHT << "[1]" << RESET << " Register New Device (First Time Setup)\n";
        std::cout << "  " << CF_BRIGHT << "[2]" << RESET << " Enter License Key\n";
        std::cout << "  " << CF_BRIGHT << "[3]" << RESET << " Teams / Zero Trust Enroll\n";
        std::cout << "  " << CF_BRIGHT << "[4]" << RESET << " Change Mode\n";
        std::cout << "  " << CF_BRIGHT << "[5]" << RESET << " Change Protocol (WireGuard / MASQUE)\n";
        std::cout << "  " << CF_BRIGHT << "[6]" << RESET << " Back to Main Menu\n";
        std::cout << "\n  Select (1-6): ";

        if (!std::getline(std::cin, choice)) break;
        if (choice == "6") break;

        std::cout << "\n";
        if (choice == "1") {
            std::cout << "  " << BOLD << CF_ORANGE << ">> Registering device... (Accepting TOS)" << RESET << "\n";
            print_output(run_shell("registration new --accept-tos"));
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        else if (choice == "2") {
            std::string key;
            std::cout << "  " << BOLD << "License Key: " << RESET;
            std::getline(std::cin, key);

            if (!key.empty() && is_safe_input(key)) {
                std::cout << "\n  " << BOLD << CF_ORANGE << ">> Applying License Key..." << RESET << "\n";
                print_output(run_shell("registration license " + key));
            } else {
                std::cout << "\n  " << RED << ">> Invalid or empty input. Cancelled." << RESET << "\n";
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        else if (choice == "3") {
            std::string org;
            std::cout << "  " << BOLD << "Organization Name: " << RESET;
            std::getline(std::cin, org);

            if (!org.empty() && is_safe_input(org)) {
                std::cout << "\n  " << BOLD << CF_ORANGE << ">> Enrolling to Teams..." << RESET << "\n";
                print_output(run_shell("teams-enroll " + org));
            } else {
                std::cout << "\n  " << RED << ">> Invalid or empty input. Cancelled." << RESET << "\n";
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        else if (choice == "4") {
            std::cout << "  " << BOLD << "Select Mode:" << RESET << "\n";
            std::cout << "  " << CF_BRIGHT << "[1]" << RESET << " Standard Warp+\n";
            std::cout << "  " << CF_BRIGHT << "[2]" << RESET << " DNS Only (1.1.1.1)\n";
            std::cout << "  " << CF_BRIGHT << "[3]" << RESET << " Warp+ & DNS\n";
            std::cout << "  " << CF_BRIGHT << "[4]" << RESET << " Proxy (Local SOCKS5)\n";
            std::cout << "  Choice: ";

            std::string mode_choice;
            std::getline(std::cin, mode_choice);

            std::cout << "\n";
            if (mode_choice == "1") print_output(run_shell("mode warp"));
            else if (mode_choice == "2") print_output(run_shell("mode doh"));
            else if (mode_choice == "3") print_output(run_shell("mode warp+doh"));
            else if (mode_choice == "4") print_output(run_shell("mode proxy"));

            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
        else if (choice == "5") {
            std::cout << "  " << BOLD << "Select Protocol:" << RESET << "\n";
            std::cout << "  " << CF_BRIGHT << "[1]" << RESET << " WireGuard\n";
            std::cout << "  " << CF_BRIGHT << "[2]" << RESET << " MASQUE (Recommended)\n";
            std::cout << "  Choice: ";

            std::string proto_choice;
            std::getline(std::cin, proto_choice);

            std::cout << "\n";
            if (proto_choice == "1") print_output(run_shell("tunnel protocol set WireGuard"));
            else if (proto_choice == "2") print_output(run_shell("tunnel protocol set MASQUE"));

            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
    }
}

int main() {
    if (!check_warp_installed()) {
        clear_screen();
        print_header();
        std::cout << "\n  " << RED << BOLD << "Critical Error: 'warp-cli' not found on system!" << RESET << "\n";
        std::cout << "  Would you like to install Cloudflare WARP now? (Y/n): ";
        std::string ans;
        std::getline(std::cin, ans);
        if (ans.empty() || ans == "Y" || ans == "y") {
            install_warp();
            if (!check_warp_installed()) {
                return 1;
            }
        } else {
            return 1;
        }
    }

    if (!check_and_start_service()) {
        std::cout << "  " << RED << "Warning: WARP service is not running. Commands will likely fail!" << RESET << "\n";
        pause_screen();
    }

    std::string choice;

    while (true) {
        std::string status_text, status_color;
        get_status_info(status_text, status_color);

        clear_screen();
        print_header();

        std::cout << "  " << BOLD << "Network Info:" << RESET << "\n";
        std::cout << "  Current Status: [" << status_color << status_text << RESET << "]\n\n";

        std::cout << "  What would you like to do?\n\n";
        std::cout << "  " << CF_BRIGHT << "[1]" << RESET << " Connect\n";
        std::cout << "  " << CF_BRIGHT << "[2]" << RESET << " Disconnect\n";
        std::cout << "  " << CF_BRIGHT << "[3]" << RESET << " Setup & Settings\n";
        std::cout << "  " << CF_BRIGHT << "[4]" << RESET << " DNS Families & Filters\n";
        std::cout << "  " << CF_BRIGHT << "[5]" << RESET << " Advanced & Diagnostics\n";
        std::cout << "  " << CF_BRIGHT << "[6]" << RESET << " Refresh Status\n";
        std::cout << "  " << CF_BRIGHT << "[7]" << RESET << " Exit\n";
        std::cout << "\n  Select (1-7): ";

        if (!std::getline(std::cin, choice)) break;

        if (choice == "7") {
            std::cout << "\n  " << BOLD << CF_ORANGE << "Stay safe, Neuwj! 👋\n" << RESET;
            std::cout << "  " << CF_YELLOW << "made by Neuwj - neuwj@bk.ru\n" << RESET;
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            clear_screen();
            break;
        }
        else if (choice == "1") {
            std::cout << "\n  " << BOLD << CF_ORANGE << ">> Initiating Connection... " << RESET;
            std::cout.flush();

            auto future = std::async(std::launch::async, []() { return run_shell("connect"); });
            const char spinner[] = {'|', '/', '-', '\\'};
            int i = 0;

            while (future.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
                std::cout << "\b" << spinner[i++ % 4] << std::flush;
            }

            std::cout << "\b" << GREEN << "Done!" << RESET << "\n";
            print_output(future.get());
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        else if (choice == "2") {
            std::cout << "\n  " << BOLD << CF_YELLOW << ">> Breaking Connection..." << RESET << "\n";
            print_output(run_shell("disconnect"));
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
        else if (choice == "3") {
            settings_menu();
        }
        else if (choice == "4") {
            dns_filters_menu();
        }
        else if (choice == "5") {
            advanced_menu();
        }
        else if (choice == "6") {
            continue;
        }
    }

    return 0;
}
