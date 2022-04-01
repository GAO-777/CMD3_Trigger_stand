#include "db_control.h"
#include "ui_db_control.h"

DB_Control::DB_Control(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DB_Control)
{
    ui->setupUi(this);


    MCHS = new MCHS_Imitator();
    ADIS = new class ADIS();


    /* = = = Создает виджет для ConnectionsBar = = = */
    Connections_Bar = new ConnectionsBar(this);
    openDevice_pb = new QPushButton("Подлючиться");
    connect(openDevice_pb,&QPushButton::clicked,this,&DB_Control::openDevice);
    openDevice_pb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->StatusLayout->addWidget(Connections_Bar,0,Qt::AlignLeft);
    ui->StatusLayout->addWidget(openDevice_pb,Qt::AlignLeft);
    ui->StatusLayout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed));

    // - - - Control Page - - - //
    WriteList = new CommandList();
    WriteList->Name = "Write_List";
    ReadList = new CommandList();
    ReadList->Name = "Read_List";
    ui->CommandLists_hl->addWidget(WriteList);
    ui->CommandLists_hl->addWidget(ReadList);

    // - - - Control Page - - - //
    QStringList *CF_Masks = new QStringList();
    *CF_Masks<<"OrBGO"<<"OrLXe"<<"OrCsI"<<"OneCluster"<<"TwoCluster"<<"ThreeAndMoreCluster"<<"NTN"<<"Z_Asymmetry";
    *CF_Masks<<"nZ_Asymmetry"<<"EthrBarrel[0]"<<"EthrBarrel[1]"<<"EthrBarrel[2]"<<"EthrCalorimeter[0]"<<"EthrCalorimeter[1]";
    *CF_Masks<<"EthrCalorimeter[2]"<<"EthrCalorimeter[3]"<<"GapSizeLower[0]"<<"GapSizeLower[1]"<<"GapSizeUpper[0]"<<"GapSizeUpper[1]";
    *CF_Masks<<"ClusterSizeLower"<<"ClusterSizeUpper"<<"BGO_Bhabha_external"<<"BGO_Bhabha_intermediate"<<"BGO_Bhabha_internal"<<"BGO_Cosmic";

    for(int i=0; i<26; i++){
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(CF_Masks->at(i));
        item->setCheckState(Qt::Unchecked);
        ui->CF_Masks_lw->addItem(item);
    }

    // - - - Начальные параметры - - - //
    ui->CTRL_SetCTRLReg_pb->setEnabled(false);
    ui->CTRL_ReadErrors_pb->setEnabled(false);
    ui->CTRL_Write_pb->setEnabled(false);
    ui->CTRL_Read_pb->setEnabled(false);

    *CF_RAM_0<<"OrBGO"<<"OrLXe"<<"OrCsI"<<"OneCluster"<<"TwoCluster"<<"ThreeAndMoreCluster"<<"AllTriggered"<<"NoOneTriggered";
    *CF_RAM_0<<"NTN_Right"<<"NTN_Left"<<"NTN"<<"Z_Asymmetry"<<"BGO_Bhabha_external"<<"BGO_Bhabha_intermediate"<<"BGO_Bhabha_internal"<<"BGO_Cosmic";
}

DB_Control::~DB_Control()
{
    delete ui;
}

void DB_Control::openDevice()
{
    MCHS->setSN(Connections_Bar->USB_SN_l->text());
    MCHS->openUSB();
    qDebug()<< MCHS->USB_DeviceStatus;
    MCHS->GeneratorsOff();

    if(MCHS->USB_DeviceStatus == true){
        Connections_Bar->setUSBstatus(MCHS->USB_DeviceStatus);
        Connections_Bar->SettingsConnection_pb->setEnabled(false);
        openDevice_pb->setText("Отключиться");
        disconnect(openDevice_pb,&QPushButton::clicked,this,&DB_Control::openDevice);
        connect(openDevice_pb,&QPushButton::clicked,this,&DB_Control::closeDevice);
        ui->CTRL_SetCTRLReg_pb->setEnabled(true);
        ui->CTRL_ReadErrors_pb->setEnabled(true);
        ui->CTRL_Write_pb->setEnabled(true);
        ui->CTRL_Read_pb->setEnabled(true);
    }else {
        QMessageBox::critical(this,
                              ("Ошибка"),
                              ("Произошла ошибка при подлючении."));
    }
}

