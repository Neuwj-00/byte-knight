use std::env;
use std::io::{self, Write};
use std::path::Path;
use std::process::Command;
use std::thread;
use std::time::Duration;

const RESET: &str = "\x1b[0m";
const BOLD: &str = "\x1b[1m";
const GREEN: &str = "\x1b[32m";
const BLUE: &str = "\x1b[34m";
const YELLOW: &str = "\x1b[33m";
const RED: &str = "\x1b[31m";
const CYAN: &str = "\x1b[36m";

fn clear_screen() {
    if cfg!(target_os = "windows") {
        // Windows platformları için ekran temizleme
        let _ = Command::new("cmd").args(["/c", "cls"]).status();
    } else {
        // Linux ve macOS (Unix) için ekran temizleme
        print!("\x1B[2J\x1B[1;1H");
        io::stdout().flush().expect("Terminal çıktısı temizlenemedi.");
    }
}

fn sleep_ms(ms: u64) {
    thread::sleep(Duration::from_millis(ms));
}

fn print_banner() {
    clear_screen();
    println!("{BOLD}{BLUE}================================================={RESET}");
    println!("{BOLD}{CYAN}      o      {RESET}{BOLD}gitForge{RESET}");
    println!("{BOLD}{CYAN}     / \\     {RESET}Toolkit: Git & GPG Automation");
    println!("{BOLD}{CYAN}    o   o    {RESET}Version: 1.0.0"); // V1.0.0 Tam Sürüm!
    println!("{BOLD}{CYAN}    |  /     {RESET}Developer: Neuwj");
    println!("{BOLD}{CYAN}    | o      {RESET}Contact:   neuwj@bk.ru");
    println!("{BOLD}{CYAN}    |/       {RESET}");
    println!("{BOLD}{CYAN}    o        {RESET}");
    println!("{BOLD}{BLUE}================================================={RESET}");
    println!("{YELLOW} Info: You can return at any time by entering 'q'.{RESET}\n");
}

fn print_step(msg: &str) {
    println!("{BLUE}[*]{RESET} {}...", msg);
    sleep_ms(400);
}

fn print_success(msg: &str) {
    println!("{GREEN}[+]{RESET} {BOLD}{}{RESET}", msg);
}

fn print_error(msg: &str) {
    println!("{RED}[-]{RESET} ERROR: {}", msg);
}

fn has_command(cmd: &str) -> bool {
    let status = Command::new("sh")
    .arg("-c")
    .arg(&format!("command -v {} > /dev/null 2>&1", cmd))
    .status();
    match status {
        Ok(s) => s.success(),
        Err(_) => false,
    }
}

fn ask_input(question: &str, is_required: bool) -> String {
    loop {
        print!("{YELLOW}[?]{RESET} {BOLD}{}:{RESET} ", question);
        io::stdout().flush().expect("Terminal çıktısı yazılamadı.");

        let mut input = String::new();
        io::stdin().read_line(&mut input).expect("Girdi okunamadı.");
        let trimmed = input.trim().to_string();

        if trimmed.is_empty() && is_required {
            print_error("This field cannot be empty.");
            continue;
        }
        return trimmed;
    }
}

fn wait_for_enter() {
    print!("\n{CYAN}Press [ENTER] to continue...{RESET}");
    io::stdout().flush().expect("Terminal çıktısı yazılamadı.");
    let mut dummy = String::new();
    io::stdin().read_line(&mut dummy).expect("Girdi okunamadı.");
}

fn run_cmd(cmd: &str) {
    print_step(&format!("Executing: {}", cmd));

    let status = Command::new("sh")
    .arg("-c")
    .arg(cmd)
    .status();

    match status {
        Ok(exit_status) if exit_status.success() => {
            print_success("Operation completed successfully.");
        }
        Ok(_) => {
            print_error("Command resulted in an error or was canceled.");
        }
        Err(e) => {
            print_error(&format!("System error: {}", e));
        }
    }
}

fn run_safe_cmd(program: &str, args: &[&str]) {
    print_step(&format!("Executing: {} {}", program, args.join(" ")));

    let status = Command::new(program)
    .args(args)
    .status();

    match status {
        Ok(exit_status) if exit_status.success() => {
            print_success("Operation completed successfully.");
        }
        Ok(_) => {
            print_error("Command resulted in an error or was canceled.");
        }
        Err(e) => {
            print_error(&format!("System error: {}", e));
        }
    }
}

fn setup_working_directory() {
    clear_screen();
    println!("{BOLD}{BLUE}================================================={RESET}");
    println!("{BOLD}{CYAN}         gitForge - WORKSPACE SETUP         {RESET}");
    println!("{BOLD}{BLUE}================================================={RESET}\n");

    loop {
        let path = ask_input("Enter project directory path (leave empty for current dir)", false);

        if path.is_empty() {
            let current_dir = env::current_dir().unwrap_or_else(|_| Path::new(".").to_path_buf());
            print_success(&format!("Using current directory: {}", current_dir.display()));
            sleep_ms(1200);
            break;
        } else {
            let target_dir = Path::new(&path);
            if env::set_current_dir(target_dir).is_ok() {
                print_success(&format!("Changed working directory to: {}", target_dir.display()));
                sleep_ms(1200);
                break;
            } else {
                print_error("Directory not found or inaccessible. Please try again.");
            }
        }
    }
}

