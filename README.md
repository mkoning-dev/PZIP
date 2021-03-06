# PZIP
Multithreaded run-length encoding.

## Description
PZIP is based on an operating systems course assignment, which asked students to create a run-length encoder C program for Linux using multiple threads. This version is similar, but written in C++ without using platform specific libraries.
It features both encoding and decoding on input files. The decoding algorithm only uses a single thread, since its purpose is primarily to verify the encoding process.

## Technology
* C++20
* Threads and guard locks

## Build
This project was built with MS Visual Studio. It will be updated with CMake in the future. Tests and benchmarks will be added to the code and documentation as well.

## Usage
```
PZIP.exe <options> <filename>
  options:
    -e, --encode to encode the specified file.
    -d, --decode to decode the specified file.
```

## Author
Created by Martijn Koning.