void DB_Control::closeDevice()
{
    MCHS->closeUSB();
    if(MCHS->USB_DeviceStatus == false){
        Connections_Bar->setUSBstatus(MCHS->USB_DeviceStatus);
        Connections_Bar->SettingsConnection_pb->setEnabled(true);
        openDevice_pb->setText("Подлючиться");
        disconnect(openDevice_pb,&QPushButton::clicked,this,&DB_Control::closeDevice);
        connect(openDevice_pb,&QPushButton::clicked,this,&DB_Control::openDevice);
        ui->CTRL_SetCTRLReg_pb->setEnabled(false);
        ui->CTRL_ReadErrors_pb->setEnabled(false);
        ui->CTRL_Write_pb->setEnabled(false);
        ui->CTRL_Read_pb->setEnabled(false);
    }else {
        QMessageBox::critical(this,
                              ("Ошибка"),
                              ("Произошла ошибка при отключении устройства."));
    }

}

bool DB_Control::rawDataToRAM(QList<unsigned short> *positional, QList<unsigned short> *energy,
                              QList<unsigned short> *addr, QList<unsigned short> *data, QString who)
{
/*
    Фунция раскладывает позц. и энерг. данные по нужным адресам памяти InData_TestRAM.

*/

    for (int i=0;i<energy->size();i++){
        if(positional->at(i)>65535){
            QMessageBox::critical(this,("Ошибка"),QString("%1 - Позиционные данные превышают 16 бит").arg(i));
            return false;
        }
        if(energy->at(i)>255){
            QMessageBox::critical(this,("Ошибка"),QString("%1 - Энергетические данные превышают 8 бит").arg(i));
            return false;
        }
    }

    unsigned int startAddr = 0;
    if(who == "BGO") startAddr = TestRAM_BGO_offset;
    if(who == "LXe") startAddr = TestRAM_LXe_offset;
    if(who == "CsI") startAddr = TestRAM_CsI_offset;
    int p = 0,e = 0;
    for(int i=0;e<energy->size();i++){
        if(i%3 == 0){
            addr->append(startAddr + i);
            data->append(positional->at(p));
            p++;
        }
        else if(i%3 == 1 & i!=7){
            addr->append(startAddr +i);
            unsigned int newData = ((positional->at(p)<<8)&0xFF00) | energy->at(e);
            data->append(newData);
            e++;
        }
        else if(i%3 == 2){
            addr->append(startAddr +i);
            unsigned int newData = (( energy->at(e)<<8)&0xFF00) | ((positional->at(p)>>8)&0xFF);
            data->append(newData);
            e++;
            p++;
        }
        if(i==7){
            addr->append(startAddr +i);
            unsigned int newData = energy->at(e);
            data->append(newData);
            e++;
        }
    }
    return true;
}


