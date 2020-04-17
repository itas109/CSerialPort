#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "SerialPortInfo.h"

#include <QDebug>
#include <QScrollBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this,&MainWindow::emitUpdateReceive,this,&MainWindow::OnUpdateReceive,Qt::QueuedConnection);

    //hide pushButtonReadSync
    ui->pushButtonReadSync->hide();

    //ui
    ui->pushButtonOpen->setText(tr("open"));

    rx = 0;
    tx = 0;


    setWindowTitle(this->windowTitle() + " " +QString::fromStdString(m_SerialPort.getVersion()));

    //init

    std::list<std::string> portNameList = CSerialPortInfo::availablePorts();

    list<string>::iterator itor;

    int i = 0;
    for (itor = portNameList.begin(); itor != portNameList.end(); ++itor)
    {
        ui->comboBoxPortName->insertItem(i,QString::fromLocal8Bit(string(*itor).c_str()));
        i++;
    }

    m_SerialPort.readReady.connect(this, &MainWindow::OnReceive);

    ui->comboBoxBaudrate->setCurrentText("9600");
    ui->comboBoxDataBit->setCurrentText("8");

    qDebug() << QString::fromStdString(m_SerialPort.getVersion());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnReceive()
{
    int iRet = -1;
    char * str = NULL;
    str = new char[512];
    iRet = m_SerialPort.readAllData(str);

//    qDebug() << "read length : " << iRet;

    if(iRet != -1)
    {
        // TODO: 中文需要由两个字符拼接，否则显示为空""
        QString m_str = QString::fromLocal8Bit(str,iRet);

//        qDebug() << "receive : " << m_str;

        emitUpdateReceive(m_str);
	}
    else
    {

    }

    delete str;
    str = NULL;

}

void MainWindow::OnUpdateReceive(QString str)
{
    rx += str.length();
//    qDebug() << "receive : " << rx;
    ui->labelRXValue->setText(QString::number(rx));

    //追加文本
    ui->plainTextEditReceive->moveCursor (QTextCursor::End);
    ui->plainTextEditReceive->insertPlainText(str);

    //移动滚动条到底部
//    QScrollBar *scrollbar = ui->plainTextEditReceive->verticalScrollBar();
//    if (scrollbar)
//    {
//        scrollbar->setSliderPosition(scrollbar->maximum());
//    }
}

void MainWindow::on_pushButtonOpen_clicked()
{
    if(ui->pushButtonOpen->text() == tr("open"))
    {
        if(ui->comboBoxPortName->count() > 0)
        {
            m_SerialPort.init(ui->comboBoxPortName->currentText().toStdString(),
                              ui->comboBoxBaudrate->currentText().toInt(),
                              itas109::Parity(ui->comboBoxParity->currentIndex()),
                              itas109::DataBits(ui->comboBoxDataBit->currentText().toInt()),
                              itas109::StopBits(ui->comboBoxStopBit->currentIndex()));

            if(m_SerialPort.open())
            {
                ui->pushButtonOpen->setText(tr("close"));
            }
            else
            {
                QMessageBox::information(NULL,tr("information"),tr("open port error"));
                ui->pushButtonOpen->setText(tr("open"));
                qDebug()<< m_SerialPort.getLastError();
            }
        }
        else
        {
            QMessageBox::information(NULL,tr("information"),tr("This Computer no avaiable port"));
            qDebug()<< "This Computer no avaiable port";
        }
    }
    else
    {
        m_SerialPort.close();

        ui->pushButtonOpen->setText(tr("open"));
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    if(m_SerialPort.isOpened())
    {
        QString sendStr = ui->plainTextEditSend->toPlainText();

        QByteArray ba = sendStr.toLocal8Bit();
        const char *s = ba.constData();

        // 支持中文并获取正确的长度
        m_SerialPort.writeData(s,ba.length());

        tx += sendStr.length();

        ui->labelTXValue->setText(QString::number(tx));
    }
    else
    {
        QMessageBox::information(NULL,tr("information"),tr("please open serial port first"));
        qDebug() << tr("please open serial port first");
    }
}

void MainWindow::on_pushButtonClear_clicked()
{
    tx = 0;
    rx = 0;

    ui->labelTXValue->setText(QString::number(tx));
    ui->labelRXValue->setText(QString::number(rx));
}

void MainWindow::on_checkBoxSync_stateChanged(int arg1)
{
    //not checked
    if(arg1 == 0)
    {
        m_SerialPort.close();
    }
    else
    {
        m_SerialPort.close();
    }
}

void MainWindow::on_pushButtonReadSync_clicked()
{
    OnReceive();
}

void MainWindow::on_checkBoxSync_clicked(bool checked)
{
    //AsynchronousOperate
    if(checked)
    {
        m_SerialPort.setOperateMode(itas109::SynchronousOperate);

        ui->pushButtonReadSync->show();
    }
    else
    {
        m_SerialPort.setOperateMode(itas109::AsynchronousOperate);

        ui->pushButtonReadSync->hide();
    }

}
