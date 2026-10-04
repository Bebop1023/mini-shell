# Mini Shell (C++): Progress

## Current status
- **Milestone:** M5. Pipes (M0–M4 done ✅)
- **Last completed step:** Moved exec code into `runCommand()` (prep for running two commands)
- **Next step:** M5.2–M5.5: write the pipe code (`pipe`, two forks, `dup2`, close ends, wait for both)
- **Repo:** https://github.com/Bebop1023/mini-shell
- **Hours:** Session 1 (2026-09-29): 1.5 h
- **Deadline:** Apply between Oct 5 and Oct 18, 2026 (NVIDIA Ignite). No benefit to applying early, so aim for a polished project. Target: M9 done before applying.

### Files in the project
| File | What it does |
|------|--------------|
| `PROGRESS.md` | This file. Tracks progress, walls, and concepts (Claude maintains it) |
| `main.cpp` | `runCommand()` builds argv and execs (child only, never returns). `main()`: prompt, read, `parsePipeline`, built-ins (`exit`, `cd`), then `fork` + `runCommand` + `waitpid` |
| `parser.h` | Declarations of the parsing functions (the "menu") and the `Pipeline` struct (commands + error) |
| `parser.cpp` | Parsing code: `split()` breaks a line into words; `parsePipeline()` splits words at `|` into commands and reports syntax errors. No fork/exec, so the fuzzer can test it safely |
| `mysh`, `mysh.dSYM/` | Compiled program + debug info (build output, git-ignored) |
| `.gitignore` | Keeps the compiled binary and debug files out of git |

### Build command
```
clang++ -std=c++20 -Wall -Wextra -g -fsanitize=address main.cpp parser.cpp -o mysh
./mysh
```
Toolchain: Command Line Tools, Apple clang 21 (`xcode-select -s /Library/Developer/CommandLineTools`).

---

## Milestone checklist

### M0. Echo loop
- [x] M0.0 Create the git repo and `.gitignore`
- [x] M0.1 Print the prompt `mysh> ` (and flush it)
- [x] M0.2 Read a line with `std::getline` and print it back
- [x] M0.3 Loop forever: prompt, read, echo, repeat
- [x] M0.4 Quit on `exit`
- [x] M0.5 Quit cleanly on Ctrl+D (end of input)

### M1. Split the line into words
- [x] M1.1 Write `split()` using `std::istringstream`, returning `std::vector<std::string>`
- [x] M1.2 Print each word in `[brackets]` to prove extra spaces are handled
- [x] M1.3 Skip empty lines and lines with only spaces

### M2. Run one command
- [x] M2.1 `fork()` and print which process is the parent and which is the child
- [x] M2.2 Convert `vector<string>` into a `char*` argv array ending in `nullptr`
- [x] M2.3 Call `execvp()` in the child
- [x] M2.4 Call `waitpid()` in the parent so the prompt comes back after the command
- [x] M2.5 Test with `ls`, `ls -l`, `echo hello world`, `pwd`

### M3. Bad commands don't kill the shell
- [x] M3.1 Check `execvp`'s return value and print an error with `perror`
- [x] M3.2 `_exit(127)` in the child after a failed exec (no duplicate shells)
- [x] M3.3 Handle `fork()` failing
- [x] M3.4 Prove it: type a bad command, then `exit` once and the shell really quits

### M4. Built-ins: cd and exit
- [x] M4.1 Understand why `cd` can't run in a child process
- [x] M4.2 Check for built-ins before forking
- [x] M4.3 `cd <dir>` using `chdir()`
- [x] M4.4 `cd` with no argument goes to `$HOME`
- [x] M4.5 Error messages for bad directories

### M5. Pipes: cmd1 | cmd2
- [x] M5.0 Move `split()` into `parser.h` / `parser.cpp` (so the fuzzer can test parsing without running commands)
- [x] M5.1 Split the words at `|` into two commands (bad input like `| ls`, `ls |` gives an error)
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

