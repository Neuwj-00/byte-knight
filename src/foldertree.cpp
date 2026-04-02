#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstdio>

namespace fs = std::filesystem;

bool isTooLarge(const fs::path& path, bool hide_hidden, int limit = 500) {
    int count = 0;
    std::vector<fs::path> dirs;
    dirs.push_back(path);

    while (!dirs.empty()) {
        fs::path current = dirs.back();
        dirs.pop_back();

        try {
            for (const auto& entry : fs::directory_iterator(current)) {
                if (hide_hidden && entry.path().filename().string().front() == '.') {
                    continue;
                }

                count++;
                if (count >= limit) return true;

                if (fs::is_directory(entry.status()) && !fs::is_symlink(entry.symlink_status())) {
                    dirs.push_back(entry.path());
                }
            }
        } catch (...) {
        }
    }
    return false;
}

void buildTree(const fs::path& path, const std::string& prefix, bool hide_hidden, std::stringstream& output) {
    std::vector<fs::directory_entry> entries;
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (hide_hidden && entry.path().filename().string().front() == '.') {
                continue;
            }
            entries.push_back(entry);
        }
    } catch (const std::exception& e) {
        output << prefix << " [Access Denied]\n";
        return;
    }

    std::sort(entries.begin(), entries.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
        return a.path().filename().string() < b.path().filename().string();
    });

    for (size_t i = 0; i < entries.size(); ++i) {
        bool is_last = (i == entries.size() - 1);
        const auto& entry = entries[i];
        std::string filename = entry.path().filename().string();

        output << prefix << (is_last ? "└── " : "├── ") << filename;

        if (fs::is_symlink(entry.symlink_status())) {
            try {
                fs::path target = fs::read_symlink(entry.path());
                output << " -> " << target.string();
            } catch(...) {}
            output << "\n";
        }
        else if (fs::is_directory(entry.status())) {
            output << "\n";
            std::string next_prefix = prefix + (is_last ? "    " : "│   ");
            buildTree(entry.path(), next_prefix, hide_hidden, output);
        }
        else {
            output << "\n";
        }
    }
}

void copyToClipboard(const std::string& text) {
    FILE* pipe = popen("xclip -selection clipboard", "w");
    if (pipe) {
        fwrite(text.c_str(), 1, text.size(), pipe);
        pclose(pipe);
        std::cout << "\n✅ Output successfully copied to clipboard!\n";
    } else {
        std::cerr << "\n❌ Copy failed. Please ensure 'xclip' is installed on your system.\n";
        std::cerr << "To install: sudo apt install xclip\n";
    }
}

int main(int argc, char* argv[]) {
    bool hide_hidden = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h") {
            hide_hidden = true;
        }
    }

    std::cout << "========================================================\n";
    std::cout << "      Tree Generator V0.0.5 - by Neuwj - neuwj@bk.ru    \n";
    std::cout << "========================================================\n\n";

    std::string target_path;
    std::cout << "Enter the directory path (press Enter for current directory): ";
    std::getline(std::cin, target_path);

    if (target_path.empty()) {
        target_path = ".";
    }

    fs::path p(target_path);
    if (!fs::exists(p)) {
        std::cerr << "Error: Specified directory not found: " << target_path << "\n";
        return 1;
    }

    if (isTooLarge(p, hide_hidden, 500)) {
        std::cout << "\nWarning: This directory path will be very long (You are scanning an extremely large directory).\n";
        std::cout << "Are you sure? (Y/N): ";
        std::string answer;
        std::getline(std::cin, answer);
        if (answer != "Y" && answer != "y") {
            std::cout << "Operation cancelled.\n";
            return 0;
        }
    }

    std::stringstream tree_output;
    std::string root_dir_name;
    try {
        fs::path absolute_path = fs::canonical(p);
        root_dir_name = absolute_path.filename().string();

        if (root_dir_name.empty()) {
            root_dir_name = absolute_path.string();
        }
    } catch (...) {
        root_dir_name = p.string();
    }

    tree_output << root_dir_name << "\n";
    buildTree(p, "", hide_hidden, tree_output);

    std::cout << "\n" << tree_output.str() << "\n";

    std::cout << "(Press 1 and Enter to copy this output to clipboard, or press any other key to exit): ";
    std::string input;
    std::getline(std::cin, input);

    if (input == "1") {
        copyToClipboard(tree_output.str());
    }

    return 0;
}
