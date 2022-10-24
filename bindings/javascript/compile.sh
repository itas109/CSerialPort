#! /bin/bash

# swig 4.1.0
swig -javascript -node -c++ -outcurrentdir -I../../include ../interface/cserialport.i

node-gyp --debug configure build

node index.js
