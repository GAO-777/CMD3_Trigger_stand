#include "tools.h"
#include<QHeaderView>

/*===============================================================================================*\
  ███████████████████████████████████████████████████████████████████████████████████████████████
  ███████████████████████████───█─█─█─██─█────█───█───█────█─██─█───█████████████████████████████
  ███████████████████████████─███─█─█──█─█─██─██─███─██─██─█──█─█─███████████████████████████████
  ███████████████████████████───█─█─█─█──█─█████─███─██─██─█─█──█───█████████████████████████████
  ███████████████████████████─███─█─█─██─█─██─██─███─██─██─█─██─███─█████████████████████████████
  ███████████████████████████─███───█─██─█────██─██───█────█─██─█───█████████████████████████████
  ███████████████████████████████████████████████████████████████████████████████████████████████
\*===============================================================================================*/
WORD* QListToWord(QList<unsigned int> *InData)
{
    WORD* OutData  = new WORD[InData->size()];
    QList<unsigned int>::iterator i;
    int j=0;
    for (i = InData->begin(); i != InData->end(); ++i){
        OutData[j] = *i;
        j++;
    }
    return OutData;
}

QList<unsigned int> WordToQList(WORD* InData, int size)
{
    QList<unsigned int> OutData = QList<unsigned int>();
    for (int i=0; i < size; ++i)
        OutData.append(InData[i]);
    return OutData;
}

/*===============================================================================================*\
  ███████████████████████████████████████████████████████████████████████████████████
  ████████████────█────█─███─█─███─█────█─██─█────██─███───█───█───██████████████████
  ████████████─██─█─██─█──█──█──█──█─██─█──█─█─██──█─████─██─████─███████████████████
  ████████████─████─██─█─█─█─█─█─█─█────█─█──█─██──█─████─██───██─███████████████████
  ████████████─██─█─██─█─███─█─███─█─██─█─██─█─██──█─████─████─██─███████████████████
  ████████████────█────█─███─█─███─█─██─█─██─█────██───█───█───██─███████████████████
  ███████████████████████████████████████████████████████████████████████████████████
\*===============================================================================================*/
CommandList::CommandList(QTableWidget *parent) :
    QTableWidget(parent)
{
    Name = "";
    Items = new QList<unsigned int>();
    Addr = new QList<unsigned int>();
    Data = new QList<unsigned int>();
    // = = = Выполняем заполнение QTableWidget записями с помощью цикла = = = //

    setColumnCount(3);   // Указываем число колонок
    setShowGrid(true);   // Включаем сетку
    setHorizontalHeaderLabels(QStringList() << "" << " Address"<<"Data" );
    resizeColumnToContents(0);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    resizeColumnToContents(0);

    QFont font = horizontalHeader()->font();
    font.setBold (true);
    horizontalHeader()->setFont(font);
}

void CommandList::createRow(int s)
{
    // = = = Выполняем заполнение QTableWidget записями с помощью цикла = = = //
    for(int i = size; i<s; i++){
        // Вставляем строку
        insertRow(i);
        // Создаём виджет, который будет содержать в себе чекбокс
        QWidget *checkBoxWidget = new QWidget();
        QCheckBox *checkBox = new QCheckBox();      // объявляем и инициализируем чекбокс
        QHBoxLayout *layoutCheckBox = new QHBoxLayout(checkBoxWidget); // создаём слой с привязкой к виджету
        layoutCheckBox->addWidget(checkBox);            // Устанавливаем чекбокс в слой
        layoutCheckBox->setAlignment(Qt::AlignCenter);  // Отцентровываем чекбокс
        layoutCheckBox->setContentsMargins(0,0,0,0);    // Устанавливаем нулевые отступы

        checkBox->setChecked(true);

        setCellWidget(i,0, checkBoxWidget);
        // Далее забираем все данные из результата запроса и устанавливаем в остальные поля
        setItem(i,1, new QTableWidgetItem("0"));
        setItem(i,2, new QTableWidgetItem("0"));
        setRowHeight(i,25);
        setColumnWidth(i,60);

    }
    resizeColumnToContents(0);

    size = s;
}

void CommandList::updateQLists()
{
    Items->clear();
    Addr->clear();
    Data->clear();

    for(int i = 0; i < size; i++){
        // Забираем виджет из слоя и кастуем его в QCheckBox
        QWidget *item = (cellWidget(i,0));
        QCheckBox *checkB = qobject_cast <QCheckBox*> (item->layout()->itemAt(0)->widget());
        QTableWidgetItem* lineAddr = this->item(i, 1);
        QTableWidgetItem* lineData = this->item(i, 2);
        if(checkB->isChecked())
            if(lineAddr->text() != ""){
                Items->append(static_cast<unsigned int>(i));
                Addr->append(static_cast<unsigned int>(lineAddr->text().toInt()));
                Data->append(static_cast<unsigned int>(lineData->text().toInt()));
            }
    }
}

