#include "webview.h"

#include <iostream>
#include <string>

// read file
#include <fstream>
#include <streambuf>

#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"
using namespace itas109;

#include "json.hpp"
using json = nlohmann::json;

bool isReceiveHex = false;

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
    MyListener(CSerialPort *sp, webview::webview *w)
        : p_sp(sp)
        , p_w(w){};

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
                    std::string *jsCode = new std::string("outputReceive('");
                    if (isReceiveHex)
                    {
                        *jsCode += stringToHex(std::string(data, readBufferLen));
                    }
                    else
                    {
                        *jsCode += std::string(data, readBufferLen);
                    }
                    *jsCode += "')";
                    p_w->dispatch(
                        [this, jsCode]()
                        {
                            p_w->eval(*jsCode);
                            delete jsCode;
                        });
                }

                delete[] data;
            }
        }
    };

private:
    CSerialPort *p_sp;
    webview::webview *p_w;
};

// index.html
std::string htmlContent =
    R"(
<!DOCTYPE html>
<html lang='en'>

<head>
    <meta charset='utf-8'>
    <title>CSerialPort Webview Example</title>

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
    <div id='header'>CSerialPort Webview Example</div>

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
        <input type='checkbox' id='isSendHex' />
        <label>isSendHex</label>
        <label>Send</label>
        <input id='send' type='text' style='width:230px;' value='https://github.com/itas109/CSerialPort'>
        <button id='onSend' onclick='onSend()'>send</button>
    </div>

    <hr />

    <div id='left'>
        <h2>Receive</h2>
        <input type='checkbox' id='isReceiveHex' onclick='onSetReceiveMode()' />
        <label>isReceiveHex</label>
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
            let result = await window.getVersion();
            document.title += ' - ' + result.version;
        })();

        // listPortSelect
        const listPortSelect = document.getElementById('listPortSelect');
        (async function listPort() {
            const listPorts = await window.listPorts();
            listPorts.forEach(listPort => {
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

            await window.init(params);
            let result = await window.open({});
            output(JSON.stringify(result));
        }

        async function onClose() {
            await window.close({});
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
            result = await window.writeData(params);
            output(JSON.stringify(result));
        }

        async function onSetReceiveMode() {
            const params = {
                isReceiveHex: document.querySelector('#isReceiveHex').checked
            };
            await window.setReceiveMode(params);
        }

        async function onSetDtr(checkbox) {
            await window.setDtr({
                isSet: checkbox.checked
            });
            output('setDtr ' + checkbox.checked);
        }

        async function onSetRts(checkbox) {
            await window.setRts({
                isSet: checkbox.checked
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
    webview::webview w(true, nullptr);
    MyListener listener(&sp, &w);
    std::string tile = "CSerialPort Webview Example - ";
    tile += sp.getVersion();
    w.set_title(tile);
    w.set_size(800, 600, WEBVIEW_HINT_NONE);

    w.bind("listPorts",
           [](const std::string &req) -> std::string
           {
               std::vector<SerialPortInfo> m_availablePortsList = CSerialPortInfo::availablePortInfos();

               json array = json::array();
               for (auto &portInfo : m_availablePortsList)
               {
                   json obj = json::object();
                   obj["portName"] = portInfo.portName;
                   // obj["description"] = portInfo.description; // invalid UTF-8 byte at index
                   obj["hardwareId"] = portInfo.hardwareId;
                   array.push_back(obj);
               }
               return array.dump();
           });

    w.bind("init",
           [&sp](const std::string &req) -> std::string
           {
               json jsonReq = json::parse(req);
               std::string portName = jsonReq.at(0).at("portName").get<std::string>();
               int baudRate = jsonReq.at(0).at("baudRate").get<int>();
               sp.init(portName.c_str(), baudRate);
               return "{}";
           });

    w.bind("open",
           [&sp, &listener](const std::string &req) -> std::string
           {
               json result = json::object();
               bool isOpen = sp.open();
               if (isOpen)
               {
                   sp.connectReadEvent(&listener);
               }
               result["open"] = isOpen;
               return result.dump();
           });

    w.bind("isOpen",
           [&sp](const std::string &req) -> std::string
           {
               json result = json::object();
               result["isOpen"] = sp.isOpen();
               return result.dump();
           });

    w.bind("close",
           [&sp](const std::string &req) -> std::string
           {
               sp.close();
               sp.disconnectReadEvent();
               return "{}";
           });

    w.bind("writeData",
           [&sp](const std::string &req) -> std::string
           {
               json jsonReq = json::parse(req);
               std::string data = jsonReq.at(0).at("data").get<std::string>();
               int size = jsonReq.at(0).at("size").get<int>();
               bool isHex = jsonReq.at(0).at("isHex").get<bool>();
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
               return result.dump();
           });
    w.bind("setReceiveMode",
           [=](const std::string &req) -> std::string
           {
               json jsonReq = json::parse(req);
               isReceiveHex = jsonReq.at(0).at("isReceiveHex").get<bool>();
               return "{}";
           });

    w.bind("setDtr",
           [&sp](const std::string &req) -> std::string
           {
               json jsonReq = json::parse(req);
               bool isSet = jsonReq.at(0).at("isSet").get<bool>();
               sp.setDtr(isSet);
               return "{}";
           });

    w.bind("setRts",
           [&sp](const std::string &req) -> std::string
           {
               json jsonReq = json::parse(req);
               bool isSet = jsonReq.at(0).at("isSet").get<bool>();
               sp.setRts(isSet);
               return "{}";
           });

    w.bind("getVersion",
           [&sp](const std::string &req) -> std::string
           {
               json result = json::object();
               result["version"] = sp.getVersion();
               return result.dump();
           });

    std::ifstream file("index.html");
    if (file.is_open())
    {
        htmlContent = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
    else
    {
        std::cerr << "open index.html file failed. use default html" << std::endl;
    }
    // w.navigate("file:///E:/Git/CSerialPort_upload/examples/CommWebview/bin/Debug/index.html");
    w.set_html(htmlContent);
    w.run();

    return 0;
}