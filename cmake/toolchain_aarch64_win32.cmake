# 交叉编译的系统名称
set(CMAKE_SYSTEM_NAME Linux)
# 交叉编译的CPU架构
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# 指明C和C++编译器
# https://releases.linaro.org/components/toolchain/binaries/7.5-2019.12/aarch64-linux-gnu/gcc-linaro-7.5.0-2019.12-i686-mingw32_aarch64-linux-gnu.tar.xz
set(CMAKE_C_COMPILER D:/gcc-linaro-7.5.0-2019.12-i686-mingw32_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc.exe)
set(CMAKE_CXX_COMPILER D:/gcc-linaro-7.5.0-2019.12-i686-mingw32_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++.exe)

# 对FIND_PROGRAM()起作用，有三种取值，NEVER,ONLY,BOTH,第一个表示不在你CMAKE_FIND_ROOT_PATH下进行查找，第二个表示只在这个路径下查找，第三个表示先查找这个路径，再查找全局路径，对于这个变量来说，一般都是调用宿主机的程序，所以一般都设置成NEVER
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# 下面的三个选项表示只在交叉环境中查找库和头文件
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
