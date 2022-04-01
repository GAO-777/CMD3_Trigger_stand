#include "controller.h"
#include "controller.h"
#include "ui_controller.h"
#include<QMessageBox>
Controller::Controller(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Controller)
{
    ui->setupUi(this);
    connect(ui->CreateWList_pb,&QPushButton::clicked,this,&Controller::createWList);
    connect(ui->CreateRList_pb,&QPushButton::clicked,this,&Controller::createRList);
    connect(ui->SendWL_pb,&QPushButton::clicked,this,&Controller::writeSlot);
    connect(ui->SendRL_pb,&QPushButton::clicked,this,&Controller::readSlot);


    WriteList = new CommandList();
    WriteList->Name = "Write_List";
    ReadList = new CommandList();
    ReadList->Name = "Read_List";

    ui->MGL->addWidget(WriteList,0,0);
    ui->MGL->addWidget(ReadList,0,1);


    /* = = = Создает виджет для ConnectionsBar = = = */
    Connections_Bar = new ConnectionsBar(this);
    openDevice_pb = new QPushButton("Подлючиться");
    connect(openDevice_pb,&QPushButton::clicked,this,&Controller::openDevice);
    openDevice_pb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->StatusLayout->addWidget(Connections_Bar,0,Qt::AlignLeft);
    ui->StatusLayout->addWidget(openDevice_pb,Qt::AlignLeft);
    ui->StatusLayout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed));


// = = = = = = = = Загрузка начальных данных при запуске = = = = = = = = = = = = = = = //
    QSettings settings("BINP","Terminal");
    Connections_Bar->USB_SN_l->setText(settings.value("USB/SN","NULL").toString());




// = = = = = = = = Определение переменных в начале работы  = = = = = = = = = = = = = = = //
    USB_DeviceStatus = false;
    USB_Device = new USB_Class();
    ui->SendRL_pb->setEnabled(false);
    ui->SendWL_pb->setEnabled(false);
}

Controller::~Controller()
{
    delete ui;
}

void Controller::createWList()
{
    WriteList->createRow(ui->NumWList_le->text().toInt());
}
void Controller::createRList()
{
    ReadList->createRow(ui->NumRList_le->text().toInt());
}

void Controller::openDevice()
{
    USB_Device->Select_Device(Connections_Bar->USB_SN_l->text().toStdString().c_str());
    USB_DeviceStatus = USB_Device->Open_Device();
    if(USB_DeviceStatus == true){
        Connections_Bar->setUSBstatus(USB_DeviceStatus);
        Connections_Bar->SettingsConnection_pb->setEnabled(false);
        openDevice_pb->setText("Отключиться");
        ui->SendRL_pb->setEnabled(true);
        ui->SendWL_pb->setEnabled(true);
        disconnect(openDevice_pb,&QPushButton::clicked,this,&Controller::openDevice);
        connect(openDevice_pb,&QPushButton::clicked,this,&Controller::closeDevice);
    }else {
        QMessageBox::critical(this,
                                     ("Ошибка"),
                                     ("Произошла ошибка при подлючении."));
    }
}

void Controller::closeDevice()
{
    USB_DeviceStatus = !USB_Device->Close_Device();
    if(USB_DeviceStatus == false){
        Connections_Bar->setUSBstatus(USB_DeviceStatus);
        Connections_Bar->SettingsConnection_pb->setEnabled(true);
        openDevice_pb->setText("Подлючиться");
        ui->SendRL_pb->setEnabled(false);
        ui->SendWL_pb->setEnabled(false);
        disconnect(openDevice_pb,&QPushButton::clicked,this,&Controller::closeDevice);
        connect(openDevice_pb,&QPushButton::clicked,this,&Controller::openDevice);
    }else {
        QMessageBox::critical(this,
                                     ("Ошибка"),
                                     ("Произошла ошибка при отключении устройства."));
    }

}

void Controller::closeEvent(QCloseEvent *event)
{
    QSettings settings("BINP","Terminal");
    settings.setValue("USB/SN",Connections_Bar->USB_SN_l->text());
    settings.sync();
}

void Controller::writeSlot()
{
    WriteList->updateQLists();  // Обновляем листы
    int sizeofList = static_cast<int>(WriteList->Items->size()); // Берем из списка кол-во зачеканых строк
    qDebug()<<" = = = Controller TAB = = = ";
    qDebug()<<"Size of List:"<<sizeofList;
    for(int i=0;i<sizeofList;i++){
        qDebug()<<QString( QString("Write: [%1]<=[%2]")
                           .arg(WriteList->Addr->at(i)).arg(WriteList->Data->at(i)));
    }
    USB_Write_Data(USB_Device, QListToWord(WriteList->Addr),sizeofList, QListToWord(WriteList->Data));
    qDebug()<<" = = = = = = = = = = = = = ";
}

void Controller::readSlot()
{
    ReadList->updateQLists();  // Обновляем листы
    int sizeofList = static_cast<int>(ReadList->Items->size()); // Берем из списка кол-во зачеканых строк
    unsigned int* InData = new unsigned int[sizeofList]; // Массив прочитанных данных

    qDebug()<<" = = = Controller TAB = = = ";
    qDebug()<<"Size of List:"<<sizeofList;
    for(int i=0;i<sizeofList;i++){
        qDebug()<<QString( QString("Read: [%1]")
                           .arg(ReadList->Addr->at(i)));
    }
    USB_Read_Data(USB_Device, QListToWord(ReadList->Addr),sizeofList,InData);
    ReadList->fill_Data(ReadList->Items,InData);
    qDebug()<<" = = = = = = = = = = = = = ";

    /*
    unsigned int* InData = new unsigned int[ReadList->size];
    ReadList->updateQLists();
    QList<unsigned int>* ItemsCheck = ReadList->Items;
    //QList<unsigned int>* Addr = ReadList->getListAddr();
    QList<unsigned int>* Data;
    USB_Read_Data(USB_Device, QListToWord(Addr),(int) ReadList->size, InData);
    ReadList->fillTable(ItemsCheck,InData);
*/
}

void Controller::on_ClearWRList_pb_clicked()
{
    WriteList->clearList();
    ReadList->clearList();
}

void Controller::on_SaveWRList_pb_clicked()
{
   QString saveFileName = QFileDialog::getSaveFileName(this,
                                                "Save file",
                                                QDir::currentPath(),
                                                "INI files (*.ini);;All files (*.*)");

    WriteList->save(saveFileName);
    ReadList->save(saveFileName);
}

void Controller::on_LoadWRList_pb_clicked()
{
    QString openFileName = QFileDialog::getOpenFileName(this,
                                                        "Open file",
                                                        QDir::currentPath(),
                                                        "INI files (*.ini);;All files (*.*)");
   WriteList->load(openFileName);
   ReadList->load(openFileName);

}