void CommandList::fill_Data(QList<unsigned int> *Items,unsigned int* InData)
{
    QList<unsigned int>::iterator j;

    int i =0;
    for (j = Items->begin(); j != Items->end(); ++j){
         QTableWidgetItem* lineData = this->item(*j, 2);
         lineData->setText(QString::number(InData[i]));
         i++;
    }
}

void CommandList::fill_Data(QList<unsigned int> *Items, QList<unsigned int> *InData)
{
    QList<unsigned int>::iterator j;
    int i =0;
    for (j = Items->begin(); j != Items->end(); ++j){
         QTableWidgetItem* lineData = this->item(*j, 2);
         lineData->setText(QString::number(InData->at(i)));
         i++;
    }
}

void CommandList::getDataToSave()
{
    Items->clear();
    Addr->clear();
    Data->clear();
    for(int i = 0; i < size; i++){
        // Забираем виджет из слоя и кастуем его в QCheckBox
        QWidget *item = (cellWidget(i,0));
        QCheckBox *checkB = qobject_cast <QCheckBox*> (item->layout()->itemAt(0)->widget());
        QTableWidgetItem* lineAddr = this->item(i, 1);
        QTableWidgetItem* lineData = this->item(i, 2);
        Items->append(checkB->isChecked());
        Addr->append(lineAddr->text().toInt());
        Data->append(lineData->text().toInt());
    }

}

void CommandList::setDataToLoad(int size,QList<unsigned int>* Items,QList<unsigned int>* Addr,QList<unsigned int>* Data)
{
    clearList();
    createRow(size);
    for (int i=0;i<size;i++){
        QWidget *item = (cellWidget(i,0));
        QCheckBox *checkB = qobject_cast <QCheckBox*> (item->layout()->itemAt(0)->widget());
        QTableWidgetItem* lineAddr = this->item(i, 1);
        QTableWidgetItem* lineData = this->item(i, 2);
        lineAddr->setText(QString::number(Addr->at(i)));
        lineData->setText(QString::number(Data->at(i)));
        if(Items->at(i) == 1) checkB->setCheckState(Qt::Checked);
        else checkB->setCheckState(Qt::Unchecked);
    }
}

void CommandList::clearList()
{
    for(int i = 0; i < size; i++){
        // Забираем виджет из слоя и кастуем его в QCheckBox
        QWidget *item = (cellWidget(i,0));
        QCheckBox *checkB = qobject_cast <QCheckBox*> (item->layout()->itemAt(0)->widget());
        QTableWidgetItem* lineAddr = this->item(i, 1);
        QTableWidgetItem* lineData = this->item(i, 2);
        checkB->setChecked(true);
        lineAddr->setText("0");
        lineData->setText("0");
    }
}

void CommandList::save(QString saveFileName)
{
    QSettings* List_Save = new QSettings(saveFileName,QSettings::IniFormat);
    // Сохраняем данные QList о состоянии ячеек
    getDataToSave();

    // = = = = = Сохраняем лист  = = = = = //
    List_Save->beginWriteArray(Name);
    for (int i = 0; i < size; ++i) {
        List_Save->setArrayIndex(i);
        List_Save->setValue("Check",Items->at(i));
        List_Save->setValue("Addr", Addr->at(i));
        List_Save->setValue("Data", Data->at(i));
    }
    List_Save->endArray();
    List_Save->sync();
}

void CommandList::load(QString openFileName)
{
    QSettings* List_Save = new QSettings(openFileName,QSettings::IniFormat);
    // = = = = = Загружаем лист  = = = = = //
    QList<unsigned int>* WriteList_Items = new QList<unsigned int>();
    QList<unsigned int>* WriteList_Addr = new QList<unsigned int>();
    QList<unsigned int>* WriteList_Data= new QList<unsigned int>();
    int size_w = List_Save->beginReadArray(Name);
    for (int i = 0; i < size_w; ++i){
        List_Save->setArrayIndex(i);
        WriteList_Items->append(List_Save->value("Check").toInt());
        WriteList_Addr->append(List_Save->value("Addr").toInt());
        WriteList_Data->append(List_Save->value("Data").toInt());
      }
    List_Save->endArray();
    setDataToLoad(size_w,WriteList_Items,WriteList_Addr,WriteList_Data);
    delete WriteList_Items;
    delete WriteList_Addr;
    delete WriteList_Data;
}

