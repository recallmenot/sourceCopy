# SourceCopy

Terminal-based utility designed to streamline working with local source code alongside large language models (LLMs).
It allows you to browse directories, select files, and copy their contents to the clipboard —optionally preformatted in Markdown— for quick integration into LLM workflows.

## Features

- Navigate directories and select files using an ncurses-based TUI.
- Toggle between plain text and Markdown formatting.
- Copies selected file contents to the clipboard via xclip.
- Lightweight and fast, built with C++.

## Prerequisites

- g++ compiler
- ncurses library (e.g., libncurses5-dev on Debian/Ubuntu, ncurses on arch)
- xclip (for clipboard support)

## Installation

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/sourcecopy.git
   cd sourcecopy
   ```

2. Build the program:
   ```
   make
   ```

3. Install to ~/.local/bin:
   ```
   make install
   ```

4. (Optional) Add ~/.local/bin to your PATH in Zsh:
   ```
   make zsh_user_path_add
   ```
   Follow the prompt to source ~/.zshrc or restart your shell.

## Usage

With sourceCopy being available in your PATH, simply run it from your project's root dir:
```
sourceCopy
```
You'll be able to browse the project dir from here.
The upper window shows the current directory's contents ([D] for directories, [X] for selected files).
The lower window lists selected files.

### Keys

- **J/K/Down/Up**: Move cursor down/up in the file list.
- **H/Backspace**: Navigate to the parent directory.
- **L/Space**: Toggle file selection or enter a directory.
- **B**: Toggle Markdown formatting (ON by default).
- **Y/Enter**: Copy content of all selected files to the clipboard.
- **Q**: Quit.

## Example

Select a few source files, press Enter, and paste the clipboard contents into your LLM interface—formatted and ready to go!
