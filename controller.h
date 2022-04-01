#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include<QLabel>
#include<QLineEdit>
#include<QPushButton>
#include<QTableWidget>

#include <Windows.h>
#include "USB_Class.h"
#include "drv/ftd2xx.h"


#include "tools.h"
#include<QDebug>
#include<QSettings>
#include<QFileDialog>

namespace Ui {
class Controller;
}

class Controller : public QWidget
{
    Q_OBJECT

public:
    explicit Controller(QWidget *parent = nullptr);
    ~Controller();

    // = = = USB Connection = = = //
    bool USB_DeviceStatus;
    USB_Class* USB_Device;

    // = = = Общее = = = //
    QLineEdit* numWriteLines;
    QLineEdit* numReadLines;
    CommandList* WriteList;
    CommandList* ReadList;
    ConnectionsBar* Connections_Bar;
    QPushButton* openDevice_pb;
    void createWList();
    void createRList();
    void openDevice();
    void closeDevice();
    void closeEvent(QCloseEvent *event);


    // = = = Отправка = = = //
    void writeSlot();
    void readSlot();



private slots:
    void on_ClearWRList_pb_clicked();
    void on_SaveWRList_pb_clicked();
    void on_LoadWRList_pb_clicked();

private:
    Ui::Controller *ui;
};

#endif // CONTROLLER_H
