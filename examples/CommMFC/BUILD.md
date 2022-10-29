# MFC使用CSerialPort动态库

CommMFCDLL示例代码支持两种方式编译：

- cmake方式(可生成vs2008及以上的版本)
- Visual Studio 2008及以上直接调用(vs2008以上版本可通过Visual Studio进行升级)

注意事项：

- x64动态库只能用于x64的程序调用，x86同理
- debug的动态库只能用于动态库程序调用，release同理
- 动态库和使用的程序一般要使用相同编译器


## 1. 使用cmake生成CSerialPort动态库
```
$ git clone https://github.com/itas109/CSerialPort
$ cd CSerialPort
$ mkdir bin
$ cd bin
$ cmake .. -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON
$ cmake --build . --config Debug
$ cmake --install . --config Debug
```

CSerialPort安装目录结构：
```
D:/CSerialPort/bin $ tree
.
+--- bin
|   +--- libcserialport.dll
+--- include
|   +--- CSerialPort
|   |   +--- osplatformutil.h
|   |   +--- SerialPort.h
|   |   +--- SerialPortBase.h
|   |   +--- SerialPortInfo.h
|   |   +--- SerialPortInfoBase.h
|   |   +--- SerialPortInfoUnixBase.h
|   |   +--- SerialPortInfoWinBase.h
|   |   +--- SerialPortUnixBase.h
|   |   +--- SerialPortWinBase.h
|   |   +--- SerialPort_global.h
|   |   +--- SerialPort_version.h
|   |   +--- sigslot.h
+--- lib
|   +--- cmake
|   |   +--- CSerialPort
|   |   |   +--- cserialport-config.cmake
|   +--- libcserialport.lib
```

## 2. MFC中以cmake方式使用CSerialPort的动态库【推荐】

### 2.1 通过find_package自动搜索CSerialPort头文件及动态库【推荐】

```
cd D:/CSerialPort/examples/CommMFCDLL
mkdir bin
cd bin
cmake .. -DCMAKE_PREFIX_PATH="../../bin/install"
cmake --build . --config Debug
```

注意：
出现如下错误，可设置CMAKE_PREFIX_PATH指定搜索路径，如`cmake .. -DCMAKE_PREFIX_PATH="../../bin/install"`

```
Could not find a package configuration file provided by "CSerialPort" with
  any of the following names:

    CSerialPortConfig.cmake
    cserialport-config.cmake
```

### 2.2 手动指定头文件及动态库

注意：可修改`examples\CommMFCDLL\CMakeLists.txt`手动配置路径

CMakeLists.txt

```
cmake_minimum_required(VERSION 2.8.12)

project(CommMFCDLL)

find_package(MFC)
if (NOT MFC_FOUND)
  MESSAGE(FATAL_ERROR "MFC not found")
endif()

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2) # 1 the static MFC library 2 shared MFC library

set(MFCFiles CommMFCDLL.cpp CommMFCDLL.h CommMFCDLL.rc CommMFCDLLDlg.cpp CommMFCDLLDlg.h Resource.h stdafx.cpp stdafx.h targetver.h)

if (NOT CSERIALPORT_INSTALL_PATH)
    set(CSERIALPORT_INSTALL_PATH CACHE STRING "set CSerialPort Install Path" FORCE)
    set_property(CACHE CSERIALPORT_INSTALL_PATH PROPERTY STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/install")
endif()
include_directories(${CSERIALPORT_INSTALL_PATH}/include)
link_directories(${CSERIALPORT_INSTALL_PATH}/lib)

add_executable(${PROJECT_NAME} WIN32 ${MFCFiles})
target_link_libraries (${PROJECT_NAME} libcserialport)
```

```
cd D:/CSerialPort/examples/CommMFCDLL
mkdir bin
cd bin
cmake .. -DCSERIALPORT_INSTALL_PATH=../../bin/install
cmake --build . --config Debug
```

## 3. MFC中手动配置方式使用CSerialPort的动态库

### 3.1 添加头文件路径

右键【CommMFCDLL根命名空间】-【属性】-【C/C++】-【常规】-【附加包含目录】-添加CSerialPort的头文件目录

```
D:\CSerialPort\bin\install\include
```
或
```
$(ProjectDir)\..\..\bin\install\include
```

### 3.2 添加库文件路径

- 添加库文件目录

右键【CommMFC根命名空间】-【属性】-【链接器】-【常规】-【附加库目录】-添加CSerialPort的库文件目录

```
D:\CSerialPort\bin\install\lib
```
或
```
$(ProjectDir)\..\bin\install\lib
```

- 添加库附加依赖项

右键【CommMFC根命名空间】-【属性】-【链接器】-【输入】-【附加依赖项】-添加`libcserialport.lib`

---
Reference:
1. https://github.com/itas109/CSerialPort
2. https://gitee.com/itas109/CSerialPort
3. https://blog.csdn.net/itas109