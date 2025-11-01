# nix-shell
# make
# make install
{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    gcc
    gnumake
    # Add other tools: cmake, pkg-config, etc.
  ];
  buildInputs = with pkgs; [
    ncurses  # Provides <ncurses.h> and -lncurses linker flag
  ];
}
