CUR_DIR=.

DEMO_DIR=${CUR_DIR}
SRC_DIR=${CUR_DIR}/../../src

INC_DIR=-I${CUR_DIR}/../../include


SRC =   ${DEMO_DIR}/CSerialPortDemoNoGui.cpp \
        ${SRC_DIR}/SerialPortInfo.cpp \
	${SRC_DIR}/SerialPortInfoBase.cpp \
      	${SRC_DIR}/SerialPortInfoUnixBase.cpp \
	${SRC_DIR}/SerialPort.cpp \
	${SRC_DIR}/SerialPortBase.cpp \
	${SRC_DIR}/SerialPortUnixBase.cpp \

OBJ = ${SRC}

TARGET=CSerialPortDemoNoGui
CC=g++

# include header
CCFLAGS += ${INC_DIR}

# pthread support
CCFLAGS += -lpthread


${TARGET}: ${OBJ}
	${CC} ${OBJ} ${CCFLAGS} -o ${TARGET}
	@echo "Compile done."

clean:
	@rm -f *~ *.out
	@echo "Clean tempreator files done."

	@rm -f ${TARGET}
	@echo "Clean target files done."

	@echo "Clean done."
