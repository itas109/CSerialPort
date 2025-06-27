{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };
  outputs = { nixpkgs, ... }: let
    pkgs = import nixpkgs {
      system = "x86_64-linux";
    };
  in {
    devShells."x86_64-linux".default = pkgs.mkShell {
      nativeBuildInputs = with pkgs; [
        cmake
      ];
      hardeningDisable = [ "all" ];
    };
    packages."x86_64-linux".default = pkgs.stdenv.mkDerivation {
      pname = "CSerialPort";
      version = "9999";

      src = ./.;
      strictDeps = true;

      nativeBuildInputs = with pkgs; [
        cmake
      ];

      meta = {
        homepage = "https://blog.csdn.net/itas109/article/details/84282860";
        description = "CSerialPort - lightweight cross-platform serial port library for C++/C/C#/Java/Python/Node.js/Electron/Rust ";
        license = pkgs.lib.licenses.lgpl3;
        platforms = pkgs.lib.platforms.unix;
      };

    };
  };
}
