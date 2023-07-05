# CSerialPort for Java

```
swig: 4.1.1 (2022-11-30)
cmake: 3.8.2
java: 1.8.0
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

## Build

```
cd java
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

## Run

```
cd bin
java -Djava.library.path=. -jar CommJava.jar
```

## Tree

```
bindings/java $tree
.
+--- bin
|   +--- CommJava.jar
|   +--- cserialport.dll
+--- CMakeLists.txt
+--- cserialport.i
+--- example
|   +--- CommJava.java
+--- generate
|   +--- BaudRate.java
|   +--- CSerialPort.java
|   +--- CSerialPortInfo.java
|   +--- cserialportJava.java
|   +--- cserialportJavaJNI.java
|   +--- cserialportJAVA_wrap.cxx
|   +--- cserialportJAVA_wrap.h
|   +--- CSerialPortListener.java
|   +--- DataBits.java
|   +--- FlowControl.java
|   +--- OperateMode.java
|   +--- Parity.java
|   +--- SerialPortError.java
|   +--- SerialPortInfo.java
|   +--- SerialPortInfoVector.java
|   +--- StopBits.java
+--- README.md
```

