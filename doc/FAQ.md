# Frequently Asked Questions


Q1 : CSerialPort可以在一个程序中定义多个对象吗？

A : 可以，定义多个串口对象，并且最好关联不同的响应函数

---

Q2 : 有没有办法保证接收的数据不截断？

A : 没有办法保证数据不截断，但是可以使用通信协议的方式将数据拼成一帧完成数据再处理

---

Q3 : 打开串口后报SetCommState()错误

A : 一般情况下是初始化参数错误，可以先使用默认参数，然后逐步添加参数排查问题

---

Q4 : 如何编译动态库?

A : 工程目录CSerialPort\lib下分别有Linux和Windows的动态库编译方法

---

Q5 : 如何调用动态库?

A : 

windows下可以运行CSerialPort\Demo\CommDLL的示例

linux下将生成的libcserialport.so文件拷贝到CSerialPort\Demo\CommNoGui目录，执行下列命令即可

```
    g++ CSerialPortDemoNoGui.cpp -o CSerialPortDemoNoGui -I../../src -L. -lcserialport

    export LD_LIBRARY_PATH=./

    ./CSerialPortDemoNoGui
```

