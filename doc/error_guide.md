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