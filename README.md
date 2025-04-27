# Mini Binary File Inspector and Editor (Hex Editor) in C

## Overview
This project implements a minimal interactive hex editor written in C.  
It allows users to load binary files into memory, inspect and modify them at the byte level, and save changes back to disk.

Developed as part of the Extended System Programming Lab at BGU.

## Features
- Load sections of a binary file into memory.
- Display content in hexadecimal or decimal format.
- Modify memory manually (overwrite bytes).
- Save modified memory back to a file at a specified location.
- Interactive command-line menu interface.
- Toggle Debug Mode to see internal state during runtime.
- Safe buffer management and dynamic memory allocation.

## Technologies
- Language: C
- File I/O: `fopen`, `fseek`, `fread`, `fwrite`
- Memory: Manual management via `malloc`, `free`
- CLI: `fgets`, `scanf`, parsing user input

## Build Instructions
```bash
make
```

## Run Instructions
```bash
./hex_editor
```
Follow the menu instructions to load files, inspect, and modify memory.

## Notes
- Supports unit sizes of 1, 2, or 4 bytes.
- Input file must be accessible and readable.
- This tool is educational and intended for small binary file experimentation.
