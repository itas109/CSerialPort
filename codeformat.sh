#!/bin/bash

find . -path '*/src/*.cpp' -o -path '*/include/CSerialPort/*.hpp' -o -path '*/include/CSerialPort/*.h' | xargs clang-format -style=file -i
