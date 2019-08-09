#g++ SerialPortInfo.cpp SerialPortInfoBase.cpp SerialPortInfoUnixBase.cpp SerialPort.cpp SerialPortBase.cpp SerialPortUnixBase.cpp -fPIC -lpthread -shared -o libcserialport.so
CUR_DIR=.

DEMO_DIR=${CUR_DIR}
SRC_DIR=${CUR_DIR}/../../src

INC_DIR= -I${SRC_DIR}

SRC = ${SRC_DIR}/SerialPortInfo.cpp \
	  ${SRC_DIR}/SerialPortInfoBase.cpp \
      ${SRC_DIR}/SerialPortInfoUnixBase.cpp \
	  ${SRC_DIR}/SerialPort.cpp \
	  ${SRC_DIR}/SerialPortBase.cpp \
	  ${SRC_DIR}/SerialPortUnixBase.cpp \

OBJ = ${SRC}

TARGET=libcserialport.so
CC=g++

# include header
CCFLAGS += ${INC_DIR}

# include header
CCFLAGS += -fPIC -shared

# pthread support
CCFLAGS += -lpthread

${TARGET}: ${OBJ}
	${CC} ${OBJ} ${CCFLAGS} -o ${TARGET}
	@echo "Compile library done."

clean:
	@rm -f ${TARGET}
	@echo "Clean target files done."

	@echo "Clean done."