/*===============================================================================================*\
  ███████████████████████████████████████████████████████████████████████████████████████████████
  ████████████────█────█─██─█─██─█───█────█───█───█────█─██─█───█───█───█─█─█────████████████████
  ████████████─██─█─██─█──█─█──█─█─███─██─██─███─██─██─█──█─█─███─████─██─█─█─██─████████████████
  ████████████─████─██─█─█──█─█──█───█─█████─███─██─██─█─█──█───█───██─██─█─█────████████████████
  ████████████─██─█─██─█─██─█─██─█─███─██─██─███─██─██─█─██─███─█─████─██─█─█─███████████████████
  ████████████────█────█─██─█─██─█───█────██─██───█────█─██─█───█───██─██───█─███████████████████
  ███████████████████████████████████████████████████████████████████████████████████████████████
\*===============================================================================================*/

#include "ui_connectionsetup.h"

ConnectionsBar *ParentSB;

ConnectionSetup::ConnectionSetup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionSetup)
{
    ui->setupUi(this);
    ParentSB = static_cast<ConnectionsBar*>(parent);
    ParentSB->SettingsConnection_pb->setEnabled(false);
    connect(ui->USB_ConnectionType_rb,&QRadioButton::clicked,this,&ConnectionSetup::PageSwitch);
    connect(ui->UDP_ConnectionType_rb,&QRadioButton::clicked,this,&ConnectionSetup::PageSwitch);

    USB_page = new QWidget(this);
    UDP_page = new QWidget(this);
    ui->MVL_Pages->addWidget(USB_page);
    ui->MVL_Pages->addWidget(UDP_page);
    USB_page->setVisible(false);
    UDP_page->setVisible(false);

    init_Pages();

}

ConnectionSetup::~ConnectionSetup()
{
    delete ui;
}

void ConnectionSetup::PageSwitch()
{
    if(ui->USB_ConnectionType_rb->isChecked()){
        UDP_page->setVisible(false);
        USB_page->setVisible(true);
    }else if(ui->UDP_ConnectionType_rb->isChecked()){
        UDP_page->setVisible(true);
        USB_page->setVisible(false);
    }
}

void ConnectionSetup::UpdateDiviceList()
{
    USB_SN_cb->clear();
    FT_STATUS ftStatus;
    DWORD numDevs;
    // create the device information list
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus == FT_OK) {
        printf("Number of devices is %d\n",numDevs);
    }
    if (numDevs > 0) {
        USB_DevInfo =
                (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
        ftStatus = FT_GetDeviceInfoList(USB_DevInfo,&numDevs);
        if (ftStatus == FT_OK) {
            for (int i = 0; i < static_cast<int>(numDevs); i++) {
                USB_SN_cb->addItem(QString(USB_DevInfo[i].SerialNumber));
            }
        }
    }
    USB_SN_cb->update();
}

void ConnectionSetup::USB_page_ChangedDevice(int i)
{
    USB_Description_l->setText(USB_DevInfo[i].Description);
    USB_ID_l->setText(QString::number(USB_DevInfo[i].ID));
    USB_LocId_l->setText(QString::number(USB_DevInfo[i].LocId));

}

void ConnectionSetup::UpdateSetup()
{
    if(ui->USB_ConnectionType_rb->isChecked()){
        ParentSB->ConnectionType = USB;
        if(USB_SN_cb->currentIndex()>=0){
            ParentSB->USB_SN_l->setText(USB_DevInfo[USB_SN_cb->currentIndex()].SerialNumber);
        }else {
            ParentSB->USB_SN_l->setText("NULL");
        }
    }else{
        ParentSB->ConnectionType = UDP;
        ParentSB->IP_address = IPaddress->text();
        ParentSB->Port = Port->value();
    }
}