void DB_Control::on_TR_Clear_clicked()
{
    ui->TR_BGO_1->clear();
    ui->TR_BGO_2->clear();
    ui->TR_BGO_3->clear();
    ui->TR_BGO_4->clear();
    ui->TR_BGO_E1->clear();
    ui->TR_BGO_E2->clear();
    ui->TR_BGO_E3->clear();
    ui->TR_BGO_E4->clear();

    ui->TR_LXe_1->clear();
    ui->TR_LXe_2->clear();
    ui->TR_LXe_3->clear();
    ui->TR_LXe_4->clear();
    ui->TR_LXe_5->clear();
    ui->TR_LXe_E1->clear();
    ui->TR_LXe_E2->clear();
    ui->TR_LXe_E3->clear();
    ui->TR_LXe_E4->clear();
    ui->TR_LXe_E5->clear();

    ui->TR_CsI_1->clear();
    ui->TR_CsI_2->clear();
    ui->TR_CsI_3->clear();
    ui->TR_CsI_4->clear();
    ui->TR_CsI_5->clear();
    ui->TR_CsI_E1->clear();
    ui->TR_CsI_E2->clear();
    ui->TR_CsI_E3->clear();
    ui->TR_CsI_E4->clear();
    ui->TR_CsI_E5->clear();
}

void DB_Control::on_TR_Send_clicked()
{
    QList<unsigned short> *BGO_pos = new  QList<unsigned short>();
    QList<unsigned short> *BGO_energy = new  QList<unsigned short>();
    QList<unsigned short> *BGO_addr = new  QList<unsigned short>();
    QList<unsigned short> *BGO_data = new  QList<unsigned short>();

    QList<unsigned short> *LXe_pos = new  QList<unsigned short>();
    QList<unsigned short> *LXe_energy = new  QList<unsigned short>();
    QList<unsigned short> *LXe_addr = new  QList<unsigned short>();
    QList<unsigned short> *LXe_data = new  QList<unsigned short>();

    QList<unsigned short> *CsI_pos = new  QList<unsigned short>();
    QList<unsigned short> *CsI_energy = new  QList<unsigned short>();
    QList<unsigned short> *CsI_addr = new  QList<unsigned short>();
    QList<unsigned short> *CsI_data = new  QList<unsigned short>();

    BGO_pos->append(ui->TR_BGO_1->text().toInt());
    BGO_pos->append(ui->TR_BGO_2->text().toInt());
    BGO_pos->append(ui->TR_BGO_3->text().toInt());
    BGO_pos->append(ui->TR_BGO_4->text().toInt());
    BGO_energy->append(ui->TR_BGO_E1->text().toInt());
    BGO_energy->append(ui->TR_BGO_E2->text().toInt());
    BGO_energy->append(ui->TR_BGO_E3->text().toInt());
    BGO_energy->append(ui->TR_BGO_E4->text().toInt());

    LXe_pos->append(ui->TR_LXe_1->text().toInt());
    LXe_pos->append(ui->TR_LXe_2->text().toInt());
    LXe_pos->append(ui->TR_LXe_3->text().toInt());
    LXe_pos->append(ui->TR_LXe_4->text().toInt());
    LXe_pos->append(ui->TR_LXe_5->text().toInt());
    LXe_energy->append(ui->TR_LXe_E1->text().toInt());
    LXe_energy->append(ui->TR_LXe_E2->text().toInt());
    LXe_energy->append(ui->TR_LXe_E3->text().toInt());
    LXe_energy->append(ui->TR_LXe_E4->text().toInt());
    LXe_energy->append(ui->TR_LXe_E5->text().toInt());

    CsI_pos->append(ui->TR_CsI_1->text().toInt());
    CsI_pos->append(ui->TR_CsI_2->text().toInt());
    CsI_pos->append(ui->TR_CsI_3->text().toInt());
    CsI_pos->append(ui->TR_CsI_4->text().toInt());
    CsI_pos->append(ui->TR_CsI_5->text().toInt());
    CsI_energy->append(ui->TR_CsI_E1->text().toInt());
    CsI_energy->append(ui->TR_CsI_E2->text().toInt());
    CsI_energy->append(ui->TR_CsI_E3->text().toInt());
    CsI_energy->append(ui->TR_CsI_E4->text().toInt());
    CsI_energy->append(ui->TR_CsI_E5->text().toInt());

    rawDataToRAM(BGO_pos,BGO_energy,BGO_addr,BGO_data,"BGO");
    rawDataToRAM(LXe_pos,LXe_energy,LXe_addr,LXe_data,"LXe");
    rawDataToRAM(CsI_pos,CsI_energy,CsI_addr,CsI_data,"CsI");

    MCHS->SendWrite154(ui->Num_Down_Link->value(),6,BGO_addr,BGO_data);
    MCHS->SendWrite154(ui->Num_Down_Link->value(),8,LXe_addr,LXe_data);
    MCHS->SendWrite154(ui->Num_Down_Link->value(),8,CsI_addr,CsI_data);

    delete BGO_pos;
    delete BGO_energy;
    delete BGO_addr;
    delete BGO_data;
    delete LXe_pos;
    delete LXe_energy;
    delete LXe_addr;
    delete LXe_data;
    delete CsI_pos;
    delete CsI_energy;
    delete CsI_addr;
    delete CsI_data;
}

