# SourceCopy

SourceCopy is a terminal-based utility designed to streamline working with local source code alongside large language models (LLMs). It allows you to browse directories, select files, and copy their contents to the clipboard—optionally formatted in Markdown—for quick integration into LLM workflows.

## Features

- Navigate directories and select files using an ncurses-based TUI.
- Toggle between plain text and Markdown formatting.
- Copies selected file contents to the clipboard via xclip.
- Lightweight and fast, built with C++.

## Prerequisites

- g++ compiler
- ncurses library (e.g., libncurses5-dev on Debian/Ubuntu)
- xclip (for clipboard support)
- Zsh (optional, for PATH setup in ~/.zshrc)

## Installation

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/sourcecopy.git_
   cd sourcecopy_
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
   make zsh_user_path_add_
   ```
   Follow the prompt to source ~/.zshrc or restart your shell.

## Usage

Run the program:
```
sourceCopy
```

- **J/K**: Move cursor up/down in the file list.
- **H**: Navigate up one directory.
- **Space/L**: Toggle file selection or enter a directory.
- **B**: Toggle Markdown formatting (ON by default).
- **Enter**: Copy selected file contents to the clipboard.
- **Q**: Quit.

The upper window shows the current directory's contents ([D] for directories, [X] for selected files). The lower window lists selected files.

## Example

Select a few source files, press Enter, and paste the clipboard contents into your LLM interface—formatted and ready to go!

## License

MIT License. See LICENSE file for details.

## Contributing

Pull requests and issues are welcome! Feel free to improve features or add support for other shells beyond Zsh.
