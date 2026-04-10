import curses
import subprocess
import os
import threading
import queue
import time

def enqueue_output(out, q):
    try:
        for line in iter(out.readline, ''):
            if line:
                q.put(line)
    except ValueError:
        pass
    finally:
        out.close()

def get_flatpaks():
    try:
        result = subprocess.check_output(
            ['flatpak', 'list', '--app', '--columns=application,name'],
            text=True
        )
        apps = []
        for line in result.strip().split('\n'):
            if not line.strip(): continue
            parts = line.split('\t')
            if len(parts) >= 2:
                apps.append((parts[0].strip(), parts[1].strip()))
        return apps
    except Exception:
        return []

def get_debs():
    try:
        result = subprocess.check_output(
            ['dpkg-query', '-W', '-f=${Package}\t${Version}\n'],
            text=True
        )
        debs = []
        for line in result.strip().split('\n'):
            if not line.strip(): continue
            parts = line.split('\t')
            if len(parts) >= 2:
                debs.append((parts[0].strip(), parts[1].strip()))
        debs.sort(key=lambda x: x[0].lower())
        return debs
    except Exception:
        return []

def filter_items(items, query):
    if not query:
        return items
    query = query.lower()
    return [item for item in items if query in item[0].lower() or query in item[1].lower()]

def set_cursor_visibility(state):
    try:
        curses.curs_set(state)
    except curses.error:
        pass

def execute_command_ui(stdscr, cmd, process_name):
    max_y, max_x = stdscr.getmaxyx()
    stdscr.clear()

    if max_y < 10 or max_x < 50:
        stdscr.addstr(0, 0, "Please enlarge the terminal and press any key.")
        stdscr.getch()
        return

    stdscr.addstr(1, 2, f"Action: {process_name}", curses.A_BOLD)
    stdscr.addstr(2, 2, "-" * (max_x - 4))
    stdscr.refresh()

    try:
        process = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1
        )

        q = queue.Queue()
        t = threading.Thread(target=enqueue_output, args=(process.stdout, q))
        t.daemon = True
        t.start()

        spinner = ['|', '/', '-', '\\']
        spin_idx = 0
        log_lines = []
        log_max_lines = max(1, max_y - 6)

        stdscr.nodelay(1)

        while process.poll() is None or not q.empty():
            stdscr.addstr(1, max_x - 4, spinner[spin_idx], curses.A_BOLD)
            spin_idx = (spin_idx + 1) % len(spinner)

            while not q.empty():
                try:
                    line = q.get_nowait().strip()
                    if line:
                        log_lines.append(line)
                except queue.Empty:
                    break

            if len(log_lines) > log_max_lines:
                log_lines = log_lines[-log_max_lines:]

            for idx, log_line in enumerate(log_lines):
                display_line = log_line[:max_x-4]
                stdscr.addstr(4 + idx, 2, " " * (max_x - 4))
                stdscr.addstr(4 + idx, 2, display_line)

            stdscr.refresh()
            time.sleep(0.1)

        if process.returncode == 0:
            stdscr.addstr(1, max_x - 4, "OK", curses.A_BOLD)
            stdscr.addstr(max_y - 1, 2, " SUCCESS: Operation completed. Press any key to continue... ", curses.A_REVERSE)
        else:
            stdscr.addstr(1, max_x - 4, "X", curses.A_BOLD)
            stdscr.addstr(max_y - 1, 2, " ERROR: Operation failed. Press any key to continue... ", curses.A_REVERSE)

        stdscr.addstr(max_y - 1, max_x - 6, "Neuwj")

    except Exception as e:
        stdscr.addstr(4, 2, f"Error: {str(e)}")
        stdscr.addstr(max_y - 1, 2, " ERROR: Press any key to continue... ", curses.A_REVERSE)
        stdscr.addstr(max_y - 1, max_x - 6, "Neuwj")

    stdscr.nodelay(0)
    curses.flushinp()
    stdscr.getch()