void DB_Control::on_CTRL_SetCTRLReg_pb_clicked()
{
    QList<unsigned short> *addr = new  QList<unsigned short>();
    QList<unsigned short> *data = new  QList<unsigned short>();
    bitset<16> ControlReg; // Регистр управления

    ControlReg.set(0,ui->CTRL_BlockTF_cb->isChecked());
    ControlReg.set(1,ui->CTRL_BlockCF_cb->isChecked());
    ControlReg.set(2,ui->CTRL_BlockMT_cb->isChecked());
    ControlReg.set(3,ui->CTRL_UseTestRAM_cb->isChecked());
    ControlReg.set(4,ui->CTRL_StartOne_cb->isChecked());
    // Формируем массив адресов и данных для записи в блок
    // - - - ControlReg - - - //
    addr->append(ControlReg_offset);
    data->append(static_cast<unsigned short>(ControlReg.to_ulong()));
    // - - - StartOneWidth - - - //
    addr->append(Start_One_Width_Offset);
    data->append(static_cast<unsigned short>(ui->CTRL_StartOneWidth_sb->value()));

    MCHS->SendWrite154(ui->Num_Down_Link->value(),2,addr,data);
    delete addr;
    delete data;
}

void DB_Control::on_CTRL_ReadErrors_pb_clicked()
{
    QList<unsigned short> *addr = new  QList<unsigned short>();
    QList<unsigned short> *data = new  QList<unsigned short>();
    addr->append(CTR_ERROR_CPU);
    addr->append(CTR_ERROR_Link);
    addr->append(CTR_ERROR_Arbiter);
    addr->append(CTR_RESET);
    MCHS->SendRead145(ui->Num_Down_Link->value(),4,addr,data);
    ui->CTRL_CPU_Error_cb->setValue(data->at(0));
    ui->CTRL_Link_Error_cb->setValue(data->at(1));
    ui->CTRL_Arbiter_Error_cb->setValue(data->at(2));
    ui->CTRL_CTR_Reset_Error_cb->setValue(data->at(3));
    for(int i=0;i<4;i++){
        qDebug()<<data->at(i);

    }
    delete addr;
    delete data;
}

void DB_Control::on_CTRL_CreateWList_pb_clicked()
{
     WriteList->createRow(ui->CTRL_NumWL_sb->value());
}

void DB_Control::on_CTRL_CreateRList_pb_clicked()
{
    ReadList->createRow(ui->CTRL_NumRL_sb->value());
}

void DB_Control::on_CTRL_Write_pb_clicked()
{
    WriteList->updateQLists();  // Обновляем листы
    int sizeofList = static_cast<int>(WriteList->Items->size()); // Берем из списка кол-во зачеканых строк
    QList<unsigned short>* addr = new  QList<unsigned short>();
    QList<unsigned short>* data = new  QList<unsigned short>();
    QList<unsigned int>::Iterator i;    // для addr
    QList<unsigned int>::Iterator p;    // для data
    for(i = WriteList->Addr->begin(); i != WriteList->Addr->end(); i++){
        addr->append(*i);
    }
    for(p = WriteList->Data->begin(); p != WriteList->Data->end(); p++){
        data->append(*p);
    }
    MCHS->SendWrite154(ui->Num_Down_Link->value(),sizeofList,addr,data);
    delete addr;
    delete data;
}

