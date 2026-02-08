#include <iostream>
#include <string>

// read file
#include <fstream>
#include <streambuf>

#include <mutex>
#include <condition_variable>

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
using namespace itas109;

// cpp-httplib v0.30.2
#include "httplib.h"

// nlohmann_json v3.7.3
#include "json.hpp"
using json = nlohmann::json;

bool isReceiveHex = false;

std::string readBuffer;
std::mutex readMutex;
std::condition_variable readCV;

std::string stringToHex(const std::string &input)
{
    static const char *const lut = "0123456789ABCDEF";
    size_t len = input.length();
    std::string output;
    output.reserve(2 * len);

    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }

    return output;
}

void HexStrToString(const std::string &input, char *des)
{
    const char *src = input.c_str();
    int len = (int)(input.size() / 2);
    while (len--)
    {
        *des = (*src > '9' ? *src + 9 : *src) << 4;
        ++src;
        *des |= (*src > '9' ? *src + 9 : *src) & 0x0F;
        ++src;
        ++des;
    }
}

class MyListener : public CSerialPortListener
{
public:
    MyListener(CSerialPort *sp, httplib::Server *svr)
        : p_sp(sp)
        , p_svr(svr) {};

    void onReadEvent(const char *portName, unsigned int readBufferLen)
    {
        if (readBufferLen > 0)
        {
            char *data = new char[readBufferLen + 1]; // '\0'

            if (data)
            {
                int recLen = p_sp->readData(data, readBufferLen);
                if (recLen > 0)
                {
                    std::lock_guard<std::mutex> lock(readMutex);
                    if (isReceiveHex)
                    {
                        readBuffer += stringToHex(std::string(data, recLen));
                    }
                    else
                    {
                        readBuffer.append(data, recLen);
                    }
                    readCV.notify_all();
                }

                delete[] data;
            }
        }
    };

private:
    CSerialPort *p_sp;
    httplib::Server *p_svr;
};

// index.html
std::string htmlContent =
    R"(
<!DOCTYPE html>
<html lang='en'>

<head>
    <meta charset='utf-8'>
    <title>CSerialPort Web Server</title>

    <style>
        #header {
            text-align: center;
            padding: 1px;
        }

        #left {
            position: absolute;
            width: 55%;
            height: 100%;
        }

        #right {
            margin-left: 55%;
            height: 100%;
        }

        #receive {
            width: 50vw;
            white-space: pre-wrap;
            word-wrap: break-word;
        }

        #log {
            width: 40vw;
            white-space: pre-wrap;
            word-wrap: break-word;
        }
    </style>
</head>

