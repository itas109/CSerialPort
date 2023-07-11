#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "CSerialPort/SerialPortInfo.h"

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

    vector<SerialPortInfo> portNameList = CSerialPortInfo::availablePortInfos();

    for (size_t i = 0; i < portNameList.size(); i++)
    {
        ui->comboBoxPortName->insertItem(i,QString::fromLocal8Bit(portNameList[i].portName));
    }

    m_SerialPort.connectReadEvent(this);

    ui->comboBoxBaudrate->setCurrentText("9600");
    ui->comboBoxDataBit->setCurrentText("8");

    qDebug() << QString::fromStdString(m_SerialPort.getVersion());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onReadEvent(const char *portName, unsigned int readBufferLen)
{
    if(readBufferLen > 0)
    {
        int recLen = 0;
        char * str = NULL;
        str = new char[readBufferLen];
        recLen = m_SerialPort.readData(str, readBufferLen);

        if(recLen > 0)
        {
            // TODO: 中文需要由两个字符拼接，否则显示为空""
            QString m_str = QString::fromLocal8Bit(str,recLen);
            emitUpdateReceive(m_str);
        }
        else
        {

        }

        if(str)
        {
            delete[] str;
            str = NULL;
        }
    }
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
            m_SerialPort.init(ui->comboBoxPortName->currentText().toStdString().c_str(),
                              ui->comboBoxBaudrate->currentText().toInt(),
                              itas109::Parity(ui->comboBoxParity->currentIndex()),
                              itas109::DataBits(ui->comboBoxDataBit->currentText().toInt()),
                              itas109::StopBits(ui->comboBoxStopBit->currentIndex()));

            m_SerialPort.setReadIntervalTimeout(ui->lineEditReadIntervalTimeoutMS->text().toInt());

            if(m_SerialPort.open())
            {
                ui->pushButtonOpen->setText(tr("close"));
            }
            else
            {
                QMessageBox::information(NULL,tr("information"),tr("open port error") + QString("\n\ncode: %1\nmessage: %2").arg(m_SerialPort.getLastError()).arg(m_SerialPort.getLastErrorMsg()));
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
    if(m_SerialPort.isOpen())
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
    int recLen = 0;
    char str[4096] = {0};
    recLen = m_SerialPort.readAllData(str);

    if(recLen > 0)
    {
        // TODO: 中文需要由两个字符拼接，否则显示为空""
        QString m_str = QString::fromLocal8Bit(str,recLen);
        emitUpdateReceive(m_str);
    }
    else
    {

    }
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

void MainWindow::on_checkBoxDTR_clicked(bool checked)
{
    m_SerialPort.setDtr(checked);
}

void MainWindow::on_checkBoxRTS_clicked(bool checked)
{
    m_SerialPort.setRts(checked);
}
