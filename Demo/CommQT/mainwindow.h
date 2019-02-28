#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SerialPort.h"

using namespace std;
using namespace itas109;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public has_slots<>
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void OnReceive();

    void on_pushButtonOpen_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonClear_clicked();

private:
    Ui::MainWindow *ui;

    CSerialPort m_SerialPort;

    int tx;
    int rx;
};

#endif // MAINWINDOW_H
