# Chip-8 Emulator

A simple Chip-8 emulator written in C.

## Features

- Interprets and runs Chip-8 ROMs (in-progress)
- Keyboard input mapping (in-progress)
- Graphics rendering (in-progress)
- Sound support (in-progress)
- Accurate timing and instruction emulation (in-progress)

### Using Nix

The recommended way to manage dependencies is with [Nix](https://nixos.org/) and [direnv](https://direnv.net/), which together provide a reproducible development environment.

1. **Install Nix:**  
   Follow the instructions at the [Determinate Nix Docs](https://docs.determinate.systems/determinate-nix/) or the [nix installer repository](https://github.com/DeterminateSystems/nix-installer).

2. **Enable the development environment:**
   - If you use direnv, run:

     ```sh
     direnv allow
     ```

     This will automatically load the development environment whenever you enter the project directory.

   - Without direnv, you can enter the environment manually:
     - For Nix Flake users:

       ```sh
       nix develop
       ```

     - For classic Nix shell users:

       ```sh
       nix shell
       ```

### Without Nix

Ensure you have a C compiler (e.g., `gcc`) installed.

To build the emulator, run:

```sh
make
```

Or manually compile:

```sh
gcc -o chip8-emulator src/*.c -Iinclude
```

## Usage

```sh
./chip8-emulator [--rom filename | --r filename | filename ]
```

You can specify the ROM file as a positional argument or with `--rom`/`--r`:

Examples:

```sh
./chip8-emulator PONG.ch8
./chip8-emulator --file PONG.ch8
./chip8-emulator --filename PONG.ch8
```

## Controls (in-progress)

- Map your keyboard keys to Chip-8 hex keypad as described in your implementation.

## License

MIT License. See [LICENSE](LICENSE) for details.

## References

- [Cowgod’s Chip-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [Wikipedia: Chip-8](https://en.wikipedia.org/wiki/CHIP-8)

```

```