void DB_Control::on_CTRL_Read_pb_clicked()
{
    ReadList->updateQLists();  // Обновляем листы
    int sizeofList = static_cast<int>(ReadList->Items->size()); // Берем из списка кол-во зачеканых строк
    QList<unsigned short>* addr = new  QList<unsigned short>();
    QList<unsigned short>* data = new  QList<unsigned short>();
    QList<unsigned int>* Indata = new  QList<unsigned int>();
    QList<unsigned int>::Iterator i;    // для addr
    QList<unsigned short>::Iterator p;    // для data
    for(i = ReadList->Addr->begin(); i != ReadList->Addr->end(); i++){
        addr->append(*i);
    }
    MCHS->SendRead145(ui->Num_Down_Link->value(),sizeofList,addr,data);
    for(p = data->begin(); p != data->end(); p++){
        Indata->append(*p);
    }

    ReadList->fill_Data(ReadList->Items,Indata);
    qDebug()<<" = = = CTRL TAB = = = ";
    qDebug()<<"Size of List:"<<sizeofList;
    for(int i=0;i<sizeofList;i++){
        qDebug()<<QString( QString("Read: [%1]")
                           .arg(data->at(i)));
    }
}

void DB_Control::on_CTRL_SaveList_pb_clicked()
{
    QString saveFileName = QFileDialog::getSaveFileName(this,
                                                "Save file",
                                                QDir::currentPath(),
                                                "INI files (*.ini);;All files (*.*)");
    WriteList->save(saveFileName);
    ReadList->save(saveFileName);
}

void DB_Control::on_CTRL_LoadList_pb_clicked()
{
    QString openFileName = QFileDialog::getOpenFileName(this,
                                                        "Open file",
                                                        QDir::currentPath(),
                                                        "INI files (*.ini);;All files (*.*)");
   WriteList->load(openFileName);
   ReadList->load(openFileName);
}

void DB_Control::on_CTRL_Clear_pb_clicked()
{
    WriteList->clearList();
    ReadList->clearList();
}

void DB_Control::on_CTRL_Start_One_pb_clicked()
{
    QList<unsigned short> *addr = new  QList<unsigned short>();
    QList<unsigned short> *data = new  QList<unsigned short>();

    addr->append(Start_One_Offset);
    data->append(0);

    MCHS->SendWrite154(ui->Num_Down_Link->value(),1,addr,data);
    delete addr;
    delete data;

}

void DB_Control::on_CF_Masks_lw_itemClicked(QListWidgetItem *item)
{
    if(item->checkState() == Qt::Checked)
    {
        CF_Masks[ui->CF_Mask_NUM_sb->value()-1] = CF_Masks[ui->CF_Mask_NUM_sb->value()-1] | (0x1<<ui->CF_Masks_lw->row(item));
        qDebug()<<CF_Masks[ui->CF_Mask_NUM_sb->value()-1];
    }
    if(item->checkState() == Qt::Unchecked)
    {
        CF_Masks[ui->CF_Mask_NUM_sb->value()-1] = CF_Masks[ui->CF_Mask_NUM_sb->value()-1] & ~(0x1<<ui->CF_Masks_lw->row(item));
        //qDebug()<<CF_Masks[ui->CF_Mask_NUM_sb->value()-1];
    }
}

void DB_Control::on_CF_Mask_NUM_sb_valueChanged(int arg1)
{
    for(int i=0; i<26; i++){
        QListWidgetItem *item = new QListWidgetItem;
        item = ui->CF_Masks_lw->item(i);
        item->setCheckState(Qt::Unchecked);
    }
    for(int i=0; i<8; i++){
        CF_Masks[i] = 0;
    }
}

