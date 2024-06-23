/**
 * @file index.js
 * @author itas109 (itas109@qq.com) \n\n
 * Blog : https://blog.csdn.net/itas109 \n
 * Github : https://github.com/itas109 \n
 * Gitee : https://gitee.com/itas109 \n
 * QQ Group : 129518033
 * @brief Node.js CSerialPort Example Node.js的CSerialPort示例程序
 */
'use strict'

const util = require('util');
const readline = require('readline');

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

let CSerialPort;
try {
    CSerialPort = require("./build/Debug/cserialport.node").CSerialPort;
} catch (error) {
    CSerialPort = require("./build/Release/cserialport.node").CSerialPort;
}

const sp = new CSerialPort();

console.log(util.format('Version: %s\n', sp.getVersion()));

const portInfoArray = sp.availablePortInfos();
const portInfoSize = portInfoArray.length;

if (0 == portInfoSize) {
    console.log("No valid port");
}
else {
    for (let i = 1; i <= portInfoSize; i++) {
        console.log(util.format('%d - %s %s %s', i, portInfoArray[i - 1].portName, portInfoArray[i - 1].description, portInfoArray[i - 1].hardwareId));
    }

    rl.question(util.format('Please Input The Index Of Port(1 - %d)\n', portInfoSize), (myInput) => {
        rl.close();

        if (myInput >= 1 && myInput <= portInfoSize) {
            let portName = portInfoArray[myInput - 1].portName;
            console.log(util.format('Port Name: %s', portName));

            sp.init(portName); // windows:COM1 Linux:/dev/ttyS0

            sp.open();
            console.log(util.format('Open %s %s', portName, sp.isOpen() ? 'Success' : 'Failed'));

            const hex = Buffer.from([0x31, 0x32, 0x33, 0x34, 0x35]);
            sp.writeData(hex, hex.length);

            const data = Buffer.from("itas109");
            sp.writeData(data, data.length);

            let countRead = 0;
            sp.onReadEvent((arrayBufferReadData) => {
                console.log(util.format('Count: %d, Length: %d, Str: %s, Hex: %s', ++countRead, arrayBufferReadData.length, arrayBufferReadData.toString(), arrayBufferReadData.toString('hex')));
            })

            // let countRead = 0;
            // for (; ;) {
            //     let readData = Buffer.alloc(1024);
            //     let recLen = sp.readData(readData, 1024);
            //     if (recLen > 0) {
            //         console.log(util.format('Count: %d, Length: %d, Str: %s, Hex: %s', ++countRead, recLen, readData.toString(), readData.toString('hex', 0, recLen)));

            //         // return receive data
            //         sp.writeData(readData, recLen);
            //     }
            // }
        }
    });
}