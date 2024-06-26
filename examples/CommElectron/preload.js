let CSerialPort;
try {
    CSerialPort = require("./build/Debug/cserialport.node").CSerialPort;
} catch (error) {
    CSerialPort = require("./build/Release/cserialport.node").CSerialPort;
}
global.gCSerialPort = new CSerialPort();
global.gIsHex = false;