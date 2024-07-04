# CSerialPort for Python

```
swig: 4.1.1 (2022-11-30)
cmake: 3.26.4
python: 3.10.11
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
cd bindings/python
mkdir bin
cd bin
cmake .. -DSWIG_EXECUTABLE=D:/swigwin-4.1.1/swig.exe
cmake --build . --config Release
```

**Notice:** 

```
D:/swigwin-4.1.1/swig.exe should replace with your path to swig.exe
```

or (config swig to path)

```
set path=D:/swigwin-4.1.1;%path%
cmake ..
```

## Run

```
cd bindings/python
cmake -E copy generate/cserialport.py example
cmake -E copy bin/bin/Release/_cserialport.pyd example
python example/main.py
```

### Tree

```
bindings/python $tree
.
+--- bin
|   +--- bin
|   |   +--- Release
|   |   |   +--- _cserialport.pyd
+--- CMakeLists.txt
+--- cserialport.i
+--- example
|   +--- cserialport.py
|   +--- main.py
|   +--- _cserialport.pyd
+--- generate
|   +--- cserialport.py
|   +--- cserialportPYTHON_wrap.cxx
|   +--- cserialportPYTHON_wrap.h
+--- README.md
```