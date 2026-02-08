TARGET = CSerialPortWebServer
CROSS_COMPILE ?=
CC = $(CROSS_COMPILE)g++
# CC=/usr/bin/arm-linux-gnueabi-g++ # make CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
# CC=D:/gcc-linaro-7.5.0-2019.12-i686-mingw32_arm-linux-gnueabi/bin/arm-linux-gnueabi-g++

CUR_DIR  = .
SRC_DIR  = ${CUR_DIR}/../../src
INC_DIR  = -I${CUR_DIR}/../../include
CUSTOM_CCFLAGS = -std=c++11 ${INC_DIR} -DCSERIALPORT_DEBUG -lpthread

# compiler flags
CCFLAGS = -O2 ${CUSTOM_CCFLAGS}
LDFLAGS = -lpthread

# source files
SRC = ${CUR_DIR}/main.cpp \
      ${SRC_DIR}/SerialPortInfo.cpp \
      ${SRC_DIR}/SerialPortInfoBase.cpp \
      ${SRC_DIR}/SerialPortInfoUnixBase.cpp \
      ${SRC_DIR}/SerialPort.cpp \
      ${SRC_DIR}/SerialPortBase.cpp \
      ${SRC_DIR}/SerialPortAsyncBase.cpp \
      ${SRC_DIR}/SerialPortUnixBase.cpp

# *.cpp to *.o
OBJ = $(notdir $(SRC:.cpp=.o))

# release(default)
all: ${TARGET}

# debug
debug: CCFLAGS = -g ${CUSTOM_CCFLAGS}
debug: LDFLAGS = -lpthread
debug: clean all

# compile
# [RECOMMEND] compile *.cpp to *.o, and link *.o to main
# ${TARGET}: ${OBJ}
# 	${CC} ${OBJ} -o ${TARGET} ${LDFLAGS}
# 	@echo "compile ${TARGET} done."

# %.o: ${SRC_DIR}/%.cpp
# 	${CC} ${CCFLAGS} -c $< -o $@

# %.o: ${CUR_DIR}/%.cpp
# 	${CC} ${CCFLAGS} -c $< -o $@

# compile all files everytime(only main, no *.o)
${TARGET}: ${SRC}
	${CC} ${SRC} ${CCFLAGS} -o ${TARGET}
	@echo "compile ${TARGET} done."

clean:
	@rm -f *.o ${TARGET} *~ *.out
	@echo "Clean done."

rebuild: clean all

.PHONY: all clean debug rebuild