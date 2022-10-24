REM swig 4.1.0
swig -javascript -node -c++ -outcurrentdir -I../../include ../interface/cserialport.i

call node-gyp --debug configure build

node index.js

pause