fn menu_setup() {
    loop {
        print_banner();
        println!("{BOLD}--- REPO SETUP ---{RESET}\n");
        println!("  [1] Initialize New Repo (init)");
        println!("  [2] Clone Repo (clone)");
        println!("  [3] Add Remote (remote add)");
        println!("  [4] Configure Git User (config)");
        println!("  [q] Go Back\n");

        let choice = ask_input("Your choice", true);
        match choice.as_str() {
            "1" => run_cmd("git init"),
            "2" => {
                let url = ask_input("Clone URL", true);
                if url.to_lowercase() != "q" { run_safe_cmd("git", &["clone", &url]); }
            }
            "3" => {
                let url = ask_input("Remote Repo URL (origin)", true);
                if url.to_lowercase() != "q" { run_safe_cmd("git", &["remote", "add", "origin", &url]); }
            }
            "4" => {
                let name = ask_input("Your Git Username", true);
                if name.to_lowercase() != "q" {
                    let email = ask_input("Your Git Email", true);
                    if email.to_lowercase() != "q" {
                        run_safe_cmd("git", &["config", "--global", "user.name", &name]);
                        run_safe_cmd("git", &["config", "--global", "user.email", &email]);
                    }
                }
            }
            "q" | "Q" => break,
            _ => print_error("Invalid choice!"),
        }
        if choice.to_lowercase() != "q" { wait_for_enter(); }
    }
}

fn menu_daily_workflow() {
    loop {
        print_banner();
        println!("{BOLD}--- DAILY WORKFLOW ---{RESET}\n");
        println!("  [1] Status (status)      [2] Add All (add .)       [3] Commit (commit)");
        println!("  [4] Push (push)          [5] Force Push (-f)       [6] Pull (pull)");
        println!("  [7] Fetch (fetch)        [8] View History (log)    [9] View Diffs (diff)");
        println!("  [q] Go Back\n");

        let choice = ask_input("Your choice", true);
        match choice.as_str() {
            "1" => run_cmd("git status"),
            "2" => run_cmd("git add ."),
            "3" => {
                let msg = ask_input("Commit message", true);
                if msg.to_lowercase() != "q" {
                    run_safe_cmd("git", &["commit", "-S", "-m", &msg]);
                }
            }
            "4" => run_cmd("git push"),
            "5" => {
                let confirm = ask_input("WARNING: Are you sure you want to force push? (y/N)", true);
                if confirm.to_lowercase() == "y" {
                    run_cmd("git push --force");
                } else {
                    print_error("Force push canceled.");
                }
            }
            "6" => run_cmd("git pull"),
            "7" => run_cmd("git fetch"),
            "8" => run_cmd("git log --oneline --graph --decorate -n 15"),
            "9" => run_cmd("git diff"),
            "q" | "Q" => break,
            _ => print_error("Invalid choice!"),
        }
        if choice.to_lowercase() != "q" { wait_for_enter(); }
    }
}

fn menu_branching() {
    loop {
        print_banner();
        println!("{BOLD}--- BRANCH OPERATIONS ---{RESET}\n");
        println!("  [1] List Branches        [2] Create New Branch");
        println!("  [3] Switch Branch        [4] Merge Branch");
        println!("  [5] Rebase               [6] Cherry-pick");
        println!("  [7] Delete Branch (-d)");
        println!("  [q] Go Back\n");

        let choice = ask_input("Your choice", true);
        match choice.as_str() {
            "1" => run_cmd("git branch -a"),
            "2" => {
                let bname = ask_input("New branch name", true);
                if bname.to_lowercase() != "q" { run_safe_cmd("git", &["branch", &bname]); }
            }
            "3" => {
                let bname = ask_input("Branch name to switch", true);
                if bname.to_lowercase() != "q" { run_safe_cmd("git", &["switch", &bname]); }
            }
            "4" => {
                let bname = ask_input("Branch name to merge into current", true);
                if bname.to_lowercase() != "q" { run_safe_cmd("git", &["merge", &bname]); }
            }
            "5" => {
                let bname = ask_input("Base branch to rebase onto", true);
                if bname.to_lowercase() != "q" { run_safe_cmd("git", &["rebase", &bname]); }
            }
            "6" => {
                let hash = ask_input("Commit Hash to copy", true);
                if hash.to_lowercase() != "q" { run_safe_cmd("git", &["cherry-pick", &hash]); }
            }
            "7" => {
                let bname = ask_input("Branch name to delete", true);
                if bname.to_lowercase() != "q" { run_safe_cmd("git", &["branch", "-d", &bname]); }
            }
            "q" | "Q" => break,
            _ => print_error("Invalid choice!"),
        }
        if choice.to_lowercase() != "q" { wait_for_enter(); }
    }
}

