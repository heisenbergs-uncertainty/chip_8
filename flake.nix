{
  description = "CHIP-8 Emulator (C) - Nix Flake";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = pkgs.mkShell {
          buildInputs = [
            pkgs.gcc
            pkgs.clang
            pkgs.SDL2
            pkgs.makeWrapper
            pkgs.pkg-config
          ];
          shellHook = ''
            echo "Welcome to the CHIP-8 emulator dev shell!"
          '';
        };
      }
    );
}
