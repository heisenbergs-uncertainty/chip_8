{
  description = "CHIP-8 Emulator (C) - Nix Flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        # Common build inputs shared between package and dev shell
        buildDeps = with pkgs; [
          SDL2
          SDL2_image
        ];

        # Development-only dependencies
        devDeps = with pkgs; [
          # Compilers
          gcc
          clang

          # Build tools
          gnumake
          pkg-config
          makeWrapper

          # Debugging & profiling (valgrind excluded - broken on macOS ARM)
          gdb
          lldb

          # Static analysis & formatting
          clang-tools  # clang-format, clang-tidy
          cppcheck

          # Documentation
          doxygen
        ];

      in {
        # Build the CHIP-8 emulator as a package
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "chip8-emulator";
          version = "1.0.0";
          src = ./.;

          nativeBuildInputs = with pkgs; [ pkg-config gnumake clang ];
          buildInputs = buildDeps;

          buildPhase = ''
            make release CC=clang
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp chip8-emulator $out/bin/
          '';

          meta = with pkgs.lib; {
            description = "A CHIP-8 emulator written in C";
            license = licenses.mit;
            platforms = platforms.unix;
            mainProgram = "chip8-emulator";
          };
        };

        # Development shell with all tools
        devShells.default = pkgs.mkShell {
          buildInputs = buildDeps ++ devDeps;

          # Set up environment variables
          shellHook = ''
            export CHIP8_DEV=1
            echo ""
            echo "🎮 CHIP-8 Emulator Development Shell"
            echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
            echo ""
            echo "Build commands:"
            echo "  make          - Build release binary"
            echo "  make debug    - Build with debug symbols"
            echo "  make test     - Run unit tests"
            echo "  make memcheck - Run with AddressSanitizer"
            echo "  make clean    - Clean build artifacts"
            echo "  make help     - Show all targets"
            echo ""
            echo "Available tools: clang, gcc, gdb, lldb, valgrind, clang-format, cppcheck"
            echo ""
          '';

          # Ensure pkg-config finds SDL2
          PKG_CONFIG_PATH = "${pkgs.SDL2.dev}/lib/pkgconfig:${pkgs.SDL2_image}/lib/pkgconfig";
        };

        # Minimal shell for CI builds
        devShells.ci = pkgs.mkShell {
          buildInputs = buildDeps ++ (with pkgs; [ clang gnumake pkg-config ]);
        };
      }
    );
}
