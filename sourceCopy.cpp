#include <ncurses.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

class SourceCopy {
private:
    WINDOW* upper_win;
    WINDOW* lower_win;
    std::vector<std::pair<std::string, bool>> dir_contents;
    std::vector<std::string> selected_files;
    size_t upper_cursor, upper_scroll;
    size_t lower_cursor, lower_scroll;
    int max_y, max_x;
    bool use_markdown = true;
    std::string base_dir;
    std::string current_dir;
    std::string last_dir_name;

    void resizeWindows() {
        getmaxyx(stdscr, max_y, max_x);
        wresize(upper_win, max_y / 2, max_x);
        wresize(lower_win, max_y / 2, max_x);
        mvwin(lower_win, max_y / 2, 0);
    }

    void refreshUpper() {
        werase(upper_win);
        box(upper_win, 0, 0);
        int display_height = max_y / 2 - 2;
        for (size_t i = upper_scroll; i < dir_contents.size() && i < upper_scroll + static_cast<size_t>(display_height); i++) {
            if (i == upper_cursor) wattron(upper_win, A_REVERSE);
            std::string full_path = current_dir == "." ? dir_contents[i].first : current_dir + "/" + dir_contents[i].first;
            bool is_selected = std::find(selected_files.begin(), selected_files.end(), full_path) != selected_files.end();
            mvwprintw(upper_win, i - upper_scroll + 1, 1, "%s %s", 
                     dir_contents[i].second ? "[D]" : (is_selected ? "[X]" : "[ ]"), 
                     dir_contents[i].first.c_str());
            if (i == upper_cursor) wattroff(upper_win, A_REVERSE);
        }
        mvwprintw(upper_win, 0, 1, "Dir: %s", current_dir.c_str());
        wrefresh(upper_win);
    }

    void refreshLower() {
        werase(lower_win);
        box(lower_win, 0, 0);
        int display_height = max_y / 2 - 2;
        for (size_t i = lower_scroll; i < selected_files.size() && i < lower_scroll + static_cast<size_t>(display_height); i++) {
            if (i == lower_cursor) wattron(lower_win, A_REVERSE);
            mvwprintw(lower_win, i - lower_scroll + 1, 1, "%s", selected_files[i].c_str());
            if (i == lower_cursor) wattroff(lower_win, A_REVERSE);
        }
        mvwprintw(lower_win, 0, 1, "Selected Files");
        if (current_dir == ".") {
            mvwprintw(lower_win, max_y / 2 - 1, 1, "J/K/Down/Up: Down/Up | L/Space: Toggle File/Enter Dir | B: Markdown [%s] | Y/Enter: Copy Contents | Q: Quit",
                     use_markdown ? "ON" : "OFF");
        } else {
            mvwprintw(lower_win, max_y / 2 - 1, 1, "J/K/Down/Up: Down/Up | H/Backspace: Parent Dir | L/Space: Toggle File/Enter Dir | B: Markdown [%s] | Y/Enter: Copy Contents | Q: Quit",
                     use_markdown ? "ON" : "OFF");
        }
        wrefresh(lower_win);
    }

    void updateDirContents() {
        dir_contents.clear();
        if (current_dir != ".") {
            dir_contents.push_back({"..", true});
        }
        DIR* dir = opendir(".");
        if (dir) {
            struct dirent* ent;
            struct stat st;
            while ((ent = readdir(dir)) != NULL) {
                std::string name = ent->d_name;
                if (name != "." && name != "..") {
                    if (stat(name.c_str(), &st) == 0) {
                        dir_contents.push_back({name, S_ISDIR(st.st_mode)});
                    }
                }
            }
            closedir(dir);
            std::sort(dir_contents.begin() + (current_dir != "." ? 1 : 0), dir_contents.end());
        }
        if (!last_dir_name.empty()) {
            for (size_t i = 0; i < dir_contents.size(); i++) {
                if (dir_contents[i].first == last_dir_name) {
                    upper_cursor = i;
                    if (upper_cursor >= static_cast<size_t>(max_y / 2 - 2)) {
                        upper_scroll = upper_cursor - (max_y / 2 - 3);
                    }
                    break;
                }
            }
            last_dir_name.clear();
        }
    }

