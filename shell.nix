{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.gcc
    pkgs.clang
    pkgs.SDL2
    pkgs.makeWrapper
    pkgs.pkg-config
  ];

  shellHook = ''
    echo "Welcome to the CHIP-8 emulator dev shell (legacy shell.nix)!"
  '';
}
