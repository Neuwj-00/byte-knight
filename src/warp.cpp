#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <algorithm>

const std::string CF_ORANGE = "\033[38;2;243;128;32m";
const std::string CF_YELLOW = "\033[38;2;250;173;63m";
const std::string CF_BRIGHT = "\033[38;2;255;213;128m";
const std::string GREEN     = "\033[1;32m";
const std::string RED       = "\033[1;31m";
const std::string CYAN      = "\033[1;36m";
const std::string RESET     = "\033[0m";
const std::string BOLD      = "\033[1m";

struct PcloseDeleter {
    void operator()(FILE* f) const {
        if (f) pclose(f);
    }
};

void clear_screen() {
    std::cout << "\033[2J\033[3J\033[H";
    std::cout.flush();
}

void pause_screen() {
    std::cout << "\n  " << CF_YELLOW << "Press Enter to continue..." << RESET;
    std::string dummy;
    std::getline(std::cin, dummy);
}

std::string run_cmd(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::string full_cmd = "warp-cli " + cmd + " 2>&1";

    std::unique_ptr<FILE, PcloseDeleter> pipe(popen(full_cmd.c_str(), "r"));

    if (!pipe) {
        return "error: popen failed";
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void get_status_info(std::string& status_text, std::string& status_color) {
    std::string output = run_cmd("status");

    std::string lower_output = output;
    std::transform(lower_output.begin(), lower_output.end(), lower_output.begin(), ::tolower);

    if (lower_output.find("connected") != std::string::npos && lower_output.find("disconnected") == std::string::npos) {
        status_text = "CONNECTED";
        status_color = GREEN;
    } else if (lower_output.find("disconnected") != std::string::npos) {
        status_text = "DISCONNECTED";
        status_color = CF_YELLOW;
    } else if (lower_output.find("connecting") != std::string::npos) {
        status_text = "CONNECTING...";
        status_color = CYAN;
    } else {
        status_text = "OFFLINE / ERROR";
        status_color = RED;
    }
}

void print_header() {
    std::cout << BOLD << CF_ORANGE << "  ☁️  CLOUD"
    << CF_YELLOW << "FLARE "
    << CF_BRIGHT << "WARP+\n" << RESET;
    std::cout << "  " << CF_YELLOW << "made by neuwj - neuwj@bk.ru\n" << RESET;
    std::cout << CF_ORANGE << "  ----------------------------------\n" << RESET;
}

void dns_filters_menu() {
    std::string choice;
    while (true) {
        clear_screen();
        print_header();

        std::cout << "  " << BOLD << "🛡️  DNS Families & Filters" << RESET << "\n\n";
        std::cout << "  " << CF_BRIGHT << "[1]" << RESET << " Off (Disable Filtering)\n";
        std::cout << "  " << CF_BRIGHT << "[2]" << RESET << " Malware Only\n";
        std::cout << "  " << CF_BRIGHT << "[3]" << RESET << " Full (Malware + Adult Content)\n";
        std::cout << "  " << CF_BRIGHT << "[4]" << RESET << " Back to Main Menu\n";
        std::cout << "\n  Select (1-4): ";

        if (!std::getline(std::cin, choice)) break;

        if (choice == "4") break;

        if (choice == "1") {
            std::cout << "  " << run_cmd("dns families off") << "\n";
        } else if (choice == "2") {
            std::cout << "  " << run_cmd("dns families malware") << "\n";
        } else if (choice == "3") {
            std::cout << "  " << run_cmd("dns families full") << "\n";
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

        if (choice == "1") {
            std::cout << "\n" << run_cmd("registration show") << "\n";
            pause_screen();
        } else if (choice == "2") {
            std::cout << "\n" << run_cmd("settings") << "\n";
            pause_screen();
        } else if (choice == "3") {
            std::string target;
            std::cout << "\n  " << BOLD << "Enter IP or Domain to exclude: " << RESET;
            std::getline(std::cin, target);
            if (!target.empty()) {
                std::cout << "  " << run_cmd("exclude add " + target) << "\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        } else if (choice == "4") {
            std::cout << "\n" << run_cmd("exclude list") << "\n";
            pause_screen();
        } else if (choice == "5") {
            std::cout << "\n  " << BOLD << CF_ORANGE << ">> Running diagnostics... This may take a moment." << RESET << std::endl;
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

        if (choice == "6") {
            break;
        }
        else if (choice == "1") {
            std::cout << "\n  " << BOLD << CF_ORANGE << ">> Registering device... (Accepting TOS)" << RESET << std::endl;
            std::cout << "  " << run_cmd("registration new --accept-tos") << "\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        else if (choice == "2") {
            std::string key;
            std::cout << "\n  " << BOLD << "Key: " << RESET;
            std::getline(std::cin, key);

            if (!key.empty()) {
                std::cout << "  " << BOLD << CF_ORANGE << ">> Applying License Key..." << RESET << std::endl;
                std::cout << "  " << run_cmd("registration license " + key) << "\n";
            } else {
                std::cout << "  " << RED << ">> Key input was empty. Canceled." << RESET << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        else if (choice == "3") {
            std::string org;
            std::cout << "\n  " << BOLD << "Organization Name: " << RESET;
            std::getline(std::cin, org);

            if (!org.empty()) {
                std::cout << "  " << BOLD << CF_ORANGE << ">> Enrolling to Teams..." << RESET << std::endl;
                std::cout << "  " << run_cmd("teams-enroll " + org) << "\n";
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        else if (choice == "4") {
            std::cout << "\n  " << BOLD << "Select Mode:" << RESET << "\n";
            std::cout << "  " << CF_BRIGHT << "[1]" << RESET << " Standard Warp+\n";
            std::cout << "  " << CF_BRIGHT << "[2]" << RESET << " DNS Only (1.1.1.1)\n";
            std::cout << "  " << CF_BRIGHT << "[3]" << RESET << " Warp+ & DNS\n";
            std::cout << "  " << CF_BRIGHT << "[4]" << RESET << " Proxy (Local SOCKS5)\n";
            std::cout << "  Choice: ";

            std::string mode_choice;
            std::getline(std::cin, mode_choice);

            if (mode_choice == "1") {
                std::cout << "  " << run_cmd("mode warp") << "\n";
            } else if (mode_choice == "2") {
                std::cout << "  " << run_cmd("mode doh") << "\n";
            } else if (mode_choice == "3") {
                std::cout << "  " << run_cmd("mode warp+doh") << "\n";
            } else if (mode_choice == "4") {
                std::cout << "  " << run_cmd("mode proxy") << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
        else if (choice == "5") {
            std::cout << "\n  " << BOLD << "Select Protocol:" << RESET << "\n";
            std::cout << "  " << CF_BRIGHT << "[1]" << RESET << " WireGuard\n";
            std::cout << "  " << CF_BRIGHT << "[2]" << RESET << " MASQUE (Recommended)\n";
            std::cout << "  Choice: ";

            std::string proto_choice;
            std::getline(std::cin, proto_choice);

            if (proto_choice == "1") {
                std::cout << "  " << run_cmd("tunnel protocol set WireGuard") << "\n";
            } else if (proto_choice == "2") {
                std::cout << "  " << run_cmd("tunnel protocol set MASQUE") << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
    }
}

int main() {
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
            std::cout << "  " << CF_YELLOW << "made by neuwj - neuwj@bk.ru\n" << RESET;
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            break;
        }
        else if (choice == "1") {
            std::cout << "\n  " << BOLD << CF_ORANGE << ">> Initiating Connection..." << RESET << std::endl;
            run_cmd("connect");
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        else if (choice == "2") {
            std::cout << "\n  " << BOLD << CF_YELLOW << ">> Breaking Connection..." << RESET << std::endl;
            run_cmd("disconnect");
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
