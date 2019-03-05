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

    //ui
    ui->pushButtonOpen->setText(tr("open"));

    rx = 0;
    tx = 0;

    //init

    std::list<std::string> portNameList = CSerialPortInfo::availablePorts();

    list<string>::iterator itor;

    int i = 0;
    for (itor = portNameList.begin(); itor != portNameList.end(); ++itor)
    {
        ui->comboBoxPortName->insertItem(i,QString::fromStdString(*itor));
        i++;
    }

    m_SerialPort.readReady.connect(this, &MainWindow::OnReceive);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnReceive()
{
    char * str = NULL;
    str = new char[256];
    m_SerialPort.readAllData(str);

    QString m_str = QString::fromLocal8Bit(str);

    qDebug() << "receive : " << m_str;

    //追加文本（plainTextEditReceive是一个QPlainTextEdit对象）
    auto workCursor = ui->plainTextEditReceive->textCursor();
    workCursor.movePosition(QTextCursor::End);
    workCursor.insertText(m_str);
    workCursor.insertBlock();

    //移动滚动条到底部
    QScrollBar *scrollbar = ui->plainTextEditReceive->verticalScrollBar();
    if (scrollbar)
    {
        scrollbar->setSliderPosition(scrollbar->maximum());
    }

    rx += m_str.length();
    ui->labelRXValue->setText(QString::number(rx));
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

        m_SerialPort.writeData(sendStr.toStdString().c_str(),sendStr.length());

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
    tx = 0;

    ui->labelTXValue->setText(QString::number(tx));
    ui->labelRXValue->setText(QString::number(rx));
}
