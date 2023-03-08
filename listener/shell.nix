# shell.nix
{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
    packages = [
        pkgs.python311
        pkgs.poetry
    ];
    shellHook = ''
      poetry config virtualenvs.create true --local &&
      poetry env use $(which python) &&
      poetry install &&
      poetry update &&
      poetry shell
    '';
}
