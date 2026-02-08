# CSerialPort for WebServer

```
cmake: 3.10.0
webServer: cpp-httplib v0.30.2
json: nlohmann_json v3.7.3
```

## Build

- cmake

```
cd example/CommWebServer
mkdir bin && cd bin
cmake ..
cmake --build .
```

- make

```
cd example/CommWebServer
make

# cross compile
make CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
```

## Run

```
./CommWebServer
```

Tree

```
tree
.
+--- CMakeLists.txt
+--- httplib.h
+--- index.html
+--- json.hpp
+--- main.cpp
+--- README.md
```