void DB_Control::on_Send_CF_Masks_pb_clicked()
{
    QList<unsigned short> *addr = new  QList<unsigned short>();
    QList<unsigned short> *data = new  QList<unsigned short>();
    unsigned int CF_Mask = CF_Masks[ui->CF_Mask_NUM_sb->value()-1];
    unsigned int CF_Mask_p1 = CF_Mask & 0xFFFF;
    unsigned int CF_Mask_p2 = (CF_Mask>>16) & 0xFFFF;
    //qDebug()<<"CF_Mask_p1 = "<<CF_Mask_p1;
    //qDebug()<<"CF_Mask_p2 = "<<CF_Mask_p2;
    unsigned int Mask_addr = 0;
    switch (ui->CF_Mask_NUM_sb->value())
    {
        case 1: Mask_addr= CF_Mask_1_offset;
            break;
        case 2: Mask_addr= CF_Mask_2_offset;
            break;
        case 3: Mask_addr= CF_Mask_3_offset;
            break;
        case 4: Mask_addr= CF_Mask_4_offset;
            break;
        case 5: Mask_addr= CF_Mask_5_offset;
            break;
        case 6: Mask_addr= CF_Mask_6_offset;
            break;
        case 7: Mask_addr= CF_Mask_7_offset;
            break;
        case 8: Mask_addr= CF_Mask_8_offset;
            break;
    }
    addr->append(Mask_addr);
    data->append(CF_Mask_p1);
    addr->append(Mask_addr+1);
    data->append(CF_Mask_p2);
    MCHS->SendWrite154(ui->Num_Down_Link->value(),2,addr,data);
}

void DB_Control::on_IMChS_waitingNIM_clicked()
{
    MCHS->Init_Waiting193(ui->Num_Down_Link->value());
}

void DB_Control::on_Read_193_clicked()
{
    QList<unsigned short> *Data = new  QList<unsigned short>();
    MCHS->ReadDataFromDownLink(ui->Num_Down_Link->value(),Data);
    ui->Console->insertPlainText("-----------------------------------------\n");
    for(int i=0;i<64;i++){
        if(i%5 == 0 & i!=0)  ui->Console->insertPlainText("\n");
        QString str = QString("%1 | ").arg(Data->at(i),5);
        ui->Console->insertPlainText(str);
    }
    ui->Console->insertPlainText("\n");
}

void DB_Control::on_Console_clear_pb_clicked()
{
    ui->Console->clear();
}

void DB_Control::on_Unpack_RAM_sb_valueChanged(int arg1)
{
    unsigned int inputRAM = arg1;
    ui->Unpack_RAM_lw->clear();

    if (ui->Unpack_RAM_TF->isChecked()){

    }
    if (ui->Unpack_RAM_CF->isChecked()){
        switch (ui->Unpack_RAM_NUM->value())
        {
        case 0:
            for(int i=0;i<15;i++){
                if( (inputRAM>>i)&0x1 ){
                    QListWidgetItem *item = new QListWidgetItem;
                    item->setText(CF_RAM_0->at(i));
                    ui->Unpack_RAM_lw->addItem(item);
                }
            }
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        }
    }

}

void DB_Control::on_Set_DataFL_clicked()
{
        ADIS->setData_FL(MCHS,2,ui->spinBox->value());

}

void DB_Control::on_TestEth_clicked()
{
    QList<unsigned int>* Addr = new QList<unsigned int>();
    QList<unsigned int>* Data = new QList<unsigned int>();

    Addr->append(3073);

    WORD* OutData  = new WORD[3];
    OutData[0] = 505;

    Ethernet_Interface Eth;
    Eth.init("192.168.0.2",49320);

    Eth.write(QListToWord(Addr),OutData,1);
//    Eth.read(QListToWord(Addr),OutData,1);

    Eth.closeSocket();

    qDebug()<<OutData[0];
}
