# CSerialPort for Node.js

```
nodejs: 20.15.0 (2024-06-20)
cmake: 3.8.2
cmake-js: 7.3.0 (2024-01-16)
node-addon-api: 8.0.0 (2024-05-05)
```

## Install swig && cmake

### windows

- swig

```
$ wget https://sourceforge.net/projects/swig/files/swigwin/swigwin-4.1.1/swigwin-4.1.1.zip
```

- cmake

```
$ wget https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-windows-x86_64.msi
```

### linux

- swig

```
$ wget https://sourceforge.net/projects/swig/files/swig/swig-4.1.1/swig-4.1.1.tar.gz
$ tar zxvf swig-4.1.1.tar.gz
$ cd swig-4.1.1
$ ./configure --without-pcre
$ make
$ sudo make install
$ swig -version
SWIG Version 4.1.1
```

- cmake

```
$ wget https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-linux-x86_64.sh
$ sudo ./cmake-3.26.4-linux-x86_64.sh --prefix=/usr/local --skip-license
$ cmake --version
cmake version 3.26.4
```

### macos

- swig

```
$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
$ brew install swig
```

- cmake

```
$ brew install cmake
```

## Build

```
cd bindings/nodejs
npm i -g cmake-js
npm run build
```

## Run

```
npm run start
```
