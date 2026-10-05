# mysh: a mini Unix shell in C++

A small Unix shell written from scratch in C++20 on macOS. I built it to learn how a shell actually runs programs: processes, file descriptors, pipes, and signals, all through raw POSIX system calls.

## Features

- Runs any program on your `PATH` (`ls -l`, `grep`, `wc`, ...) with `fork`, `execvp`, and `waitpid`
- Built-in commands: `cd` (plain `cd` goes to `$HOME`) and `exit`
- One pipe: `ls -l | grep cpp`
- Output redirection: `ls > out.txt`, `ls | wc -l > count.txt`
- Ctrl+C stops the running command, not the shell
- Bad commands, bad folders, and bad syntax print an error, and the shell keeps running
- Quits on `exit` or Ctrl+D
- Runs clean under AddressSanitizer
- Parser fuzz-tested with libFuzzer (about 5.5 million inputs, 0 crashes)

## Build and run

Requires `clang++` with C++20 and `make`.

```bash
make            # optimized build -> ./mysh
./mysh

make debug      # AddressSanitizer build -> ./mysh-debug
make clean      # remove build output
```

## Example session

```
mysh> echo hello world
hello world
mysh> ls | wc -l
       5
mysh> echo saved > out.txt
mysh> cat out.txt
saved
mysh> asdf
asdf: No such file or directory
mysh> ls |
mysh: Syntax error: empty command after pipe
mysh> cd /tmp
mysh> pwd
/private/tmp
mysh> exit
```

## How it works

| Feature | How |
|---|---|
| Running a command | The shell `fork`s a copy of itself. The child calls `execvp`, which replaces it with the program. The parent `waitpid`s until it finishes, then prints the next prompt. |
| Failed commands | If `execvp` fails, the child prints the error and calls `_exit(127)` right away, so it can't keep running as a second shell. |
| `cd` | Runs inside the shell itself with `chdir`, not in a child. A child's folder change would disappear when the child exits. |
| Pipes (`a \| b`) | `pipe()` creates a channel with a read end and a write end. Child 1 uses `dup2` to point its stdout at the write end, child 2 points its stdin at the read end, and every process closes the ends it doesn't use so the reader sees end-of-file. |
| Redirection (`> file`) | The child `open`s the file (`O_WRONLY \| O_CREAT \| O_TRUNC`) and uses `dup2` to point stdout at it before calling `execvp`. |
| Ctrl+C | The shell ignores `SIGINT`. Each child restores the default handler before `exec`, because an ignored signal stays ignored across `exec`. |
| Parsing | `parser.cpp` turns a line into commands, a pipe split, and an output file, and rejects input like `\| ls`, `ls \|`, and `ls >`. It never forks or execs, so it can be tested on its own. |

### Files

| File | Purpose |
|---|---|
| `main.cpp` | Shell loop, built-ins, `runCommand()` (exec + redirect), `runPipeline()` (pipe + two children) |
| `parser.h`, `parser.cpp` | `split()` and `parsePipeline()`, which return a `Pipeline` struct (commands, output file, error) |
| `fuzz_parser.cpp`, `fuzz/seeds/` | libFuzzer target and starting inputs |
| `Makefile` | `make`, `make debug`, `make clean`, `make fuzz` |

## The 3 hardest bugs I hit

### 1. One typo, two shells

**What happened:** To understand error handling, I removed the line that ends the child after a failed `execvp`. I then typed a bad command twice and ran `ps`. It showed **three** copies of my shell, and it took three `exit`s to quit.

**Why:** When `execvp` fails, it returns, and the child is still a full copy of the shell. With nothing stopping it, the child went back to the top of the loop and started reading commands as a second shell, while the real shell waited underneath it.

**Fix:** The child calls `_exit(127)` right after a failed `execvp`. I used `_exit` instead of `return` because `return` runs cleanup that can flush the parent's copied output buffers and print text twice. 127 is the standard "command not found" exit code.

**Lesson:** After `fork`, every path in the child has to end in `exec` or exit.

### 2. AddressSanitizer: heap-buffer-overflow on plain `cd`

**What happened:** Typing `cd` with no folder crashed the AddressSanitizer build with `heap-buffer-overflow ... READ of size 1`. The stack trace pointed to the line that read `words[1]`.

**Why:** Plain `cd` gives a word list with only one item, so `words[1]` read past the end of the list. C++ doesn't bounds-check `[]`, so without AddressSanitizer this could have crashed, printed garbage, or seemed to work.

**Fix:** Check `words.size() < 2` first and use `getenv("HOME")` in that case, then check that `getenv` didn't return `nullptr` before calling `chdir`.

**Lesson:** Always check the size before indexing. To read an AddressSanitizer report, skip the standard-library frames and find the first line in your own file.

### 3. A program that hung before `main()` ran

**What happened:** A 7-line program that only printed a prompt froze at 100% CPU and printed nothing, even though the code was correct.

**Why:** I compiled the same file with and without `-fsanitize=address`. Only the AddressSanitizer build hung. Its runtime starts before `main()`, and my Xcode toolchain was older than my macOS version.

**Fix:** I switched to the updated Command Line Tools compiler with `xcode-select`, and AddressSanitizer worked again.

**Lesson:** When correct-looking code fails, change one thing at a time to isolate the cause. Code can run before `main()`.

## Fuzzing

The parser is fuzz-tested with [libFuzzer](https://llvm.org/docs/LibFuzzer.html) and AddressSanitizer.

- `fuzz_parser.cpp` feeds generated input to `parsePipeline()` and checks four rules whenever parsing succeeds: no empty commands, no `|` or `>` left inside a command, no output file without a command, and no `|` or `>` as the file name. A broken rule calls `abort()`, so libFuzzer treats logic bugs like crashes and saves the input.
- The fuzzer only links `parser.cpp`, never `main.cpp`, so generated input can't reach `execvp` and run real commands.
- `fuzz/seeds/` holds starting inputs, including the bad inputs the parser must reject (`| ls`, `ls |`, `ls | | wc`, `ls > a b`). They run first on every fuzz run, so they act as regression tests.

**Results:** about 5.5 million inputs across a 60-second run and a 10-minute run (4,851,327 inputs), with **0 crashes and 0 rule violations**.

**Checking the fuzzer itself:** I removed the parser's empty-command check on purpose and reran it. libFuzzer caught the bug within seconds, building the input `ls -ll --?l\n|  |  w` from the `ls -l` seed. That confirmed a clean result means something.

```bash
brew install llvm   # Apple clang does not ship libFuzzer
make fuzz           # build ./fuzz_parser and fuzz for 60 seconds
```

## Known limitations

- No quote handling: `echo "a b"` passes the quotes through
- `|` and `>` need spaces around them (`ls|wc` is not split)
- Only one pipe per line (`a | b | c` prints an error)
- No input redirection (`<`), append (`>>`), or background jobs (`&`)
- Built-ins don't support pipes or redirects (`cd /tmp > f` changes folder but doesn't create `f`)

## Next

- Automated tests and GitHub Actions CI on every push
