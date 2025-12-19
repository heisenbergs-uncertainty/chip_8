# Legacy shell.nix for non-flake users
# Prefer using: nix develop (with flake.nix)
{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    # Compilers
    gcc
    clang

    # Build tools
    gnumake
    pkg-config
    makeWrapper

    # SDL2 dependencies
    SDL2
    SDL2_image

    # Debugging (valgrind excluded - broken on macOS ARM)
    gdb
  ];

  shellHook = ''
    echo ""
    echo "🎮 CHIP-8 Emulator Dev Shell (legacy shell.nix)"
    echo "Tip: Use 'nix develop' with flake.nix for more tools"
    echo ""
  '';

  PKG_CONFIG_PATH = "${pkgs.SDL2.dev}/lib/pkgconfig:${pkgs.SDL2_image}/lib/pkgconfig";
}
