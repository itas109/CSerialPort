# CSerialPort for JavaScript

```
swig: 4.1.0
cmake: 3.8.2
nodejs: 12.22.12
```

## Build

```
swig -javascript -c++ -node -outdir generate -I../../include cserialport.i
node-gyp configure build --debug
```

or

```
npm i -g cmake-js
cmake-js build -D
```

## Run

```
node example/index.js
```

