# CSerialPort for Rust

```
rust: 1.83.0 (90b35a623 2024-11-26)
bindgen: 0.71.1
cmake: 3.26.4
llvm(libclang): 18.1.8
```

## Install cmake

### windows

- cmake

```
$ wget https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-windows-x86_64.msi
```

### linux

- cmake

```
$ wget https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-linux-x86_64.sh
$ sudo ./cmake-3.26.4-linux-x86_64.sh --prefix=/usr/local --skip-license
$ cmake --version
cmake version 3.26.4
```

### macos

- cmake

```
$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
$ brew install cmake
```

## Rust bindgen

```
cargo install bindgen-cli
```

## Rust generate bindings.rs

```
cd bindings/rust
set LIBCLANG_PATH=D:\\Program Files\\LLVM\\bin
bindgen ../c/cserialport.h -o bindings.rs
```

## C Build cserialport library

```
cd bindings/rust
mkdir bin
cd bin
cmake ..
cmake --build .
```

## Rust Build

```
cd bindings/rust
cargo build
```

## Rust Run

```
cd bindings/rust
cargo run
```

### Tree

```
bindings/rust $tree
.
+--- bin
|   +--- bin
|   |   +--- Debug
|   |   |   +--- cserialport.dll
|   +--- lib
|   |   +--- Debug
|   |   |   +--- cserialport.lib
+--- bindings.rs
+--- Cargo.toml
+--- CMakeLists.txt
+--- cserialport.dll
+--- cserialport.lib
+--- example
|   +--- Cargo.toml
|   +--- src
|   |   +--- main.rs
+--- README.md
+--- target
|   +--- debug
|   |   +--- CommRust.exe
```