<body>
    <div id='header'>CSerialPort Web Server</div>

    <hr />

    <div>
        <label>PortName</label>
        <select id='listPortSelect'></select>
        <label>BaudRate</label>
        <select id='baudRateSelect'></select>
        <button id='onOpen' onclick='onOpen()'>open</button>
        <button id='onClose' onclick='onClose()'>close</button>
        <input type='checkbox' id='setDtr' onclick='onSetDtr(this)' />
        <label>setDtr</label>
        <input type='checkbox' id='setRts' onclick='onSetRts(this)' />
        <label>setRts</label>
    </div>

    <hr />

    <div>
        <h2>Send</h2>
        <input type='checkbox' id='isSendHex' />
        <label for="isSendHex">isSendHex</label>
        <input id='send' type='text' style='width:230px;' value='https://github.com/itas109/CSerialPort'>
        <button id='onSend' onclick='onSend()'>send</button>
        <input type='checkbox' id='sendCycle' onclick='onSendCycle()' />
        <label for="sendCycle">sendCycle</label>
        <input id='sendCycleInterval' type='text' style='width:50px;' value='1000'>
        <label for="sendCycleInterval">ms</label>
    </div>

    <hr />

    <div id='left'>
        <h2>Receive</h2>
        <input type='checkbox' id='isReceiveHex' onclick='onSetReceiveMode()' />
        <label for="isReceiveHex">isReceiveHex</label>
        <button id='onRead' onclick='onRead()'>read</button>
        <button id='clearReceive' onclick='onClearReceive()'>clear</button>
        <pre id='receive'></pre>
    </div>

    <div id='right'>
        <h2>Log</h2>
        <button id='clearLog' onclick='onClear()'>clear</button>
        <pre id='log'></pre>
    </div>

    <script>
        // tile
        (async function listPort() {
            const getVersion = await fetch('getVersion');
            const getVersionData = await getVersion.json();
            document.title += ' - ' + getVersionData.version;
        })();

        // listPortSelect
        const listPortSelect = document.getElementById('listPortSelect');
        (async function listPort() {
            const listPorts = await fetch('listPorts');
            const listPortsData = await listPorts.json();
            listPortsData.forEach(listPort => {
                const optionElement = document.createElement('option');
                optionElement.value = listPort.portName;
                optionElement.textContent = listPort.portName;
                listPortSelect.appendChild(optionElement);
            });
        })();

        // baudRateSelect
        const baudRateSelect = document.getElementById('baudRateSelect');
        const baudRateArray = [110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 921600];
        baudRateArray.forEach(baudRate => {
            const option = document.createElement('option');
            option.value = baudRate;
            option.text = baudRate;
            if (baudRate === 9600) {
                option.selected = true; // default selected
            }
            baudRateSelect.appendChild(option);
        });
    </script>
    <script>
        if ('EventSource' in window) {
            const source = new EventSource('/onReadEvent');

            source.onopen = function (event) {
                output("onReadEvent success");
            };

            source.onmessage = function (event) {
                outputReceive(event.data);
            };

            source.onerror = function (error) {
                output('EventSource failed. ' + JSON.stringify(error));
                source.close();
            };
        }
    </script>
    <script>
        function output(str, id = '#log') {
            document.querySelector(id).textContent += str + '\n';
        }

        function outputReceive(str, id = '#receive') {
            document.querySelector(id).textContent += str;
        }

        function onClear(id = '#log') {
            document.querySelector(id).textContent = '';
        }

        function onClearReceive(id = '#receive') {
            document.querySelector(id).textContent = '';
        }

        async function onOpen() {
            const params = {
                portName: document.querySelector('#listPortSelect option:checked').value,
                baudRate: parseInt(document.querySelector('#baudRateSelect option:checked').value)
            };

            await fetch('init', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(params)
            });
            let result = await fetch('open');
            let resultData = await result.json();
            output(JSON.stringify(resultData));
        }

        async function onClose() {
            await fetch('close');
            output('close ok');
        }

        async function onSend() {
            const str = document.querySelector('#send').value;
            const params = {
                isHex: document.querySelector('#isSendHex').checked,
                data: encodeURI(str),
                size: str.length
            };
            let result;
            result = await fetch('writeData', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(params)
            });
            const writeData = await result.json();
            output(JSON.stringify(writeData));
        }

        let sendCycleIntervalId = null;
        async function onSendCycle() {
            const isSendCycle = document.querySelector('#sendCycle').checked;
            const interval = parseInt(document.querySelector('#sendCycleInterval').value);
            if (isSendCycle) {
                sendCycleIntervalId = setInterval(async () => {
                    await onSend();
                }, interval);
            }
            else {
                if (sendCycleIntervalId !== null) {
                    clearInterval(sendCycleIntervalId);
                    sendCycleIntervalId = null;
                }
            }
        }

        async function onRead() {
            let result;
            result = await fetch('readData');
            const readData = await result.text();
            if (readData.length > 0) {
                outputReceive(readData);
            }
        }

        async function onSetReceiveMode() {
            const params = {
                isReceiveHex: document.querySelector('#isReceiveHex').checked
            };
            await fetch('setReceiveMode', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(params)
            });
        }

        async function onSetDtr(checkbox) {
            await fetch('setDtr', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    isSet: checkbox.checked
                })
            });
            output('setDtr ' + checkbox.checked);
        }

        async function onSetRts(checkbox) {
            await fetch('setRts', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    isSet: checkbox.checked
                })
            });
            output('setRts ' + checkbox.checked);
        }
    </script>

</body>

</html>
    )";

