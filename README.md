# PZIP
Multithreaded run-length encoding.

## Description
PZIP is based on an operating systems course assignment, where students were asked to create a run-length encoder C program for Linux using multiple threads. This version is similar but written in C++ without using platform-specific libraries.
It features both encoding and decoding for input files. The decoding algorithm only uses a single thread, as its primary purpose is to verify the encoding process.

## Technology
* C++20
* Threads and guard locks

## Build
This project has been transitioned from Visual Studio to Visual Studio Code. It is now built using `g++` and can be compiled using the provided tasks configuration for VS Code.
Future updates will include the addition of CMake support, along with tests and benchmarks.

### Prerequisites:
- `g++` compiler (or any compatible C++20 compiler)
- [VS Code](https://code.visualstudio.com/)
- [MSYS2](https://www.msys2.org/) or other similar environments for Windows users (for `g++` and related tools)

### Build Instructions:
1. Open the project in VS Code.
2. Ensure the `tasks.json` file is properly configured for your environment.
3. Compile the program by running the build task in VS Code.

## Usage
```
PZIP.exe <options> <filename>
  options:
    -e, --encode to encode the specified file.
    -d, --decode to decode the specified file.
```

## Author
Created by Martijn Koning.