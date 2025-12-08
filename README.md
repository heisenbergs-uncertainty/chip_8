# CHIP-8 Emulator

A CHIP-8 emulator written in C.

## Overview

CHIP-8 is a simple interpreted language used on vintage computers and consoles. This emulator allows you to run classic CHIP-8 games and programs.

## Features

- CPU emulation
- Graphics rendering (using SDL2)
- Keyboard input
- ROM loading

## Requirements

- C compiler (GCC or Clang)
- SDL2 library

## Build Instructions

You can run the CHIP-8 emulator using either Nix (with or without flakes) or your system environment.

### Using Nix (Recommended)

#### With Flakes Enabled

1. Install [Nix](https://nixos.org/download.html) and [direnv](https://direnv.net/).
2. In the project directory, run:

   ```
   direnv allow
   ```

   This will automatically set up the development environment.

3. Or, manually enter the shell:

   ```
   nix develop
   ```

#### Without Flakes

1. Install Nix.
2. Enter the shell using:

   ```
   nix-shell
   ```

### macOS/Darwin

1. Install dependencies:

   ```
   brew install sdl2
   ```

2. Build the project:

   ```
   make
   ```

3. Run the emulator:

   ```
   ./chip8-emulator <ROM file>
   ```

### Linux

1. Install dependencies:

   ```
   sudo apt-get install build-essential libsdl2-dev
   ```

2. Build the project:

   ```
   make
   ```

3. Run the emulator:

   ```
   ./chip8-emulator <ROM file>
   ```

## License

MIT

```

```
