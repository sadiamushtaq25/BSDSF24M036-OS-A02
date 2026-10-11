# BSDSF24M036-OS-A02

## Operating Systems Assignment 02
### Re-engineering the Linux `ls` Utility

This project implements different versions of a simple `ls` utility in C.
Each version adds a feature to the previous implementation.

## Features

- v1.0.0: Basic directory listing
- v1.1.0: Long listing using `-l`
- v1.2.0: Down-then-across column listing
- v1.3.0: Horizontal listing using `-x`
- v1.4.0: Alphabetical sorting
- v1.5.0: Colored file names
- v1.6.0: Recursive listing using `-R`

## Requirements

- Linux
- GCC compiler
- GNU Make

## Build

Compile all versions:

    make

Remove generated binaries:

    make clean

## Run

Run the basic version:

    ./bin/ls

Run a particular version:

    ./bin/ls-v1.1.0
    ./bin/ls-v1.2.0
    ./bin/ls-v1.3.0
    ./bin/ls-v1.4.0
    ./bin/ls-v1.5.0
    ./bin/ls-v1.6.0

Pass a directory as an argument:

    ./bin/ls-v1.6.0 /tmp

Use options supported by the relevant version, for example:

    ./bin/ls-v1.1.0 -l
    ./bin/ls-v1.3.0 -x
    ./bin/ls-v1.6.0 -R

## Project Structure

- `src/`: C source files for each version
- `bin/`: Compiled executable programs
- `obj/`: Object files directory
- `man/`: Manual pages directory
- `Makefile`: Build and clean rules
- `REPORT.md`: Assignment report and answers

## Testing

Compile the project using `make`, then test each version with suitable
directories and options. Refer to `REPORT.md` for implementation details
and answers to the assignment questions.

## Author

BSDSF24M036
