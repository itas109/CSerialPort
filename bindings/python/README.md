# CSerialPort for Python

```
swig: 4.1.0
cmake: 3.8.2
python: 3.6.8
```

## Build

```
mkdir bin
cd bin
cmake .. 
cmake --build . --config Release
```

## Run

```
cmake -E copy generate/cserialport.py example
cmake -E copy bin/bin/Release/_cserialport.pyd example
python example/main.py
```