### M8. Fuzzing (added 2026-10-03)
- [ ] M8.1 Install full LLVM (`brew install llvm`). Apple clang has no libFuzzer.
- [ ] M8.2 Write `fuzz_parser.cpp` with `LLVMFuzzerTestOneInput` that calls only the parser (never fork/exec)
- [ ] M8.3 Build with `-fsanitize=fuzzer,address` and run it
- [ ] M8.4 Fix every crash it finds; log each one in Walls
- [ ] M8.5 Save crash inputs as regression tests and keep a seed corpus

### M9. Tests + CI (added 2026-10-03)
- [ ] M9.1 Test script: feed commands to `./mysh` and check the output
- [ ] M9.2 GitHub Actions workflow: build with ASan and run tests on every push
- [ ] M9.3 Run the fuzzer for 60 seconds in CI
- [ ] M9.4 CI badge and fuzzing results in the README

---

## What I built (step log)
| Date | Step | What the code does now | File / function | Concept |
|------|------|------------------------|-----------------|---------|
| 2026-09-29 | Setup | Checked clang++ and git, created PROGRESS.md | none | Toolchain |
| 2026-09-29 | M0.0 | Local git repo on branch `main`; `.gitignore` ignores `mysh`, `mysh.dSYM/`, `.vscode/` | `.gitignore` | git init, ignoring build output |
| 2026-09-29 | M0.1 | Prints `mysh> ` and exits | `main.cpp` / `main()` | `#include`, `std::cout`, flushing buffered output, exit status |
| 2026-09-29 | Git/GitHub | First commit pushed to public repo `Bebop1023/mini-shell` | none | Local vs. remote, `git remote add`, `git push -u`, personal access token |
| 2026-09-29 | M0.2 | Reads one full line from the keyboard and prints `You typed: <line>` | `main.cpp` / `main()` | `std::string`, `std::getline` (reads whole line) vs `cin >>` (one word), pass by reference |
| 2026-09-29 | M0.3–M0.4 | Loops forever; `break`s when the line is exactly `exit` | `main.cpp` / `main()` | `while (true)`, `break`, `std::string ==` compares text |
| 2026-09-29 | M0.5 | Quits on Ctrl+D instead of looping forever | `main.cpp` / `main()` | EOF, stream failure state, `if (!getline(...))` |
| 2026-09-29 | M1.1 | `split()` turns a line into a `vector` of words, skipping extra spaces | `main.cpp` / `split()` | `std::vector`, `std::istringstream`, `>>` skips spaces, `const &` (no copy) |
| 2026-09-29 | M1.2 | Prints each word as `[word]` | `main.cpp` / `main()` | Range-based `for` loop |
| 2026-09-29 | M1.3 | Empty or all-space lines just re-prompt; `   exit   ` quits | `main.cpp` / `main()` | `empty()`, `continue` vs `break`, check `empty()` before `[0]` (out-of-bounds is undefined behavior) |
| 2026-09-29 | M2.1 | Forks a child for each command; child prints its PID and exits, parent prints the child's PID | `main.cpp` / `main()` | Processes, PIDs, `fork()` returns 0 in child and child PID in parent, system calls |
| 2026-09-29 | M2.2–M2.3 | Builds a `char*` list ending in `nullptr`; child calls `execvp` so `ls`, `echo`, `pwd` really run | `main.cpp` / `main()` | Pointers, `.data()`, C-style strings, PATH search, exec replaces the process |
| 2026-09-29 | M2.4–M2.5 | Parent calls `waitpid` so output comes before the next prompt | `main.cpp` / `main()` | `waitpid`, zombies, child inherits the terminal |
| 2026-09-29 | M3.1–M3.2 | Bad command prints `<cmd>: No such file or directory`; child ends with `_exit(127)` | `main.cpp` / `main()` | exec only returns on failure, `perror`, `_exit` vs `return`, exit code 127 |
| 2026-09-29 | M3.4 | Proved one `exit` quits after a bad command (no leftover shells) | none (test) | `ps` to list processes |
| 2026-09-29 | M3.3 | If `fork` returns -1, print `fork: <reason>` and `continue` to the next prompt | `main.cpp` / `main()` | `fork` returns -1 on failure, `waitpid(-1)` means any child, `continue` vs `_exit` |
| 2026-09-29 | M4.1 | Tested `cd /tmp` then `pwd`: folder didn't change | none (test) | Each process has its own current folder |
| 2026-09-29 | M4.2–M4.3, M4.5 | `cd <dir>` calls `chdir` in the shell itself and skips the fork; bad folder prints `cd: No such file or directory` | `main.cpp` / `main()` | Built-ins, `chdir`, `.c_str()` |
| 2026-09-29 | M4.4 | Plain `cd` uses `getenv("HOME")`; checks for `nullptr` before `chdir` | `main.cpp` / `main()` | Environment variables, `getenv`, `\|\|` short-circuit, reading an ASan stack trace |
| 2026-10-03 | M5.0 | `split()` lives in `parser.cpp`, declared in `parser.h`; build lists both `.cpp` files | `parser.h`, `parser.cpp`, `main.cpp` | Headers, declaration vs definition, `#pragma once`, `""` vs `<>` includes, refactoring |
| 2026-10-03 | M5.1 | `parsePipeline()` returns a list of commands; `\| ls`, `ls \|`, `ls \| \| wc` print a syntax error; 2 commands print `pipe: a \| b` for now | `parser.cpp` / `parsePipeline()`, `main.cpp` / `main()` | `struct`, vector of vectors, returning two things in one struct, `std::cerr`, C++ is case-sensitive |
| 2026-10-04 | Refactor | Argv building + `execvp` + `_exit(127)` moved into `runCommand()` so the pipe code can reuse it | `main.cpp` / `runCommand()` | Functions to avoid duplicate code; file descriptors, `pipe()`, `dup2()` (concepts introduced) |

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

