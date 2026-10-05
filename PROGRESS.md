# Mini Shell (C++): Progress

## Current status
- **Milestone:** All milestones M0–M9 done ✅
- **Last completed step:** Final code review; README updated with known limitations and possible improvements
- **Next step:** Submit NVIDIA Ignite application (Oct 5–18). Optional: fix piped-stdin buffering, harden CI permissions
- **Repo:** https://github.com/Bebop1023/mini-shell
- **Hours:** Session 1 (2026-09-29): 1.5 h
- **Deadline:** Apply between Oct 5 and Oct 18, 2026 (NVIDIA Ignite). No benefit to applying early, so aim for a polished project. Target: M9 done before applying.

### Files in the project
| File | What it does |
|------|--------------|
| `PROGRESS.md` | This file. Tracks progress, walls, and concepts (Claude maintains it) |
| `main.cpp` | `runCommand()` restores default SIGINT, builds argv and execs (child only, never returns). `runCommand()` also redirects stdout to `outfile` with `open` + `dup2`. `runPipeline()` runs `a | b` with `pipe` + 2 forks + `dup2`, handles fork failure. `main()`: prompt, read, `parsePipeline`, built-ins (`exit`, `cd`), then `fork` + `runCommand` + `waitpid` |
| `parser.h` | Declarations of the parsing functions (the "menu") and the `Pipeline` struct (commands + error) |
| `parser.cpp` | Parsing code: `split()` breaks a line into words; `parsePipeline()` splits words at `|` into commands and reports syntax errors. No fork/exec, so the fuzzer can test it safely |
| `Makefile` | Build rules: `make`, `make debug`, `make clean` |
| `fuzz_parser.cpp` | libFuzzer target: feeds random input to `parsePipeline()` and `abort()`s if a parse rule is broken. Never touches `main.cpp` |
| `fuzz/seeds/` | Starting inputs for the fuzzer (committed) |
| `fuzz/corpus/` | Inputs the fuzzer discovered (git-ignored) |
| `tests/run_tests.sh` | 15 bash tests: pipes input into the shell, checks output with `check` / `check_absent`, fails on any ASan error |
| `.github/workflows/ci.yml` | GitHub Actions: on every push, build, test, ASan test (with Linux leak check), 60 s fuzz |
| `README.md` | Public project page: features, build, example, how it works, 3 hardest bugs, limitations |
| `mysh`, `mysh-debug`, `*.dSYM/` | Compiled programs + debug info (build output, git-ignored) |
| `.gitignore` | Keeps `mysh`, `mysh-debug`, `*.dSYM/`, `.vscode/` out of git |

