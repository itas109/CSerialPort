# CSerialPort for C

```
cmake: 3.26.4
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

## Build

```
cd bindings/c
mkdir bin
cd bin
cmake ..
cmake --build .
```

## Run

```
cd bin
CommC
```

### Tree

```
bindings/c $tree
.
+--- bin
|   +--- bin
|   |   +--- Debug
|   |   |   +--- CommC.exe
|   |   |   +--- cserialport.dll
+--- CMakeLists.txt
+--- cserialport.cpp
+--- cserialport.h
+--- example
|   +--- CMakeLists.txt
|   +--- main.c
+--- README.md
```