def cleanup_menu(stdscr):
    set_cursor_visibility(0)
    selected_idx = 0

    thumb_path = os.path.expanduser('~/.cache/thumbnails')
    os.makedirs(thumb_path, exist_ok=True)

    options = [
        ("APT: Remove Unnecessary Dependencies (autoremove)", ["pkexec", "apt-get", "autoremove", "-y"]),
        ("APT: Clear Download Cache (clean)", ["pkexec", "apt-get", "clean"]),
        ("System: Clear Old Logs (Keep last 3 days)", ["pkexec", "journalctl", "--vacuum-time=3d"]),
        ("User: Clear Thumbnail Cache", ["find", thumb_path, "-type", "f", "-delete"]),
        ("Flatpak: Remove Unused Leftovers", ["flatpak", "uninstall", "--unused", "-y"])
    ]

    while True:
        max_y, max_x = stdscr.getmaxyx()
        stdscr.clear()

        if max_y < 12 or max_x < 50:
            stdscr.addstr(0, 0, "Please enlarge the terminal.")
            stdscr.refresh()
            stdscr.getch()
            continue

        title = " SYSTEM CLEANUP (OPTIMIZER) "
        title_x = max(0, (max_x - len(title)) // 2)
        stdscr.addstr(0, title_x, title, curses.A_REVERSE | curses.A_BOLD)

        stdscr.addstr(1, 2, "Up/Down: Select | [Enter]: Run | [Q]: Back", curses.A_BOLD)
        stdscr.addstr(2, 0, "-" * (max_x - 1))

        for i, (opt_name, _) in enumerate(options):
            if i == selected_idx:
                stdscr.addstr(4 + i, 2, f"> {opt_name}", curses.A_REVERSE | curses.A_BOLD)
            else:
                stdscr.addstr(4 + i, 2, f"  {opt_name}")

        stdscr.addstr(max_y - 1, max_x - 6, "Neuwj")
        stdscr.refresh()
        key = stdscr.getch()

        if key == curses.KEY_UP and selected_idx > 0:
            selected_idx -= 1
        elif key == curses.KEY_DOWN and selected_idx < len(options) - 1:
            selected_idx += 1
        elif key in [ord('q'), ord('Q')]:
            break
        elif key == curses.KEY_RESIZE:
            continue
        elif key in [curses.KEY_ENTER, 10, 13]:
            opt_name, cmd = options[selected_idx]

            stdscr.addstr(4 + selected_idx, 2, f"> {opt_name}", curses.color_pair(1) | curses.A_REVERSE | curses.A_BOLD)

            stdscr.addstr(max_y - 1, 0, " " * (max_x - 1))
            stdscr.addstr(max_y - 1, 2, f"Are you sure? (Y/N): ", curses.A_BOLD | curses.A_REVERSE)
            stdscr.addstr(max_y - 1, max_x - 6, "Neuwj")
            stdscr.refresh()

            while True:
                conf_key = stdscr.getch()
                if conf_key in [ord('y'), ord('Y')]:
                    execute_command_ui(stdscr, cmd, opt_name)
                    break
                elif conf_key in [ord('n'), ord('N')]:
                    break

def main(stdscr):
    curses.start_color()
    curses.use_default_colors()
    curses.init_pair(1, curses.COLOR_RED, -1)

    set_cursor_visibility(0)
    stdscr.nodelay(0)

    mode = 'flatpak'
    original_items = get_flatpaks()
    display_items = original_items
    search_query = ""

    selected_idx = 0
    scroll_y = 0

    while True:
        max_y, max_x = stdscr.getmaxyx()
        stdscr.clear()

        if max_y < 12 or max_x < 60:
            stdscr.addstr(0, 0, "Terminal is too small, please enlarge it.")
            stdscr.refresh()
            key = stdscr.getch()
            if key in [ord('q'), ord('Q')]:
                break
            continue

        title = " UNINSTALLER "
        title_x = max(0, (max_x - len(title)) // 2)
        stdscr.addstr(0, title_x, title, curses.A_REVERSE | curses.A_BOLD)

        clear_hint = " | [C] Clear" if search_query else ""
        if mode == 'flatpak':
            instr = f"Mode: FLATPAK | [D] DEB | [T] Tools | [S] Search{clear_hint} | [Enter] Uninstall | [Q] Quit"
        else:
            instr = f"Mode: DEB     | [F] Flatpak | [T] Tools | [S] Search{clear_hint} | [Enter] Uninstall | [Q] Quit"

        stdscr.addstr(1, 2, instr, curses.A_BOLD)
        stdscr.addstr(2, 0, "-" * (max_x - 1))

        if search_query:
            stdscr.addstr(3, 2, f"Search Filter: '{search_query}' (Press 'C' to clear)", curses.A_BOLD)
            list_start_y = 5
        else:
            list_start_y = 3

        list_height = max_y - list_start_y - 2

        if not display_items:
            if search_query:
                stdscr.addstr(list_start_y, 2, "No packages found matching your search...")
            else:
                stdscr.addstr(list_start_y, 2, "No installed packages found in this mode...")
        else:
            for i in range(list_height):
                item_idx = scroll_y + i
                if item_idx < len(display_items):
                    item = display_items[item_idx]

                    if mode == 'flatpak':
                        display_text = f"{item[1]} ({item[0]})"
                    else:
                        display_text = f"{item[0]} (Version: {item[1]})"

                    if len(display_text) > max_x - 4:
                        display_text = display_text[:max_x-7] + "..."

                    if item_idx == selected_idx:
                        stdscr.addstr(list_start_y + i, 2, display_text, curses.A_REVERSE | curses.A_BOLD)
                    else:
                        stdscr.addstr(list_start_y + i, 2, display_text)

        stdscr.addstr(max_y - 1, max_x - 6, "Neuwj")
        stdscr.refresh()
        key = stdscr.getch()

        if key == curses.KEY_UP:
            if selected_idx > 0:
                selected_idx -= 1
                if selected_idx < scroll_y:
                    scroll_y = selected_idx

        elif key == curses.KEY_DOWN:
            if selected_idx < len(display_items) - 1:
                selected_idx += 1
                if selected_idx >= scroll_y + list_height:
                    scroll_y += 1

        elif key == curses.KEY_PPAGE:
            selected_idx = max(0, selected_idx - list_height)
            if selected_idx < scroll_y:
                scroll_y = selected_idx

        elif key == curses.KEY_NPAGE:
            selected_idx = min(len(display_items) - 1, selected_idx + list_height)
            if selected_idx >= scroll_y + list_height:
                scroll_y = max(0, selected_idx - list_height + 1)

        elif key == curses.KEY_RESIZE:
            continue

        elif key in [ord('q'), ord('Q')]:
            break

        elif key in [ord('c'), ord('C')] and search_query:
            search_query = ""
            display_items = original_items
            selected_idx = 0
            scroll_y = 0

        elif key in [ord('s'), ord('S')]:
            stdscr.addstr(max_y - 1, 0, " " * (max_x - 1))
            stdscr.addstr(max_y - 1, 2, "Search: ", curses.A_BOLD | curses.A_REVERSE)
            stdscr.addstr(max_y - 1, max_x - 6, "Neuwj")

            curses.echo()
            set_cursor_visibility(1)
            try:
                search_input = stdscr.getstr(max_y - 1, 10).decode('utf-8').strip()
            except:
                search_input = ""
            curses.noecho()
            set_cursor_visibility(0)

            if search_input:
                search_query = search_input
                display_items = filter_items(original_items, search_query)
                selected_idx = 0
                scroll_y = 0

        elif key in [ord('t'), ord('T')]:
            cleanup_menu(stdscr)
            original_items = get_flatpaks() if mode == 'flatpak' else get_debs()
            display_items = filter_items(original_items, search_query)
            if selected_idx >= len(display_items):
                selected_idx = max(0, len(display_items) - 1)

        elif key in [ord('d'), ord('D')] and mode == 'flatpak':
            mode = 'deb'
            stdscr.clear()
            stdscr.addstr(max_y // 2, (max_x // 2) - 15, "Loading DEB packages, please wait...")
            stdscr.refresh()
            original_items = get_debs()
            display_items = filter_items(original_items, search_query)
            selected_idx = 0
            scroll_y = 0

        elif key in [ord('f'), ord('F')] and mode == 'deb':
            mode = 'flatpak'
            stdscr.clear()
            stdscr.addstr(max_y // 2, (max_x // 2) - 15, "Loading Flatpak packages...")
            stdscr.refresh()
            original_items = get_flatpaks()
            display_items = filter_items(original_items, search_query)
            selected_idx = 0
            scroll_y = 0

        elif key in [curses.KEY_ENTER, 10, 13] and display_items:
            item = display_items[selected_idx]
            app_name = item[1] if mode == 'flatpak' else item[0]
            app_id = item[0]

            display_text = f"{item[1]} ({item[0]})" if mode == 'flatpak' else f"{item[0]} (Version: {item[1]})"
            if len(display_text) > max_x - 4:
                display_text = display_text[:max_x-7] + "..."

            list_idx = selected_idx - scroll_y
            stdscr.addstr(list_start_y + list_idx, 2, display_text, curses.color_pair(1) | curses.A_REVERSE | curses.A_BOLD)

            prompt = f"Uninstalling: {app_name}. Are you sure? (Y/N): "
            stdscr.addstr(max_y - 1, 0, " " * (max_x - 1))
            stdscr.addstr(max_y - 1, 2, prompt, curses.A_BOLD | curses.A_REVERSE)
            stdscr.addstr(max_y - 1, max_x - 6, "Neuwj")
            stdscr.refresh()

            while True:
                conf_key = stdscr.getch()
                if conf_key in [ord('y'), ord('Y')]:
                    cmd = ['flatpak', 'uninstall', '-y', app_id] if mode == 'flatpak' else ['pkexec', 'apt-get', 'remove', '-y', app_id]
                    execute_command_ui(stdscr, cmd, f"Uninstalling: {app_name}")

                    stdscr.clear()
                    stdscr.refresh()

                    original_items = get_flatpaks() if mode == 'flatpak' else get_debs()
                    display_items = filter_items(original_items, search_query)

                    if selected_idx >= len(display_items):
                        selected_idx = max(0, len(display_items) - 1)
                    if scroll_y > selected_idx:
                        scroll_y = max(0, selected_idx - list_height + 1)
                    break

                elif conf_key in [ord('n'), ord('N')]:
                    break

if __name__ == "__main__":
    try:
        curses.wrapper(main)
    except KeyboardInterrupt:
        pass
    finally:
        print("\nMade by Neuwj\n")
