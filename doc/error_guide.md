# 1.引入头文件错误import error 

## 错误信息error info

OS : Windows 7 64bit CN

VS : 2013 、 2015 update 3

CSerialPort : 4.0.0.181210 beta

```
1>CST2.obj : error LNK2019: 无法解析的外部符号 "public: virtual __thiscall itas109::CSerialPort::~CSerialPort(void)" (??1CSerialPort@itas109@@UAE@XZ)，该符号在函数 "public: virtual void * __thiscall itas109::CSerialPort::`vector deleting destructor'(unsigned int)" (??_ECSerialPort@itas109@@UAEPAXI@Z) 中被引用
1>CST2Dlg.obj : error LNK2001: 无法解析的外部符号 "public: virtual __thiscall itas109::CSerialPort::~CSerialPort(void)" (??1CSerialPort@itas109@@UAE@XZ)
```

## 原因分析analysis
暂无

## 解决方案solution
需要导入全部的文件到工程，不能只引用头文件，并且将cpp文件的预编译头修改为"不使用预编译头"。

后续会出一个操作视频。

# 2.this->m_mutex-> is nullptr

## 错误信息error info

OS : Windows 7 64bit CN

VS : 2013 、 2015 update 3

CSerialPort : 4.0.0.181210 beta

```
	template<class mt_policy>
	class lock_block
	{
	public:
		mt_policy *m_mutex;

		lock_block(mt_policy *mtx)
			: m_mutex(mtx)
		{
			m_mutex->lock();//error code line
		}

		~lock_block()
		{
			m_mutex->unlock();
		}
	};
```

## 原因分析analysis
1.VS2015 update 3 测试发现，在CSerialPort声明为全局变量时，sigslot.h类库会引发空指针异常。
目前原因未完全定位

2.VS2013未发现该问题

3.目前测试发现在MFC自动生成的类内定义CSerialPort成员变量正常，但是其他类中也会有崩溃问题，等待后续分析

## 解决方案solution
在MFC自动生成的类内定义CSerialPort成员变量

# 3.CommQT连续接收数据是程序崩溃

## 错误信息error info

```
QTextLine: Can't set a line width while not layouting.
QTextLayout::createLine: Called without layouting
QTextLayout::endLayout: Called without beginLayout()
```

## 原因分析analysis

## 解决方案solution

# 4.QT的ui控件写数据异常

## 错误信息error info

```
QObject::connect: Cannot queue arguments of type 'QTextBlock'
(Make sure 'QTextBlock' is registered using qRegisterMetaType().)
QObject: Cannot create children for a parent that is in a different thread.
(Parent is QTextDocument(0x966ae0), parent's thread is QThread(0x93a328), current thread is QThread(0x25f5d88)
QObject::connect: Cannot queue arguments of type 'QTextCursor'
(Make sure 'QTextCursor' is registered using qRegisterMetaType().)
```

## 原因分析analysis

不能在不用线程中操作UI

## 解决方案solution

```
// mainwindow.h
class MainWindow : public QMainWindow, public has_slots<>
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void OnReceive();

    void OnUpdateReceive(QString str);

signals:
    void emitUpdateReceive(QString str);

	...
};
```

```
// mainwindow.cpp

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this,&MainWindow::emitUpdateReceive,this,&MainWindow::OnUpdateReceive,Qt::QueuedConnection);

	...
}

void MainWindow::OnReceive()
{
	...    
	emitUpdateReceive(m_str);
	...
}

void MainWindow::OnUpdateReceive(QString str)
{
	// update ui
}
```

# 5.fatal error C1010预编译头错误

## 错误信息error info

```
fatal error C1010: 在查找预编译头时遇到意外的文件结尾。是否忘记了向源中添加“#include "stdafx.h"”?

fatal error C1010: unexpected end of file while looking for precompiled header. Did you forget to add '#include "stdafx.h"' to your source?
```


## 原因分析analysis
建立工程时默认勾选了`预编译头`选项导致的

## 解决方案solution

关闭相应cpp的预处理头 : 

`cpp文件右键属性 -> C/C++ -> 预编译头`

设置`预编译头`选项为`不使用预编译头`


`cpp Properties -> C/C++ -> Precompiled Headers`

set `Precompiled Header` to `Not Using Precompiled Header`

## 6.接收数据缺少

## 错误信息error info

仅测试了windows

A.数据缺少
CSerialPort::CSerialPort()
{
	p_serialPortBase = new CSERIALPORTBASE();

    p_serialPortBase->setMinByteReadNoify(2);
	//p_serialPortBase->setMinByteReadNoify(1);

	((CSERIALPORTBASE *)p_serialPortBase)->readReady.connect(this, &CSerialPort::onReadReady);
}

B.数据正常
{
	p_serialPortBase = new CSERIALPORTBASE();

    //p_serialPortBase->setMinByteReadNoify(2);
	p_serialPortBase->setMinByteReadNoify(1);

	((CSERIALPORTBASE *)p_serialPortBase)->readReady.connect(this, &CSerialPort::onReadReady);
}

## 原因分析analysis

可能原因：
单个字符每次都触发
但是2个及2个以上字符，只有超过最小数才能触发，所以数据会少，只有等待下次满足条件能触发。

## 解决方案solution

合理的优化：
设置等待超时，例如最小数设置为2，但是如果两次接收间隔超过50ms，则认为已经不会再来数据，同样要触发接收。