    void copyToClipboard() {
        if (!selected_files.empty()) {
            std::stringstream command;
            command << "{ ";
            for (size_t i = 0; i < selected_files.size(); i++) {
                std::string fname = selected_files[i];
                size_t pos = 0;
                while ((pos = fname.find_first_of("'\\", pos)) != std::string::npos) {
                    fname.insert(pos, "\\");
                    pos += 2;
                }
                if (i > 0) command << " && ";
                if (use_markdown) {
                    command << "echo '``` " << fname << "' && ";
                } else {
                    command << "echo '=== " << fname << " ===' && ";
                }
                command << "cat '" << fname << "' && ";
                if (use_markdown) {
                    command << "echo '```' && ";
                }
                command << "printf '\\n\\n\\n'";
            }
            command << "; } | xclip -selection clipboard";
            system(command.str().c_str());
            mvwprintw(lower_win, max_y / 2 - 2, 1, "Copied to clipboard!");
        } else {
            mvwprintw(lower_win, max_y / 2 - 2, 1, "No files selected!");
        }
        wrefresh(lower_win);
        napms(1000);
    }

    void navigateUp() {
        if (current_dir != ".") {
            std::string current_name = current_dir.substr(current_dir.find_last_of('/') + 1);
            if (current_name.empty()) current_name = current_dir;
            last_dir_name = current_name;
            chdir("..");
            current_dir = std::string(getcwd(nullptr, 0)).substr(base_dir.length());
            if (current_dir.empty()) current_dir = ".";
            else if (current_dir[0] == '/') current_dir.erase(0, 1);
            updateDirContents();
        }
    }

public:
    SourceCopy() : upper_cursor(0), upper_scroll(0), lower_cursor(0), lower_scroll(0) {
        initscr();
        clear();
        refresh();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        
        getmaxyx(stdscr, max_y, max_x);
        upper_win = newwin(max_y / 2, max_x, 0, 0);
        lower_win = newwin(max_y / 2, max_x, max_y / 2, 0);

        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd))) {
            base_dir = cwd;
        }
        current_dir = ".";
        updateDirContents();
    }

    ~SourceCopy() {
        delwin(upper_win);
        delwin(lower_win);
        endwin();
    }

    void run() {
        refreshUpper();
        refreshLower();
        doupdate();

        int ch;
        while ((ch = getch()) != 'q' && ch != 'Q') {
            switch (ch) {
                case KEY_RESIZE:
                    resizeWindows();
                    break;
                case KEY_UP:
                case 'k':
                    if (upper_cursor > 0) {
                        upper_cursor--;
                        if (upper_cursor < upper_scroll) {
                            upper_scroll--;
                        }
                    }
                    break;
                case KEY_DOWN:
                case 'j':
                    if (upper_cursor < dir_contents.size() - 1) {
                        upper_cursor++;
                        if (upper_cursor >= upper_scroll + static_cast<size_t>(max_y / 2 - 2)) {
                            upper_scroll++;
                        }
                    }
                    break;
                case 'h':
                case KEY_BACKSPACE:
                    navigateUp();
                    break;
                case ' ':
                case 'l':
                    if (!dir_contents.empty()) {
                        if (dir_contents[upper_cursor].second) {
                            std::string new_dir = dir_contents[upper_cursor].first;
                            if (new_dir == "..") {
                                navigateUp();
                            } else {
                                last_dir_name.clear();
                                chdir(new_dir.c_str());
                                current_dir = std::string(getcwd(nullptr, 0)).substr(base_dir.length());
                                if (current_dir.empty()) current_dir = ".";
                                else if (current_dir[0] == '/') current_dir.erase(0, 1);
                                updateDirContents();
                                upper_cursor = upper_scroll = 0;
                            }
                        } else {
                            std::string full_path = current_dir == "." ? 
                                                  dir_contents[upper_cursor].first : 
                                                  current_dir + "/" + dir_contents[upper_cursor].first;
                            auto it = std::find(selected_files.begin(), selected_files.end(), full_path);
                            if (it == selected_files.end()) {
                                selected_files.push_back(full_path);
                            } else {
                                selected_files.erase(it);
                            }
                            if (lower_cursor >= selected_files.size()) lower_cursor = selected_files.size() ? selected_files.size() - 1 : 0;
                        }
                    }
                    break;
                case '\n':
                case 'y':
                    copyToClipboard();
                    break;
                case 'b':
                case 'B':
                    use_markdown = !use_markdown;
                    break;
            }
            refreshUpper();
            refreshLower();
        }
    }
};

int main() {
    SourceCopy selector;
    selector.run();
    return 0;
}
