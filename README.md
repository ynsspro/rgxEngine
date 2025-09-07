# RGX Engine - A Simple Custom Regex Engine in C

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)  


## Overview

RGX Engine is a lightweight, custom regular expression (regex) engine implemented in C. It supports a simplified syntax for pattern matching, focusing on basic character classes, quantifiers, and string constraints like "starts with" and "ends with." This engine is not a full POSIX or PCRE regex implementation but provides a custom DSL (Domain-Specific Language) for common matching tasks.

The engine compiles patterns into a linked list of elements and matches them against input strings. It's designed for educational purposes or simple use cases where a full regex library isn't needed.

Key features:
- Custom syntax for character types (e.g., any char, digit, space).
- Quantifiers: exact, greater-than-or-equal, less-than, range, or.
- String constraints: starts/ends with specific substrings.
- Escape sequences for special characters.
- No external dependencies beyond standard C libraries.

**Note:** This is an experimental implementation. It may have bugs or limitations (e.g., no support for sets like `.x{e1,e2}` yet, no backreferences, no capturing groups).

## Supported Syntax

The regex patterns use a custom notation. Here's a breakdown:

### Character Types
- `.c`: Matches any alphabetic character (a-z, A-Z).
- `.n`: Matches any digit (0-9).
- `.s`: Matches a space (excluding tabs).
- `.t`: Matches a tab.
- `'string'`: Matches a literal string (e.g., `'ab'` matches "ab"). Use `\'` to escape quotes inside.
- `\\.`: Escapes special characters like `.` or `'`.

### Quantifiers (applied after a character type)
- `e_(N)`: Exactly N occurrences (e.g., `'.c_(3)'` matches exactly 3 chars like "abc").
- `e>(N)`: N or more occurrences (greedy).
- `e<(N)`: Fewer than N occurrences (up to N-1, greedy).
- `e~(N,M)`: Between N and M occurrences (inclusive).
- `e|(N,M)`: Exactly N or M occurrences.

### Constraints
- `-s{string}`: String must start with "string".
- `-e{string}`: String must end with "string".

Patterns are concatenated without operators (e.g., `'.c_(3)'.n_(2)'` matches 3 chars followed by 2 digits).

Spaces in patterns are ignored (removed during compilation).

### Examples
- `.c_(3)`: Matches exactly 3 alphabetic characters (e.g., "abc" ✔️, "ab" ❌).
- `.n>(2)`: Matches 2 or more digits (e.g., "123" ✔️, "12" ✔️, "1" ❌).
- `'ab'_(2)`: Matches "abab" exactly.
- `.s~(1,3)`: Matches 1 to 3 spaces (e.g., "  " ✔️, "    " ❌).
- `.c|(2,4)`: Matches exactly 2 or 4 alphabetic characters.
- `-s{abc}.c`: Starts with "abc" followed by one char (e.g., "abcd" ✔️).
- `-e{xyz}'`: Ends with "xyz" (e.g., "abcxyz" ✔️).
- `a\\''_(2)`: Matches "a\\'a\\'" (escaped quotes).
- `.c|(1,4)'.'.c>(2)'@gmail.com'`: Simple email-like pattern (e.g., "john.doe@gmail.com" ✔️).

For more examples, see the test cases in `rgx.c`.

## Installation and Build

### Prerequisites
- GCC (or any C compiler supporting C2x standard).
- Make (for building).

### Build Instructions
1. Clone the repository:
   ```
   git clone https://github.com/ynsspro/rgxEngine.git
   cd rgxEngine
   touch bin
   ```
2. Build and run tests:
   ```
   make
   ```
   This compiles `rgxEngine.c` and `rgx.c` into `bin/rgxEngine` and runs the tests.

3. Clean up:
   ```
   make clean
   ```

The build uses flags: `-Wall -Wextra -std=c2x -pedantic`. Uncomment `-Werror` in the Makefile to treat warnings as errors.

## Usage

### As a Library
Include `rgxEngine.h` in your project and link against `rgxEngine.c`.

Main function: `int rgx_match(const char *pattern, const char *str, int *pos);`
- `pattern`: The regex pattern string.
- `str`: The input string to match.
- `pos`: Pointer to an integer tracking the current position in `str` (start from 0).
- Returns: 1 on match, -1 on no match or error.

Example:
```c
#include "rgxEngine.h"
#include <stdio.h>

int main() {
    int pos = 0;
    if (rgx_match(".c_(3)", "abc", &pos) == 1) {
        printf("Match!\n");
    }
    return 0;
}
```

### Running Tests
The `rgx.c` file contains a suite of test cases. Build and run via `make` to see results, including parsed patterns and match outcomes.

Sample output for a test:
```
Test 1: Pattern=".c_(3)", String="abc"
Result: 1 (Expected: 1, Pos: 3)
Compiled Regex Structure:
Element 0: .c _ (3)
```

### Debugging
- Use `show_reg_str(element *node_head)` to print the compiled pattern structure.
- The `test_compiler(const char *pattern)` function (in `rgxEngine.c`) can be used to debug parsing steps.

## Limitations
- No support for alternations (e.g., `a|b`), wildcards beyond basics, or negative sets.
- Greedy matching only; no lazy quantifiers.
- No capturing or backreferences.
- Sets like `.x{e1,e2,e3}` or ranges `.x{start-end}` are mentioned but not implemented.
- Assumes ASCII input; no Unicode support.
- Error handling is basic (returns -1 on failure).
- Performance not optimized for large strings/patterns.

## Contributing
Feel free to fork and submit pull requests! Areas for improvement:
- Implement missing features (e.g., sets).
- Add more quantifiers (e.g., `*`, `+` equivalents).
- Improve error messages.
- Add unit tests.

## Acknowledgments
- Inspired by basic regex concepts, implemented from scratch.
- Thanks to the C standard library for making this possible.

If you find bugs or have suggestions, open an issue on GitHub!