void ConnectionSetup::init_Pages()
{
/*==========================================================================================*\
- - - - - - - - - - - - - USB PAGE - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
\*==========================================================================================*/
    QVBoxLayout* MVL_USBpage = new QVBoxLayout();
    QHBoxLayout* HL_Description = new QHBoxLayout();
    QHBoxLayout* HL_ID = new QHBoxLayout();
    QHBoxLayout* HL_LocId = new QHBoxLayout();

    USB_SN_cb = new QComboBox(this);
    connect(USB_SN_cb, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,&ConnectionSetup::USB_page_ChangedDevice);

    USB_Description_l = new QLabel();
    USB_ID_l = new QLabel();
    USB_LocId_l = new QLabel();

    USB_UpdateDeviceList_pb = new QPushButton("Обновить список устройств");
    connect(USB_UpdateDeviceList_pb,&QPushButton::clicked,this,&ConnectionSetup::UpdateDiviceList);

    HL_Description->addWidget(new QLabel("Description: "),0,Qt::AlignLeft);
    HL_Description->addWidget(USB_Description_l,1,Qt::AlignLeft);

    HL_ID->addWidget(new QLabel("ID:          "),0,Qt::AlignLeft);
    HL_ID->addWidget(USB_ID_l,1,Qt::AlignLeft);

    HL_LocId->addWidget(new QLabel("LocID:       "),0,Qt::AlignLeft);
    HL_LocId->addWidget(USB_LocId_l,1,Qt::AlignLeft);

    MVL_USBpage->addWidget(USB_SN_cb);
    MVL_USBpage->insertLayout(1,HL_Description,0);
    MVL_USBpage->insertLayout(2,HL_ID,0);
    MVL_USBpage->insertLayout(3,HL_LocId,0);
    MVL_USBpage->addWidget(USB_UpdateDeviceList_pb);

    USB_page->setLayout(MVL_USBpage);
    UpdateDiviceList();

/*==========================================================================================*\
- - - - - - - - - - - - - UDP PAGE - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
\*==========================================================================================*/
    QVBoxLayout* MVL_UDPpage = new QVBoxLayout();
    QHBoxLayout* HL_IP = new QHBoxLayout();
    QHBoxLayout* HL_Port = new QHBoxLayout();


    IPaddress = new QLineEdit();
    QLabel* IP_label = new QLabel("IP address :");
    IPaddress->setMinimumWidth(120);
    IPaddress->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    IP_label->setFont(QFont("Segoe UI",11, QFont::Bold));
    IP_label->setAlignment(Qt::AlignRight);
    HL_IP->addWidget(IP_label,0,Qt::AlignRight);
    HL_IP->addWidget(IPaddress,0,Qt::AlignLeft);

    QLabel* Port_label = new QLabel("Port :");
    Port_label->setFont(QFont("Segoe UI",11, QFont::Bold));
    Port_label->setAlignment(Qt::AlignRight);
    Port = new QSpinBox();
    Port->setMinimumWidth(133);
    Port->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    Port->setMaximum(900000);
    HL_Port->addWidget(Port_label,0,Qt::AlignRight);
    HL_Port->addWidget(Port,0,Qt::AlignLeft);

    MVL_UDPpage->insertLayout(0,HL_IP,0);
    MVL_UDPpage->insertLayout(1,HL_Port,0);

    UDP_page->setLayout(MVL_UDPpage);
}

void ConnectionSetup::on_ApplyButton_clicked()
{
    ParentSB->SettingsConnection_pb->setEnabled(true);
    UpdateSetup();
    close();
}

void ConnectionSetup::closeEvent(QCloseEvent *event)
{
    on_ApplyButton_clicked();
    event->accept();
}
/*===============================================================================================*\
  ███████████████████████████████████████████████████████████████████████████████████████████████
  ███████████────█────█─██─█─██─█───█────█───█───█────█─██─█───█────██────█────██████████████████
  ███████████─██─█─██─█──█─█──█─█─███─██─██─███─██─██─█──█─█─███─██──█─██─█─██─██████████████████
  ███████████─████─██─█─█──█─█──█───█─█████─███─██─██─█─█──█───█────██────█────██████████████████
  ███████████─██─█─██─█─██─█─██─█─███─██─██─███─██─██─█─██─███─█─██──█─██─█─█─███████████████████
  ███████████────█────█─██─█─██─█───█────██─██───█────█─██─█───█────██─██─█─█─███████████████████
  ███████████████████████████████████████████████████████████████████████████████████████████████
\*===============================================================================================*/

ConnectionsBar::ConnectionsBar(QWidget *parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QHBoxLayout *ConnectionsBarLayout = new QHBoxLayout(this);
    USB_SN_l = new QLabel();
    SettingsConnection_pb = new QPushButton("Настройки соединения",parent);
    connect(SettingsConnection_pb,&QPushButton::clicked,this,&ConnectionsBar::callConnectionSetup);

    Status_l = new QLabel();
    Status_l->setFixedSize(15,15);
    Status_l->setStyleSheet("background-color: red;");
    ConnectionsBarLayout->addWidget(Status_l,0,Qt::AlignLeft);
    ConnectionsBarLayout->addWidget(USB_SN_l,1,Qt::AlignLeft);
    ConnectionsBarLayout->addWidget(SettingsConnection_pb,3,Qt::AlignLeft);


    ConnectionType = NON;
}

void ConnectionsBar::setStatus(bool status)
{
    if (status){
        Status_l->setStyleSheet("background-color: green;");
    }
    else{
         Status_l->setStyleSheet("background-color: red;");
    }
}

void ConnectionsBar::callConnectionSetup()
{
    ConnectionSetup* Setup = new ConnectionSetup(this);
    Setup->setWindowFlags(Qt::Window);
    Setup->show();

}