**3. Missing include + missing brace (2026-09-29)**
- **What broke:** `error: implicit instantiation of undefined template 'std::basic_istringstream<char>'` on line 7, and `error: expected '}'` at the end of the file.
- **Why:** `<sstream>` wasn't included, so the compiler knew the name `istringstream` but not its full code. Replacing a line also deleted the `}` that closed the `while` loop.
- **Fix:** Added `#include <sstream>` and `#include <vector>`. Put the `}` back.
- **Learned:** Include a header for everything you use. `expected '}'` at the end of the file means a `{` is unmatched, and the note points to the open one.

**4. Shell quit after one command (2026-09-29)**
- **What broke:** The shell printed `[ls][-l]` once and then exited.
- **Why:** The missing `}` was added at the end of the file, so `return 0;` stayed inside the `while` loop. `return` ends the whole function, and in `main` that ends the program.
- **Fix:** Moved `return 0;` below the loop's closing `}`.
- **Learned:** Compiling isn't the same as working. Use indentation (Format Document) to see which block each line is in.

**5. VS Code red underline on `w.data()` (2026-09-29)**
- **What broke:** VS Code marked line 50 (`args.push_back(w.data());`) as an error, but `clang++` compiled it fine.
- **Why:** VS Code's IntelliSense checks code separately from the compiler and assumed an older C++ version, where `.data()` returns `const char*`.
- **Fix:** C/C++: Edit Configurations (UI) → compiler `/usr/bin/clang++`, standard `c++20`.
- **Learned:** If the editor and the compiler disagree, trust the compiler.

**⭐ 6. Duplicate shells after a failed command (2026-09-29)**
- **What broke:** With `return 1;` removed from the child, typing `asdf` twice then `ps` showed 3 `mysh` processes, and it took 3 `exit`s to quit.
- **Why:** When `execvp` fails, it returns, and the child is still a full copy of the shell. With nothing to stop it, the child loops back, prints `mysh> `, and reads input as a second shell. The parent sits in `waitpid` underneath.
- **Fix:** End the child right after a failed exec with `_exit(127)`. `_exit` skips cleanup, so the child doesn't flush the parent's copied output buffers (which would print text twice).
- **Learned:** After `fork`, every path in the child must end in exec or exit. Use `ps` to see how many processes are really running.

