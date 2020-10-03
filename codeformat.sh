#!/bin/bash

find . -path '*/src/*.cpp' -o -path '*/include/CSerialPort/*.h' ! -name 'sigslot.h' | xargs clang-format -style=file -i
