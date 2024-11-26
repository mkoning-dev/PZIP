# PZIP
Multithreaded run-length encoding.

## Description

PZIP is based on an operating systems course assignment, where students were asked to create a run-length encoder in C for Linux using multiple threads. This version is written in C++ and designed to be platform-independent, without relying on platform-specific libraries.

PZIP supports both encoding and decoding for input files. The decoding algorithm uses a single thread, primarily to verify the accuracy of the encoding process.

## Technology

- C++20
- Threads and guard locks

## Build

This project was transitioned from Visual Studio to Visual Studio Code and is now built using g++ and CMake. It can be compiled using the provided tasks configuration for VS Code, and CMake is used to manage the build process. Unit tests have been added using the Google Test framework.

### Prerequisites:

- g++ compiler (or any compatible C++20 compiler)
- VS Code
- MSYS2 or a similar environment for Windows users (for g++ and related tools)

### Build Instructions:

1. Open the project in VS Code.
2. Ensure the `tasks.json` file is properly configured for your environment.
3. To build the project, run the build task in VS Code, or use CMake and mingw32-make as needed:
    - Run `cmake ..` in the build directory.
    - Run `mingw32-make` to compile the project.

## Usage
```
pzip.exe <options> <filename>
  options:
    -e, --encode    to encode the specified file.
    -d, --decode    to decode the specified file.
```

## Author

Created by Martijn Koning.