int main()
{
    CSerialPort sp;
    httplib::Server svr;
    MyListener listener(&sp, &svr);

    svr.Get("/listPorts", [](const httplib::Request &req, httplib::Response &res)
            {
               std::vector<SerialPortInfo> m_availablePortsList = CSerialPortInfo::availablePortInfos();

               json array = json::array();
               for (auto &portInfo : m_availablePortsList)
               {
                   json obj = json::object();
                   obj["portName"] = portInfo.portName;
                    obj["description"] = portInfo.description; // invalid UTF-8 byte at index
                   obj["hardwareId"] = portInfo.hardwareId;
                   array.push_back(obj);
               }
               res.set_content(array.dump(), "application/json"); });

    svr.Get("/getVersion", [&sp](const httplib::Request &req, httplib::Response &res)
            {
               json result = json::object();
               result["version"] = sp.getVersion();
               res.set_content(result.dump(), "application/json"); });

    svr.Post("/init", [&sp](const httplib::Request &req, httplib::Response &res)
             {
               json jsonReq = json::parse(req.body);
               std::string portName = jsonReq.at("portName").get<std::string>();
               int baudRate = jsonReq.at("baudRate").get<int>();
               sp.init(portName.c_str(), baudRate);
               res.set_content("{}", "application/json"); });

    svr.Get("/open", [&sp, &listener](const httplib::Request &req, httplib::Response &res)
            {
               json result = json::object();
               bool isOpen = sp.open();
               if (isOpen)
               {
                   sp.connectReadEvent(&listener);
               }
               result["open"] = isOpen;
               res.set_content(result.dump(), "application/json"); });

    svr.Get("/isOpen", [&sp](const httplib::Request &req, httplib::Response &res)
            {
               json result = json::object();
               result["isOpen"] = sp.isOpen();
               res.set_content(result.dump(), "application/json"); });

    svr.Get("/close", [&sp](const httplib::Request &req, httplib::Response &res)
            {
               sp.close();
               sp.disconnectReadEvent();
               res.set_content("{}", "application/json"); });

    svr.Post("/writeData", [&sp](const httplib::Request &req, httplib::Response &res)
             {
               json jsonReq = json::parse(req.body);
               std::string data = jsonReq.at("data").get<std::string>();
               int size = jsonReq.at("size").get<int>();
               bool isHex = jsonReq.at("isHex").get<bool>();
               int len = 0;
               if (isHex)
               {
                   int hexLen = (int)(data.size() / 2);
                   char *hex = new char[hexLen];
                   HexStrToString(data, hex);
                   len = sp.writeData(hex, hexLen);
               }
               else
               {
                   len = sp.writeData(data.c_str(), size);
               }
               json result = json::object();
               result["writeData"] = len;
               res.set_content(result.dump(), "application/json"); });

    svr.Get("/readData", [&sp](const httplib::Request &req, httplib::Response &res)
            {
            if (!readBuffer.empty())
            {
                res.set_content(readBuffer, "application/text");
                readBuffer.clear();
            } });

    svr.Get("/onReadEvent", [&sp](const httplib::Request &req, httplib::Response &res)
            {
                // 1. set SSE header
                // res.set_header("Content-Type", "text/event-stream");
                res.set_header("Cache-Control", "no-cache");
                res.set_header("Connection", "keep-alive");

                // 2. push data
                res.set_chunked_content_provider(
                    "text/event-stream",
                    [](size_t offset, httplib::DataSink &sink)
                    {
                        while (true)
                        {
                            std::string current_data;
                            {
                                std::unique_lock<std::mutex> lock(readMutex);
                                readCV.wait(lock, []{ return !readBuffer.empty(); });

                                current_data = "data: " + readBuffer + "\n\n";
                                readBuffer.clear();
                            }

                            if (!sink.write(current_data.c_str(), current_data.size()))
                            {
                                return false; // client disconnect
                            }

                            if (sink.is_writable() == false)
                            {
                                return false;
                            }
                        }
                        return true;
                    },
                    [](bool success) {
                        std::cout << "SSE Client Disconnected" << std::endl;
                    }
                    ); });

    svr.Post("/setReceiveMode", [=](const httplib::Request &req, httplib::Response &res)
             {
               json jsonReq = json::parse(req.body);
               isReceiveHex = jsonReq.at("isReceiveHex").get<bool>();
               res.set_content("{}", "application/json"); });

    svr.Post("/setDtr", [&sp](const httplib::Request &req, httplib::Response &res)
             {
               json jsonReq = json::parse(req.body);
               bool isSet = jsonReq.at("isSet").get<bool>();
               sp.setDtr(isSet);
               res.set_content("{}", "application/json"); });

    svr.Post("/setRts", [&sp](const httplib::Request &req, httplib::Response &res)
             {
               json jsonReq = json::parse(req.body);
               bool isSet = jsonReq.at("isSet").get<bool>();
               sp.setRts(isSet);
               res.set_content("{}", "application/json"); });

    // svr.set_mount_point("/", "."); // index.html static file
    svr.Get("/", [](const httplib::Request &, httplib::Response &res)
            { 
                std::ifstream file("index.html");
                if (file.is_open())
                {
                    htmlContent = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                }
                else
                {
                    std::cout << "index.html file not found. use default html" << std::endl;
                }
                res.set_content(htmlContent, "text/html; charset=UTF-8"); });

    std::cout << "http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}