### Build command
```
make          # builds ./mysh (optimized, -O2)
make debug    # builds ./mysh-debug (AddressSanitizer + -g)
make clean    # deletes build output
make test     # runs tests/run_tests.sh against ./mysh
make fuzz     # builds ./fuzz_parser (Homebrew clang) and fuzzes the parser for 60 s
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
- [x] M5.2 Create a pipe with `pipe()`
- [x] M5.3 Fork two children and wire them up with `dup2()`
- [x] M5.4 Close every unused pipe end (so the reader sees end-of-file)
- [x] M5.5 Wait for both children
- [x] M5.6 Test: `ls | wc -l`, `echo hello | tr a-z A-Z`

### M6. Stretch: output redirection
- [x] M6.1 Detect `> filename` in the words
- [x] M6.2 `open()` the file with the right flags and permissions
- [x] M6.3 `dup2()` the file onto stdout in the child
- [x] M6.4 Handle errors (missing filename, can't open the file)

### M7. Ship
- [x] M7.1 Makefile (`make`, `make debug`, `make clean`)
- [x] M7.2 Clean AddressSanitizer run through every feature
- [x] M7.3 README: what it does, how to build, the 3 hardest walls
- [x] M7.4 Final commit and push to GitHub
- [x] M7.5 (optional) Shell ignores Ctrl+C so only the running command dies (`signal(SIGINT, ...)`)

### M8. Fuzzing (added 2026-10-03)
- [x] M8.1 Install full LLVM (`brew install llvm`). Apple clang has no libFuzzer.
- [x] M8.2 Write `fuzz_parser.cpp` with `LLVMFuzzerTestOneInput` that calls only the parser (never fork/exec)
- [x] M8.3 Build with `-fsanitize=fuzzer,address` and run it
- [x] M8.4 Fix every crash it finds; log each one in Walls
- [x] M8.5 Save crash inputs as regression tests and keep a seed corpus

### M9. Tests + CI (added 2026-10-03)
- [x] M9.1 Test script: feed commands to `./mysh` and check the output
- [x] M9.2 GitHub Actions workflow: build with ASan and run tests on every push
- [x] M9.3 Run the fuzzer for 60 seconds in CI
- [x] M9.4 CI badge and fuzzing results in the README

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
| 2026-10-04 | M5.2–M5.6 | `runPipeline()` makes a pipe, forks two children, `dup2`s child 1's stdout to the write end and child 2's stdin to the read end, closes all extra ends, waits for both. 3+ commands print `only one pipe is supported` | `main.cpp` / `runPipeline()`, `main()` | File descriptors 0/1/2, `pipe()`, `dup2()`, `close()`, EOF only after every write end is closed |
| 2026-10-04 | M6.1 | `parsePipeline()` finds `>`; it must be followed by exactly one filename at the end. Sets `outfile` and cuts `> file` off the words. `ls >`, `ls > a b`, `> out`, `ls > f \| wc` are errors | `parser.h` / `Pipeline::outfile`, `parser.cpp` / `parsePipeline()` | Index loops, `size_t` can't go negative (use `i + 2 != size()` not `i != size() - 2`), `resize`, `break` |
| 2026-10-04 | M6.2–M6.4 | Child opens `outfile` with `O_WRONLY \| O_CREAT \| O_TRUNC, 0644`, `dup2`s it onto slot 1, closes the extra fd, then execs. In a pipeline only the right command gets the file. Bad path prints `<file>: No such file or directory` and `_exit(1)` | `main.cpp` / `runCommand()`, `runPipeline()` | `open()` flags, permissions `0644`, redirect = same chute trick as pipes |
| 2026-10-04 | Review fix | `runPipeline` checks both forks for -1: closes the pipe, waits for child 1 if it exists, returns | `main.cpp` / `runPipeline()` | Every `fork` needs a failure check; `waitpid(-1)` waits for any child |
| 2026-10-04 | M7.1 | `Makefile` with variables, `mysh` (-O2), `debug` → `mysh-debug` (ASan), `clean`, `.PHONY`; `.gitignore` uses `*.dSYM/` | `Makefile`, `.gitignore` | Make rules (target: deps, TAB + command), only rebuilds what changed, `-O2`, `.PHONY`, glob patterns in `.gitignore` |
| 2026-10-04 | M7.2 | `make debug` build run through every feature (commands, errors, cd, pipes, redirects, syntax errors, Ctrl+D): 0 AddressSanitizer errors | none (test) | ASan only checks our code (children exec uninstrumented programs); no leak checking on Apple Silicon |
| 2026-10-04 | M7.5 | Shell calls `signal(SIGINT, SIG_IGN)` at startup; each child calls `signal(SIGINT, SIG_DFL)` before exec, so Ctrl+C kills `sleep` but not mysh | `main.cpp` / `main()`, `runCommand()` | Signals, SIGINT, `SIG_IGN` / `SIG_DFL`, ignored signals are inherited and survive `exec` |
| 2026-10-04 | M7.3 | README written (by Claude, at Miles's request) from Walls + Concepts: features, build, real example session, how it works, 3 hardest bugs (duplicate shells, `cd` heap overflow, ASan hang), limitations | `fuzz_parser.cpp` | libFuzzer target: feeds random input to `parsePipeline()` and `abort()`s if a parse rule is broken. Never touches `main.cpp` |
| `fuzz/seeds/` | Starting inputs for the fuzzer (committed) |
| `fuzz/corpus/` | Inputs the fuzzer discovered (git-ignored) |
| `README.md` | Writing for recruiters: short, scannable, honest limitations |
| 2026-10-05 | M8.1 | Installed Homebrew LLVM (`/opt/homebrew/opt/llvm/bin/clang++`, clang 23); test fuzzer ran 9M inputs in 4 s | none (tooling) | Fuzzing, coverage-guided mutation, why only the parser is fuzzed |
| 2026-10-05 | M8.2–M8.3 | `fuzz_parser.cpp` with `LLVMFuzzerTestOneInput` + 4 rules; 5 seeds; Makefile `fuzz_parser` / `fuzz` targets (`FUZZCXX ?=`) | `fuzz_parser.cpp`, `Makefile`, `fuzz/seeds/` | `extern "C"`, raw bytes → `std::string`, invariants + `abort()`, seeds vs corpus |
| 2026-10-05 | M8.4 | 60 s run: 673,814 inputs, 0 crashes, 211 corpus inputs. Planted bug (removed empty-command check) was caught in seconds with input `ls -ll --?l\n\|  \|  w` | none (test) | Validating a test by planting a bug (mutation testing) |
| 2026-10-05 | M8.5 | Added 4 bad-input seeds (`ls \| \| wc`, `\| ls`, `ls \|`, `ls > a b`) as regression tests; 10-minute run: 4,851,327 inputs, 0 crashes, corpus 641. README got a Fuzzing section | `fuzz/seeds/`, `README.md` | Seeds as regression tests |
| 2026-10-05 | M9.1 | `tests/run_tests.sh` (15 tests) + `make test`; 15/15 pass on `mysh` and `mysh-debug`; confirmed a fake shell fails 13/15 | `tests/run_tests.sh`, `Makefile` | Bash functions, `$1`, `$( )`, `2>&1`, `[[ == *text* ]]`, `$'...\n'`, exit code as pass/fail, `chmod +x`, avoid platform-specific output (wc spacing) |
| 2026-10-05 | M9.2–M9.3 | `.github/workflows/ci.yml` on ubuntu-24.04: checkout, install clang, `make`, `make test`, ASan tests, `make fuzz FUZZCXX=clang++`. First run (16a54f3) passed every step | `.github/workflows/ci.yml` | CI, YAML (spaces only), jobs/steps, exit codes decide green/red, `?=` override from the command line, Linux ASan includes leak checking |
| 2026-10-05 | M9.4 | README: CI badge, Testing and CI section, `make test`, files table | `README.md` | |
| 2026-10-05 | Final review | No warnings with `-Wpedantic -Wshadow -Wconversion`; ASan + UBSan clean on all 15 tests; found piped-stdin buffering limitation; README got limitations + possible improvements | `README.md` | UBSan, stdio buffering vs child processes, CI least privilege, action pinning, `mktemp` |

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

**8. Code review: `runPipeline` ignored `fork` failure (2026-10-04)**
- **What broke:** Found in review, not at runtime. If the first `fork` in `runPipeline` returned -1, the second command would still run, reading a pipe nobody writes to, and `waitpid(-1)` would wait for any child.
- **Why:** The M3 fork check was only added to the single-command path. The new pipeline code had two more `fork` calls without checks.
- **Fix:** After each fork, `if (pid < 0)`: print the error, close both pipe ends, `waitpid` child 1 if it already exists, return.
- **Learned:** Every `fork` call needs its own failure check. New code paths don't inherit old fixes.

**Known limitations (for README):** no quote handling (`echo "a b"` keeps the quotes); `|` and `>` need spaces around them; only one pipe; built-ins ignore pipes and redirects; Ctrl+C kills the shell too.

**9. Final review: piped input swallowed by `std::cin` (2026-10-05)**
- **What broke:** `printf 'cat\nhello from stdin\n' | ./mysh` ran `hello` as a command instead of feeding it to `cat`.
- **Why:** with a pipe (not a terminal) as input, `std::cin` reads a large chunk into its own buffer, taking lines meant for child commands. A terminal sends one line at a time, so interactive use was fine.
- **Status:** documented in README Known limitations. Fix: read stdin one byte at a time with `read(0, &c, 1)` when it isn't a terminal (what bash does).
- **Learned:** buffering in the parent can steal input from children; test with piped input, not just typed input.

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
- **File descriptors:** numbered slots. 0 = input (keyboard), 1 = output (screen), 2 = errors. Programs just use the slot number and don't know where it leads.
- **Pipes:** `pipe()` makes a tube (`fds[0]` exit, `fds[1]` entrance). `dup2` points kid 1's output into the tube and kid 2's input out of it. Everyone must let go of tube ends they don't use, or the reader waits forever.
- **Redirection:** before the command starts, the shell points its output (slot 1) at the file instead of the screen. Same trick as pipes, with a file instead of a tube.
- **Makefile:** a file of build recipes. `target: what it needs`, then a TAB-indented command. `make` only rebuilds when a needed file is newer than the target.
- **Signals:** Ctrl+C sends SIGINT to every process in the terminal. The shell ignores it; children switch back to the default (die) before exec, because "ignore" is inherited and survives exec.
- **Fuzzing:** a fuzzer throws huge numbers of mutated inputs at code and keeps the ones that reach new lines. Rules + `abort()` turn logic mistakes into crashes it can detect. Plant a bug on purpose to prove the fuzzer can catch one.
- **CI:** on every push, GitHub starts a fresh Linux machine and runs my build and tests. Any command with a non-zero exit code turns the run red.
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
| M5.2 | `M5.2: move exec code into runCommand()` (4f723b5) |
| M5 | `M5: run two-command pipelines with pipe and dup2` (d9820ba) |
| M6 | `M6: output redirection with open and dup2; handle fork failure in pipelines` (0768959) |
| M7.1 | `M7.1: Makefile with release, debug (ASan), and clean targets` (3e8e2dc) |
| M7.5 | `M7.5: shell ignores Ctrl+C; children restore default SIGINT` (c898058) |
| M7 | `M7: README with build steps, design, and hardest bugs` (05009f0) |
| M8 | `M8: libFuzzer harness for the parser with seeds and invariants` (0bed9c0) |
| M9.1 | `M9.1: test script with 15 tests and make test target` (4b7ce41) |
| M9.2–M9.3 | `M9.2: GitHub Actions CI: build, tests, ASan tests, 60s fuzz` (16a54f3) |
| M9.4 | `M9: CI badge and testing section in README` (1447093) |
