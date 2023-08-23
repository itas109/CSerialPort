# CSerialPort Benchmark

## 1. compile

```
mkdir bin
cd bin
cmake ..
cmake --build . --config=Release
```

## 2. run receive mode

```
CSerialPortBenchmark COM1 0 > read.csv
```

## 3. run send mode

```
CSerialPortBenchmark COM2 1 > write.csv
```

## 4. check result

| readIndex | COM1  data |
| --------- | ---------- |
| 1         | 0          |
| 2         | 1          |
| 3         | 2          |
| 4         | 3          |
| 5         | 4          |
| 6         | 5          |
| ...       | ...        |
| 199995    | 58         |
| 199996    | 59         |
| 199997    | 60         |
| 199998    | 61         |
| 199999    | 62         |
| 200000    | 63         |
| time(ms)  | 385840     |