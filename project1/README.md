# My Custom Shell

A simple Unix-style shell written in C++. It supports running system commands, basic built-in commands, and tracks signals sent by the user.

## Features

- Execute any valid system command (using `fork()` and `execvp()`).
- Built-in commands:
  - `help` — display shell information
  - `history` — show the last 5 child process IDs
  - `exit` — print signal counts and exit
- Tracks user signals: Ctrl+C, Ctrl+\, and Ctrl+Z.
- Stores recent process IDs (up to 5).
- Handles Ctrl+D (EOF) to exit cleanly.

## Build

```bash
g++ -o simpleshell simpleshell.cpp
