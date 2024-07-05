# CSerialPort for CSharp

```
swig: 4.1.1 (2022-11-30)
cmake: 3.26.4
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
$ wget https://sourceforge.net/projects/swig/files/swig/swig-4.1.1/swig-4.1.1.tar.gz
$ tar zxvf swig-4.1.1.tar.gz
$ cd swig-4.1.1
$ ./configure --without-pcre
$ make
$ sudo make install
$ swig -version
SWIG Version 4.1.1
```
or
```
$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
$ brew install swig
```

- cmake

```
$ brew install cmake
```

## Build

- windows

```
cd bindings/csharp
mkdir bin
cd bin
cmake .. -DSWIG_EXECUTABLE=D:/swigwin-4.1.1/swig.exe
cmake --build .
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

- linux (ubuntu  22.04)

```
sudo apt update
sudo apt install mono-complete

cd bindings/csharp
mkdir bin
cd bin
cmake .. -DSWIG_EXECUTABLE=/usr/local/bin/swig
cmake --build .
```

comple example with mono

```
cd example
cmake -E copy ../bin/bin/libcserialport.so .
cmake -E copy ../generate/*.cs .

mcs Program.cs cserialportCSharp.cs cserialportCSharpPINVOKE.cs CSerialPort.cs CSerialPortInfo.cs CSerialPortListener.cs BaudRate.cs DataBits.cs FlowControl.cs OperateMode.cs Parity.cs StopBits.cs SerialPortError.cs SerialPortInfo.cs SerialPortInfoVector.cs
```

- macos (10.15)

```
wget https://download.mono-project.com/archive/6.12.0/macos-10-universal/MonoFramework-MDK-6.12.0.206.macos10.xamarin.universal.pkg

cd bindings/csharp
mkdir bin
cd bin
cmake .. -DSWIG_EXECUTABLE=/usr/local/bin/swig
cmake --build .
```

comple example with mono

```
cd example
cmake -E copy ../bin/bin/libcserialport.dylib .
cmake -E copy ../generate/*.cs .

mcs Program.cs cserialportCSharp.cs cserialportCSharpPINVOKE.cs CSerialPort.cs CSerialPortInfo.cs CSerialPortListener.cs BaudRate.cs DataBits.cs FlowControl.cs OperateMode.cs Parity.cs StopBits.cs SerialPortError.cs SerialPortInfo.cs SerialPortInfoVector.cs
```

## Run

- windows

```
cd bin
CommCSharp
```

- linux

```
cd example
LD_LIBRARY_PATH=. mono Program.exe
```

- macos

```
cd example
LD_LIBRARY_PATH=. mono Program.exe
```

### Tree

```
bindings/csharp $tree
.
+--- bin
|   +--- bin
|   |   +--- Debug
|   |   |   +--- CommCSharp.exe
|   |   |   +--- libcserialport.dll
+--- CMakeLists.txt
+--- cserialport.i
+--- example
|   +--- CMakeLists.txt
|   +--- Program.cs
+--- generate
|   +--- BaudRate.cs
|   +--- CSerialPort.cs
|   +--- cserialportCSharp.cs
|   +--- cserialportCSharpPINVOKE.cs
|   +--- cserialportCSHARP_wrap.cxx
|   +--- cserialportCSHARP_wrap.h
|   +--- CSerialPortInfo.cs
|   +--- CSerialPortListener.cs
|   +--- DataBits.cs
|   +--- FlowControl.cs
|   +--- OperateMode.cs
|   +--- Parity.cs
|   +--- SerialPortError.cs
|   +--- SerialPortInfo.cs
|   +--- SerialPortInfoVector.cs
|   +--- StopBits.cs
+--- README.md
```

