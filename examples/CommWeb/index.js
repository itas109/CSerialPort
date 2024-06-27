'use strict';

const http = require('http');
const express = require('express');
const app = express();
const port = 8080;
let httpServer = http.createServer(app);

let CSerialPort;
try {
    CSerialPort = require("./build/Debug/cserialport.node").CSerialPort;
} catch (error) {
    CSerialPort = require("./build/Release/cserialport.node").CSerialPort;
}
const sp = new CSerialPort();

// // morgan logger dev
// const logger = require('morgan');
// app.use(logger('dev'));
// console.log('load Third-party middleware - morgan logger dev');

const path = require('path');
app.use(express.static(path.join(__dirname, 'public')));

app.get('/', function (req, res, next) {
    res.send('index');
})

app.get('/pid', function (req, res, next) {
    res.send(`${process.pid}`);
})

app.get('/gc', function (req, res, next) {
    try {
        global.gc();
        res.send(JSON.stringify(process.memoryUsage()));
    } catch (error) {
        res.send("Please start app with --expose-gc, such as node --expose-gc index.js");
    }
})

app.get('/exit', function (req, res, next) {
    res.send('exit');
    process.exit(0);
})

app.get('/cserialport/listPorts', function (req, res, next) {
    const portInfoArray = sp.availablePortInfos();
    const portInfoSize = portInfoArray.length;
    let result = [];
    for (let i = 1; i <= portInfoSize; i++) {
        result.push(portInfoArray[i - 1].portName);
    }
    res.send(result);
})

app.get('/cserialport/init', function (req, res, next) {
    let portName = req.query.portName;
    sp.init(portName);
    res.send(portName + ' init ok');
})

app.get('/cserialport/init2', function (req, res, next) {
    let portName = req.query.portName;
    let baudRate = req.query.baudRate;
    sp.init2(portName, baudRate);
    res.send(portName + ' init ok');
})

app.get('/cserialport/open', function (req, res, next) {
    if (sp.isOpen()) {
        res.send('alread open');
    } else {
        let result = sp.open();
        res.send('open ' + result);
    }
})

app.get('/cserialport/close', function (req, res, next) {
    sp.close();
    res.send('close ok');
})

app.get('/cserialport/isOpen', function (req, res, next) {
    let result = sp.isOpen();
    res.send(result);
})

app.get('/cserialport/writeData', function (req, res, next) {
    let isHex = JSON.parse(req.query.isHex);
    let data = decodeURI(req.query.data);
    let buffer;
    if (isHex) {
        data = data.replace(/\s*/g, "");
        buffer = Buffer.from(data, 'hex');
    } else {
        buffer = Buffer.from(data);
    }
    let result = sp.writeData(buffer, buffer.length);
    res.send("writeData " + result);
})

app.get('/cserialport/readData', function (req, res, next) {
    let isHex = JSON.parse(req.query.isHex);
    let readBufferLen = req.query.readBufferLen;
    readBufferLen = readBufferLen > 0 ? readBufferLen : 1;
    let buffer = Buffer.alloc(readBufferLen);
    sp.readData(buffer, buffer.length);
    if (isHex) {
        res.send(buffer.toString('hex'));
    } else {
        res.send(buffer.toString());
    }
})

let isReceiveHex = false;
app.get('/cserialport/isReceiveHex', function (req, res, next) {
    isReceiveHex = JSON.parse(req.query.isReceiveHex);
    res.send('isReceiveHex ' + isReceiveHex);
})

app.get('/cserialport/onReadEvent', function (req, res, next) {
    res.setHeader('Content-Type', 'text/event-stream');
    res.setHeader('Cache-Control', 'no-cache');
    res.setHeader('Connection', 'keep-alive');
    sp.onReadEvent((arrayBufferReadData) => {
        if (isReceiveHex) {
            res.write("data: " + arrayBufferReadData.toString('hex') + "\n\n");
        } else {
            res.write("data: " + arrayBufferReadData.toString() + "\n\n");
        }
    });

    req.on('close', () => {
        res.end();
    });
})

app.get('/cserialport/setDtr', function (req, res, next) {
    let isSet = JSON.parse(req.query.isSet);
    sp.setDtr(isSet);
    res.send('setDtr ' + isSet + ' ok');
})

app.get('/cserialport/setRts', function (req, res, next) {
    let isSet = JSON.parse(req.query.isSet);
    sp.setRts(isSet);
    res.send('setRts ' + isSet + ' ok');
})

app.get('/cserialport/getVersion', function (req, res, next) {
    res.send(sp.getVersion());
})

httpServer.listen(port);

console.log('http://localhost:' + port);
