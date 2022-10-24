let addon;
try {
    addon = require("./build/Debug/cserialport.node");
} catch (error) {
    addon = require("./build/Release/cserialport.node");
}

let spInfo = new addon.SerialPortInfoVector();
spInfo = addon.CSerialPortInfo.availablePortInfos();
console.log("availableFriendlyPorts: ");
const spInfoSize = spInfo.size();
if (0 == spInfoSize) {
    console.log("No valid port");
}
else {
    for (let i = 0; i < spInfoSize; i++) {
        console.log(i + ' - ' + spInfo.get(i).portName);
    }
}

delete spInfo;