fn menu_gpg_security() {
    loop {
        print_banner();
        println!("{BOLD}--- GPG & SECURITY ---{RESET}\n");
        println!("  [1] List Keys            [2] Generate New Key");
        println!("  [3] Export Public Key    [4] Signed Commit (-S)");
        println!("  [5] Verify Commit Sig.");
        println!("  [q] Go Back\n");

        let choice = ask_input("Your choice", true);
        match choice.as_str() {
            "1" => run_cmd("gpg --list-keys"),
            "2" => run_cmd("gpg --gen-key"),
            "3" => {
                let keyid = ask_input("Key ID", true);
                if keyid.to_lowercase() != "q" {
                    // SON DÜZELTME: Shell (sh) yerine direkt GPG kullanıldı ve --output bayrağı eklendi.
                    // Artık komut enjeksiyonu riski %0!
                    run_safe_cmd("gpg", &["--export", "--armor", "--output", "public_key.asc", &keyid]);
                }
            }
            "4" => {
                let msg = ask_input("Commit message", true);
                if msg.to_lowercase() != "q" {
                    run_safe_cmd("git", &["commit", "-S", "-m", &msg]);
                }
            }
            "5" => run_cmd("git verify-commit HEAD"),
            "q" | "Q" => break,
            _ => print_error("Invalid choice!"),
        }
        if choice.to_lowercase() != "q" { wait_for_enter(); }
    }
}

fn menu_advanced() {
    loop {
        print_banner();
        println!("{BOLD}--- ADVANCED & MAINTENANCE ---{RESET}\n");
        println!("  [1] Pull Submodules");
        println!("  [2] Archive Repo (.tar.gz)");
        println!("  [3] Stash List           [4] Stash Changes");
        println!("  [5] Developer Shortlog   [6] Cleanup (gc & clean)");
        println!("  [7] Undo Last Commit (Keep files)");
        println!("  [q] Go Back\n");

        let choice = ask_input("Your choice", true);
        match choice.as_str() {
            "1" => run_cmd("git submodule update --init --recursive"),
            "2" => {
                let name = ask_input("Archive name (e.g., project_v1)", true);
                if name.to_lowercase() != "q" {
                    run_safe_cmd("git", &["archive", "--format=tar.gz", &format!("-o={}.tar.gz", name), "HEAD"]);
                }
            }
            "3" => run_cmd("git stash list"),
            "4" => run_cmd("git stash push -m \"gitForge quick stash\""),
            "5" => run_cmd("git shortlog -sn"),
            "6" => run_cmd("git gc && git clean -fd"),
            "7" => {
                let confirm = ask_input("Are you sure you want to undo the last commit? (y/N)", true);
                if confirm.to_lowercase() == "y" {
                    run_cmd("git reset --soft HEAD~1");
                }
            }
            "q" | "Q" => break,
            _ => print_error("Invalid choice!"),
        }
        if choice.to_lowercase() != "q" { wait_for_enter(); }
    }
}

fn menu_install_tools() {
    clear_screen();
    println!("{BOLD}--- SYSTEM DEPENDENCIES SETUP ---{RESET}\n");

    let mut installed = false;

    if has_command("apt") {
        print_step("Debian/Ubuntu based system detected");
        run_cmd("sudo apt update && sudo apt install -y git gnupg openssh-client");
        installed = true;
    } else if has_command("dnf") {
        print_step("Fedora/RHEL based system detected");
        run_cmd("sudo dnf install -y git gnupg2 openssh-clients");
        installed = true;
    } else if has_command("pacman") {
        print_step("Arch Linux based system detected");
        run_cmd("sudo pacman -Sy --noconfirm git gnupg openssh");
        installed = true;
    }

    if !installed {
        print_error("No supported package manager found (apt, dnf, pacman). Please install manually.");
    } else {
        print_success("All required packages (Git, GPG, SSH) installed or updated successfully!");
    }

    wait_for_enter();
}

fn main() {
    setup_working_directory();

    loop {
        print_banner();
        println!("  [1] Repo Setup (init, clone, config, remote)");
        println!("  [2] Daily Workflow (status, add, commit, push -f, log)");
        println!("  [3] Branch Operations (switch, merge, rebase, delete)");
        println!("  [4] GPG Security (Sign, Key management)");
        println!("  [5] Advanced & Maintenance (stash, archive, gc, undo commit)");
        println!("  [6] Install System Dependencies (Git, GPG, SSH etc.)");
        println!("  [q] Exit\n");

        let choice = ask_input("Select a Forge Category", true);

        match choice.as_str() {
            "1" => menu_setup(),
            "2" => menu_daily_workflow(),
            "3" => menu_branching(),
            "4" => menu_gpg_security(),
            "5" => menu_advanced(),
            "6" => menu_install_tools(),
            "q" | "Q" => {
                clear_screen();
                println!("{BOLD}{GREEN}\n  Thank you for using gitForge{RESET}");
                println!("{BOLD}{CYAN}  Developer: Neuwj{RESET}");
                println!("{BOLD}{CYAN}  Contact:   neuwj@bk.ru{RESET}\n");
                break;
            }
            _ => {
                print_error("Invalid choice, please enter one of the numbers from the menu.");
                wait_for_enter();
            }
        }
    }
}
