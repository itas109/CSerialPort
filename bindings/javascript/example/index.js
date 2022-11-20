/**
 * @file index.js
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief JavaScript CSerialPort Example  JavaScript的CSerialPort示例程序
 */
const util = require('util');
const readline = require('readline');

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

let addon;
try {
    addon = require("../build/Debug/cserialport.node");
} catch (error) {
    addon = require("../build/bin/Release/cserialport.node");
}

let sp = new addon.CSerialPort();
console.log(util.format('Version: %s\n', sp.getVersion()));

let spInfoVec = new addon.SerialPortInfoVector();
spInfoVec = addon.CSerialPortInfo.availablePortInfos();
console.log("availableFriendlyPorts: ");
const spInfoVecSize = spInfoVec.size();
if (0 == spInfoVecSize) {
    console.log("No valid port");
}
else {
    for (let i = 1; i <= spInfoVecSize; i++) {
        console.log(util.format('%d - %s %s', i, spInfoVec.get(i - 1).portName, spInfoVec.get(i - 1).description));
    }

    rl.question(util.format('Please Input The Index Of Port(1 - %d)\n', spInfoVecSize), (myInput) => {
        rl.close();

        if (myInput >= 1 && myInput <= spInfoVecSize) {
            let portName = spInfoVec.get(myInput - 1).portName;
            console.log(util.format('Port Name: %s', portName));

            sp.init(portName,     // windows:COM1 Linux:/dev/ttyS0
                9600,             // baudrate
                addon.ParityNone, // parity
                addon.DataBits8,  // data bit
                addon.StopOne,    // stop bit
                addon.FlowNone,   // flow
                4096              // read buffer size
            );
            sp.open();
            console.log(util.format('Open %s %s', portName, sp.isOpen() ? 'Success' : 'Failed'));

            // connect for read
            // sp.connectReadEvent(listener)

            // write hex data
            let hex = addon.malloc_void(5);
            addon.memmove(hex, '\x31\x32\x33\x34\x35');
            sp.writeData(hex, 5);
            // addon.free_void(hex);

            // write str data
            let data = addon.malloc_void(7);
            addon.memmove(data, "itas109");
            sp.writeData(data, 7);
            // addon.free_void(data);

            let readData = addon.malloc_void(4096);
            let countRead = 0;
            for (; ;) {
                let recLen = sp.readAllData(readData);
                if (recLen > 0) {
                    let str = addon.cdata(readData, recLen);
                    console.log(util.format('Count: %d, Length: %d, Str: %s, Hex: %s', ++countRead, recLen, str, Buffer.from(str, 'utf8').toString('hex')));

                    // return receive data
                    sp.writeData(readData, recLen);
                }
            }
        }
    });
}

delete spInfoVec;