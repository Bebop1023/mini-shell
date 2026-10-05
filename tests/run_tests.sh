#!/bin/bash
# Runs mysh with test input and checks the output.
# Usage: tests/run_tests.sh ./mysh

SHELL_BIN="${1:-./mysh}"
pass=0
fail=0


check() {
    local name="$1"
    local input="$2"
    local expected="$3"
    local actual
    actual=$(printf '%s\n' "$input" | "$SHELL_BIN" 2>&1)

    if [[ "$actual" == *"ERROR: AddressSanitizer"* ]]; then
        echo "FAIL: $name (AddressSanitizer error)"
        echo "$actual"
        fail=$((fail + 1))
    elif [[ "$actual" == *"$expected"* ]]; then
        echo "PASS: $name"
        pass=$((pass + 1))
    else
        echo "FAIL: $name"
        echo "  expected to see: $expected"
        echo "  got: $actual"
        fail=$((fail + 1))
    fi
}

check_absent() {
    local name="$1"
    local input="$2"
    local unwanted="$3"
    local actual
    actual=$(printf '%s\n' "$input" | "$SHELL_BIN" 2>&1)

    if [[ "$actual" == *"$unwanted"* ]]; then
        echo "FAIL: $name"
        echo "  should not see: $unwanted"
        echo "  got: $actual"
        fail=$((fail + 1))
    else
        echo "PASS: $name"
        pass=$((pass + 1))
    fi
}

check "echo"           "echo hello world"            "hello world"
check "extra spaces"   "   echo    a    b   "        "a b"
check "bad command"    "asdfqwer"                    "asdfqwer: No such file or directory"
check "cd and pwd"     $'cd /\npwd'                  $'/\nmysh>'
check "cd bad folder"  "cd /nope"                    "cd: No such file or directory"
check "pipe"           "echo hello | tr a-z A-Z"     "HELLO"
check "leading pipe"   "| ls"                        "empty command before pipe"
check "trailing pipe"  "ls |"                        "empty command after pipe"
check "double pipe"    "ls | | wc"                   "empty command before pipe"
check "too many pipes" "ls | wc | wc"                "only one pipe is supported"
check "redirect"       $'echo saved > /tmp/mysh_test.txt\ncat /tmp/mysh_test.txt' "saved"
check "pipe redirect"  $'echo hi | tr a-z A-Z > /tmp/mysh_test.txt\ncat /tmp/mysh_test.txt' "HI"
check "bad redirect"   "ls >"                        "Syntax error: near '>'"
check "redirect fail"  "ls > /nope/x.txt"            "/nope/x.txt: No such file or directory"
check_absent "exit stops"  $'exit\necho never'        "never"

rm -f /tmp/mysh_test.txt

echo ""
echo "$pass passed, $fail failed"
[ "$fail" -eq 0 ]


