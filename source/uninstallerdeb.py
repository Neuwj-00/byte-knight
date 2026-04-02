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

def main(stdscr):
    curses.curs_set(0)
    stdscr.nodelay(0)
    curses.use_default_colors()

    mode = 'flatpak'
    original_items = get_flatpaks()
    display_items = original_items
    search_query = ""

    selected_idx = 0
    scroll_y = 0

    while True:
        max_y, max_x = stdscr.getmaxyx()
        stdscr.clear()

        title = " UNINSTALLER "
        title_x = max(0, (max_x - len(title)) // 2)
        stdscr.addstr(0, title_x, title, curses.A_REVERSE | curses.A_BOLD)

        clear_hint = " | [C] Clear" if search_query else ""
        if mode == 'flatpak':
            instr = f"Mode: FLATPAK | [D] DEB | [S] Search{clear_hint} | [Enter] Uninstall | [Q] Quit"
        else:
            instr = f"Mode: DEB     | [F] Flatpak | [S] Search{clear_hint} | [Enter] Uninstall | [Q] Quit"

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

            curses.echo()
            curses.curs_set(1)

            search_input = stdscr.getstr(max_y - 1, 10).decode('utf-8').strip()

            curses.noecho()
            curses.curs_set(0)

            if search_input:
                search_query = search_input
                display_items = [
                    item for item in original_items
                    if search_query.lower() in item[0].lower() or search_query.lower() in item[1].lower()
                ]
                selected_idx = 0
                scroll_y = 0

        elif key in [ord('d'), ord('D')] and mode == 'flatpak':
            mode = 'deb'
            stdscr.clear()
            stdscr.addstr(max_y // 2, (max_x // 2) - 15, "Loading DEB packages, please wait...")
            stdscr.refresh()
            original_items = get_debs()
            display_items = original_items
            search_query = ""
            selected_idx = 0
            scroll_y = 0

        elif key in [ord('f'), ord('F')] and mode == 'deb':
            mode = 'flatpak'
            stdscr.clear()
            stdscr.addstr(max_y // 2, (max_x // 2) - 15, "Loading Flatpak packages...")
            stdscr.refresh()
            original_items = get_flatpaks()
            display_items = original_items
            search_query = ""
            selected_idx = 0
            scroll_y = 0

        elif key in [curses.KEY_ENTER, 10, 13] and display_items:
            item = display_items[selected_idx]
            app_name = item[1] if mode == 'flatpak' else item[0]
            app_id = item[0]

            prompt = f"Uninstall {app_name}? Y/N: "
            stdscr.addstr(max_y - 1, 0, " " * (max_x - 1))
            stdscr.addstr(max_y - 1, 2, prompt, curses.A_BOLD | curses.A_REVERSE)
            stdscr.refresh()

            while True:
                conf_key = stdscr.getch()
                if conf_key in [ord('y'), ord('Y')]:
                    stdscr.clear()
                    stdscr.addstr(1, 2, f"Uninstalling: {app_name}", curses.A_BOLD)
                    stdscr.addstr(2, 2, "-" * (max_x - 4))
                    stdscr.refresh()

                    cmd = []
                    if mode == 'flatpak':
                        cmd = ['flatpak', 'uninstall', '-y', app_id]
                    else:
                        cmd = ['pkexec', 'apt-get', 'remove', '-y', app_id]

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
                        log_max_lines = max_y - 6

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

                        stdscr.addstr(1, max_x - 4, "✓", curses.A_BOLD)
                        if process.returncode == 0:
                            stdscr.addstr(max_y - 1, 2, " SUCCESS: Operation completed. Press any key... ", curses.A_REVERSE)
                        else:
                            stdscr.addstr(max_y - 1, 2, " FAILED: Operation aborted or errored. Press any key... ", curses.A_REVERSE)

                    except Exception as e:
                        stdscr.addstr(4, 2, f"Error: {str(e)}")
                        stdscr.addstr(max_y - 1, 2, " Press any key to return... ", curses.A_REVERSE)

                    stdscr.nodelay(0)
                    curses.flushinp()
                    stdscr.getch()

                    stdscr.clear()
                    stdscr.refresh()

                    if mode == 'flatpak':
                        original_items = get_flatpaks()
                    else:
                        original_items = get_debs()

                    if search_query:
                        display_items = [
                            item for item in original_items
                            if search_query.lower() in item[0].lower() or search_query.lower() in item[1].lower()
                        ]
                    else:
                        display_items = original_items

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