**7. AddressSanitizer: heap-buffer-overflow on plain `cd` (2026-09-29)**
- **What broke:** Typing `cd` with no folder crashed with `ERROR: AddressSanitizer: heap-buffer-overflow ... READ of size 1`, and the stack trace pointed to `main.cpp:53` (`words[1].c_str()`).
- **Why:** `cd` alone gives `words` only 1 item. `words[1]` reads past the end of the list. C++ doesn't check `[]` bounds.
- **Fix:** Check `words.size() < 2` first and use `getenv("HOME")` in that case. Also check `getenv` didn't return `nullptr`.
- **Learned:** How to read an ASan report: the `ERROR` line names the bug type, and in the stack trace you skip `std::` library frames until the first `main.cpp:<line>`.

---

## Concepts learned
*(Written in my own words.)*

- **How a shell runs a program:** zsh makes a copy of itself (fork), the copy turns into my program (exec), and the original waits for it to finish (wait) before showing the prompt again.
- **`getline` vs `cin >>`:** `getline` reads the whole line including spaces. `cin >>` stops at the first space, so `hello world` would give just `hello`.
- **Ctrl+D / EOF:** Ctrl+D tells the program there is no more input. `getline` then fails, and `cin` stays failed, so you must check the result or the loop spins forever.
- **Shells launch programs:** `ls`, `echo`, `pwd` are separate programs in `/bin`. The shell finds them through PATH and starts them. It doesn't do their work.
- **fork / exec / wait:** the shell clones itself (fork), the clone becomes the command (exec), and the original waits (wait), so the shell survives every command.
- **`continue` vs `_exit`:** `continue` skips the rest of the loop in a process that keeps running. `_exit` ends the process, so nothing after it runs.
- **Why `cd` must be a built-in:** `cd` in a child changes the child's folder, then the child ends and the change is gone. The parent (the shell) never moves. Each process has its own current folder.
- **Headers:** the `.h` file is the menu (what functions exist), the `.cpp` file is the kitchen (the code). Putting only declarations in the header means the code exists once, so files that include it don't create duplicate copies.
- **Pipe parsing:** fill a box with words; a `|` puts the box on the shelf. An empty box at a `|` or at the end (with something already on the shelf) means a command is missing.
- **Compile vs link:** compiling turns `.cpp` into machine code. Linking joins the pieces into one program and connects `main`. `ld:` errors come from the linker.

---

## Git commits
| Milestone | Commit message |
|-----------|----------------|
| M0.1 | `M0.1: print shell prompt` (d5576c6) |
| M0.2 | `M0.2: read a line with getline and echo it` (0d93da8) |
| M0.3–M0.4 | `M0.3-M0.4: loop and quit on exit` (6774194) |
| M0.5 | `M0.5: quit cleanly on Ctrl+D` (7472c6d) |
| M1.1–M1.2 | `M1.1-M1.2: split line into words` (b9d3e08) |
| M1.3 | `M1.3: skip empty lines, exit via first word` (396d073) |
| M2.2–M2.3 | `M2.2-M2.3: run commands with fork and execvp` (f175da7) |
| M2 | `M2: run commands with fork, execvp, waitpid` (12cd90e) |
| M3.1–M3.2 | `M3.1-M3.2: _exit(127) on failed exec, show command name in error` (60873a9) |
| M3.3 | `M3.3: handle fork failure` (661d499), `M3.3: skip to next prompt when fork fails` (f2a8024) |
| M4 | `M4: cd and exit built-ins, cd with no argument goes to HOME` (08c2fed) |
| M5.0 | `M5.0: move split() into parser.h/parser.cpp` (e1f71ad) |
| M5.1 | `M5.1: parse pipelines, reject empty commands around \|` (15f4ef3) |
