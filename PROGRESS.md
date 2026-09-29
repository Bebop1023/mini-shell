# Mini Shell (C++): Progress

## Current status
- **Milestone:** M0. Echo loop
- **Last completed step:** M0.2 reads a line with `getline` and echoes it
- **Next step:** M0.3 + M0.4: loop forever, quit on `exit`
- **Repo:** https://github.com/Bebop1023/mini-shell
- **Deadline:** M7 by Oct 4, 2026 (NVIDIA Ignite application)

### Files in the project
| File | What it does |
|------|--------------|
| `PROGRESS.md` | This file. Tracks progress, walls, and concepts (Claude maintains it) |
| `main.cpp` | The shell's source code. `main()` prints the prompt, reads one line, echoes it |
| `mysh`, `mysh.dSYM/` | Compiled program + debug info (build output, git-ignored) |
| `.gitignore` | Keeps the compiled binary and debug files out of git |

### Build command
```
clang++ -std=c++20 -Wall -Wextra -g -fsanitize=address main.cpp -o mysh
./mysh
```
Toolchain: Command Line Tools, Apple clang 21 (`xcode-select -s /Library/Developer/CommandLineTools`).

---

## Milestone checklist

### M0. Echo loop
- [x] M0.0 Create the git repo and `.gitignore`
- [x] M0.1 Print the prompt `mysh> ` (and flush it)
- [x] M0.2 Read a line with `std::getline` and print it back
- [ ] M0.3 Loop forever: prompt, read, echo, repeat
- [ ] M0.4 Quit on `exit`
- [ ] M0.5 Quit cleanly on Ctrl+D (end of input)

### M1. Split the line into words
- [ ] M1.1 Write `split()` using `std::istringstream`, returning `std::vector<std::string>`
- [ ] M1.2 Print each word in `[brackets]` to prove extra spaces are handled
- [ ] M1.3 Skip empty lines and lines with only spaces

### M2. Run one command
- [ ] M2.1 `fork()` and print which process is the parent and which is the child
- [ ] M2.2 Convert `vector<string>` into a `char*` argv array ending in `nullptr`
- [ ] M2.3 Call `execvp()` in the child
- [ ] M2.4 Call `waitpid()` in the parent so the prompt comes back after the command
- [ ] M2.5 Test with `ls`, `ls -l`, `echo hello world`, `pwd`

### M3. Bad commands don't kill the shell
- [ ] M3.1 Check `execvp`'s return value and print an error with `perror`
- [ ] M3.2 `_exit(127)` in the child after a failed exec (no duplicate shells)
- [ ] M3.3 Handle `fork()` failing
- [ ] M3.4 Prove it: type a bad command, then `exit` once and the shell really quits

### M4. Built-ins: cd and exit
- [ ] M4.1 Understand why `cd` can't run in a child process
- [ ] M4.2 Check for built-ins before forking
- [ ] M4.3 `cd <dir>` using `chdir()`
- [ ] M4.4 `cd` with no argument goes to `$HOME`
- [ ] M4.5 Error messages for bad directories

### M5. Pipes: cmd1 | cmd2
- [ ] M5.1 Split the words at `|` into two commands
- [ ] M5.2 Create a pipe with `pipe()`
- [ ] M5.3 Fork two children and wire them up with `dup2()`
- [ ] M5.4 Close every unused pipe end (so the reader sees end-of-file)
- [ ] M5.5 Wait for both children
- [ ] M5.6 Test: `ls | wc -l`, `echo hello | tr a-z A-Z`

### M6. Stretch: output redirection
- [ ] M6.1 Detect `> filename` in the words
- [ ] M6.2 `open()` the file with the right flags and permissions
- [ ] M6.3 `dup2()` the file onto stdout in the child
- [ ] M6.4 Handle errors (missing filename, can't open the file)

### M7. Ship
- [ ] M7.1 Makefile (`make`, `make debug`, `make clean`)
- [ ] M7.2 Clean AddressSanitizer run through every feature
- [ ] M7.3 README: what it does, how to build, the 3 hardest walls
- [ ] M7.4 Final commit and push to GitHub

---

## What I built (step log)
| Date | Step | What the code does now | File / function | Concept |
|------|------|------------------------|-----------------|---------|
| 2026-09-29 | Setup | Checked clang++ and git, created PROGRESS.md | none | Toolchain |
| 2026-09-29 | M0.0 | Local git repo on branch `main`; `.gitignore` ignores `mysh`, `mysh.dSYM/`, `.vscode/` | `.gitignore` | git init, ignoring build output |
| 2026-09-29 | M0.1 | Prints `mysh> ` and exits | `main.cpp` / `main()` | `#include`, `std::cout`, flushing buffered output, exit status |
| 2026-09-29 | Git/GitHub | First commit pushed to public repo `Bebop1023/mini-shell` | none | Local vs. remote, `git remote add`, `git push -u`, personal access token |
| 2026-09-29 | M0.2 | Reads one full line from the keyboard and prints `You typed: <line>` | `main.cpp` / `main()` | `std::string`, `std::getline` (reads whole line) vs `cin >>` (one word), pass by reference |

---

## Walls I hit
*(Format: what broke, why, the fix, what I learned. ⭐ = hardest, goes in the README.)*

**1. Linker error: `Undefined symbols: "_main"` (2026-09-29)**
- **What broke:** `ld: symbol(s) not found for architecture arm64`, with no line number.
- **Why:** `main.cpp` was 0 bytes because the code was typed but never saved. The compiler reads the file on disk, not the editor. With no `main()`, the *linker* (not the compiler) failed.
- **Fix:** Save (Cmd+S), check with `cat main.cpp`, recompile. Turned on Auto Save.
- **Learned:** Building is compile, then link. Errors starting with `ld:` come from the linker and have no line numbers.

**⭐ 2. AddressSanitizer build hangs forever at startup (2026-09-29)**
- **What broke:** `./mysh` printed nothing and spun at 99% CPU. Even `mysh> ` never appeared.
- **Why:** The ASan runtime starts *before* `main()`. macOS 26.5.1 with Xcode 26.3 (older than the OS) left it stuck during startup. Proved by building the same file with and without `-fsanitize=address`: without it, it worked instantly.
- **Fix:** Updated Command Line Tools (clang 21), then ran `sudo xcode-select -s /Library/Developer/CommandLineTools` so `clang++` uses them instead of the older Xcode app. ASan works again.
- **Learned:** When the "obviously correct" code fails, change one variable at a time (here, one compiler flag) to isolate the cause. Also: Ctrl+C kills a stuck program, and code can run before `main()`.

---

## Concepts learned
*(Written in my own words.)*

- **How a shell runs a program:** zsh makes a copy of itself (fork), the copy turns into my program (exec), and the original waits for it to finish (wait) before showing the prompt again.
- **`getline` vs `cin >>`:** `getline` reads the whole line including spaces. `cin >>` stops at the first space, so `hello world` would give just `hello`.
- **Compile vs link:** compiling turns `.cpp` into machine code. Linking joins the pieces into one program and connects `main`. `ld:` errors come from the linker.

---

## Git commits
| Milestone | Commit message |
|-----------|----------------|
| M0.1 | `M0.1: print shell prompt` (d5576c6) |
| M0.2 | `M0.2: read a line with getline and echo it` (0d93da8) |
