const { app, BrowserWindow } = require('electron/main');
const path = require('path');

const createWindow = () => {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            nodeIntegration: true, // not safe
            contextIsolation: false, // not safe
            preload: path.join(__dirname, 'preload.js'), // preload script must absolute path
        }
    })

    win.loadFile('public/index.html');
}

app.whenReady().then(() => {
    createWindow();
})
