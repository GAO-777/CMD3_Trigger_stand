#include "binp_device.h"

/*===============================================================================================*\
 █████████████████████████████████████████████████████████████████████████████████████████████████
 ███████████████─███─█────█─██─█───██████───█─███─█───█───█────█───█────█────█████████████████████
 ███████████████──█──█─██─█─██─█─█████████─██──█──██─███─██─██─██─██─██─█─██─█████████████████████
 ███████████████─█─█─█─████────█───███████─██─█─█─██─███─██────██─██─██─█────█████████████████████
 ███████████████─███─█─██─█─██─███─███████─██─███─██─███─██─██─██─██─██─█─█─██████████████████████
 ███████████████─███─█────█─██─█───██████───█─███─█───██─██─██─██─██────█─█─██████████████████████
 █████████████████████████████████████████████████████████████████████████████████████████████████
\*===============================================================================================*/

MCHS_Imitator::MCHS_Imitator(QString USB_SN)
{
    USB_Device = new USB_Class();
    USB_Device->Select_Device(USB_SN.toStdString().c_str());
}

MCHS_Imitator::MCHS_Imitator()
{
    USB_Device = new USB_Class();
}

void MCHS_Imitator::setSN(QString USB_SN)
{
    USB_Device->Select_Device(USB_SN.toStdString().c_str());
}

bool MCHS_Imitator::openUSB()
{
    USB_DeviceStatus = USB_Device->Open_Device();
    return USB_DeviceStatus;
}

bool MCHS_Imitator::closeUSB()
{
    USB_DeviceStatus = !USB_Device->Close_Device();
    return !USB_DeviceStatus;
}

void MCHS_Imitator::GeneratorsOff()
{
    WORD* addr  = new WORD[4];
    WORD* data  = new WORD[4];
    for(int i =0;i<4;i++){
        addr[i] = 0;
        data[i] = 0;
    }
// - - - ВЫКЛ Генератор случайных сигналов - - -//
    addr[0] = 9221;
    data[0] = 0;
// - - - ВЫКЛ Контроллер Блокировок - - -//
    addr[1] = 9482;
    data[1] = 0;
// - - - ВЫКЛ Служебный Генератор - - -//
    addr[2] = 9994;
    data[2] = 0;
// - - - ВЫКЛ Генератор Равномерных сигналов - - -//
    addr[3] = 9225;
    data[3] = 0;

    USB_Write_Data(USB_Device,addr,4,data);
}

int MCHS_Imitator::getStartAddrOfDownLink(int numOfDownLink)
{
    int DL_startAddr = 0; // Первая ячейка адресного пр-ва DownLink'a
    // Данные, возвращенные в ходе Транзакции лежат -> DL_startAddr +17
    switch (numOfDownLink)
    {
        case 1: DL_startAddr= DL_1;
            break;
        case 2: DL_startAddr= DL_2;
            break;
        case 3: DL_startAddr= DL_3;
            break;
        case 4: DL_startAddr= DL_4;
            break;
        case 5: DL_startAddr= DL_5;
            break;
        case 6: DL_startAddr= DL_6;
            break;
        case 7: DL_startAddr= DL_7;
            break;
        case 8: DL_startAddr= DL_8;
            break;
    }
    return DL_startAddr;
}

bool MCHS_Imitator::USBtoCLink_RedirRead(int numOfDownLink, unsigned short CL_Addr, QList<unsigned short> *Data)
{
    if (numOfDownLink > 8) return false;

    int size = 62;
    WORD* addr  = new WORD[size];
    WORD* data  = new WORD[size];
    for(int i =0;i<size;i++){
        addr[i] = 0;
        data[i] = 0;
    }
    unsigned int* InData = new unsigned int[size]; // Массив прочитанных данных
    unsigned short startAddr = getStartAddrOfDownLink(numOfDownLink);
    unsigned short startAddrRead = startAddr +17;

    addr[0] = startAddr;
    data[0] = 130;
    addr[1] = startAddr + 1;
    data[1] = CL_Addr;
    addr[2] = startAddr + 10;
    data[2] = 0;
    addr[3] = startAddr + 11;
    data[3] = 0;
    USB_Write_Data(USB_Device,addr,4,data);

    qDebug()<<"\n"<<"USBtoCLink_RedirRead";
    for(int i=0;i<4;i++){
        qDebug()<<addr[i]<<"<="<<data[i];
    }

   for(int i=0;i<size;i++){
       addr[i] = startAddrRead + i;
   }
   USB_Read_Data(USB_Device,addr,size,InData);

   for(int i=0;i<size;i++){
       Data->append(InData[i]);
   }
   delete[] addr;
   delete[] data;
    return true;

}

bool MCHS_Imitator::CLink_RedirWrite(int numOfDownLink, unsigned short CL_Addr, QList<unsigned short> *Data)
{
    if (numOfDownLink > 8) return false;
    int size = 12;
    WORD* addr  = new WORD[size];
    WORD* data  = new WORD[size];
    for(int i =0;i<size;i++){
        addr[i] = 0;
        data[i] = 0;
    }

    unsigned short DL_startAddr = getStartAddrOfDownLink(numOfDownLink); // Указывает на первую ячейку памяти адресного про-ва выбранного DownLink'a

// - - - Заполняем массив адресов и данных для обмена - - - //
    // Заполняем таблицe ресурсов модуля Down_Link
    addr[0] = DL_startAddr;     // записываем
    data[0] = 137;              // ... команду
    addr[1] = DL_startAddr +1;  // записываем
    data[1] = CL_Addr;          // ... адрес КЛ

    // Записываем 8 слов данных
    for(int i=0;i<8;i++){
        addr[2+i] = DL_startAddr+2+i;
        if(i<Data->size())
            data[2+i] = Data->at(i);
        else
            data[2+i] = 0;
    }

    addr[10] = DL_startAddr + 11;
    data[10] = 0;
    addr[11] = DL_startAddr + 10;
    data[11] = 1;

    for(int i=0;i<size;i++){
        qDebug()<<addr[i]<<"<="<<data[i];
    }
    USB_Write_Data(USB_Device,addr,size, data);

    return true;
    delete[] addr;
    delete[] data;
}

bool MCHS_Imitator::CLink_TxRx(int numOfDownLink, int opcode, QList<unsigned short> *Addr, QList<unsigned short> *Data)
{
    /*
       Данная команда осуществляет запись данных во внутренние ресурсы устройств, путем использования Командных Листов (КЛ).
       Запись будет инициироваться командой WR_CMD.
       Порядок действий:
           1. Записываем адреса в соотвествующий КЛ WR_CMD, используя 137 команду
           2. Записываем данные в соотвествующий КЛ WR_CMD, используя 137 команду
           3. Отправялем WR_CMD команду.
   */

    if (numOfDownLink > 8) return false;

    int N = 0; // Кол-во слов для записи
    int addr_RedirOffset = 0;
    int addr_offset = 0;
    int rA = 0;
    bool wD = true;     // Нужно ли заполнять лист данных КЛ?

    // Проверка, что кол-во адресов равно кол-ву данных
    if (Addr->size() == Data->size())
        N = Addr->size();
    else
        return false;

    switch(opcode)
        {
          case 193: rA=0;   wD=false; break;
          case 210: rA=128; wD=false; break;
          case 226: rA=256; wD=false; break;
          case 241: rA=384; wD=false; break;
          case 202: rA=64;  wD=true;  break;
          case 217: rA=192; wD=true;  break;
          case 233: rA=320; wD=true;  break;
          case 250: rA=448; wD=true;  break;
          default : return false;
        }

    QList<unsigned short>* Addr_local = new QList<unsigned short>();
    QList<unsigned short>* Data_local = new QList<unsigned short>();
    QList<unsigned short> addr8 = QList<unsigned short>(); // Контейнер для хранения 8 слов для записи
    QList<unsigned short> data8 = QList<unsigned short>(); // Контейнер для хранения 8 слов для записи

    int cycle = ((N%64)==0) ? N/64 : (N/64)+1;
    for(int i=0;i<cycle*64;i++){
        if(i<Addr->size())
            Addr_local->append(Addr->at(i));
        else
            Addr_local->append(0);
        if(i<Data->size())
            Data_local->append(Data->at(i));
        else
            Data_local->append(0);
    }

    for(int i=0;i<cycle;i++){

        for(int j=0;j<8;j++){
            addr_RedirOffset = 8*j;
            // заполняем адресное пространство
            addr8 = Addr_local->sliced(addr_offset,8);
            data8 = Data_local->sliced(addr_offset,8);

            CLink_RedirWrite(numOfDownLink,rA+addr_RedirOffset,&addr8);
            if(wD==true) // заполняем пространство данных
                CLink_RedirWrite(numOfDownLink,rA+addr_RedirOffset+512,&data8);

            addr_offset = addr_offset+8;
            addr8.clear();
            data8.clear();
        }
        // После того, как КЛ заполнены, то отправляем соответствующую команду, чтоб инициировать выполнение
        CLink_sendOpcode(numOfDownLink,opcode);
        addr_RedirOffset = 0;
    }

    delete Addr_local;
    delete Data_local;

    return true;

}

bool MCHS_Imitator::CLink_sendOpcode(int numOfDownLink, int opcode)
{
       WORD *Addr;
       WORD *Data;
       int size,i;
       size   = 2;
       Addr   = new WORD[size];
       Data   = new WORD[size];

       unsigned short DL_startAddr = getStartAddrOfDownLink(numOfDownLink); // Указывает на первую ячейку памяти адресного про-ва выбранного DownLink'a

       Addr[0] = DL_startAddr;
       Data[0] = opcode;
       Addr[1] = DL_startAddr+10;
       Data[1] = 1;

       USB_Write_Data(USB_Device, Addr, 2, Data);
       qDebug()<<"CLink_sendOpcode: "<<"DownLink "<<numOfDownLink<<" -> "<<opcode;

       delete[] Data;
       delete[] Addr;
}


bool MCHS_Imitator::SendWrite4(int numOfDownLink,int N,QList<unsigned short> *Addr, QList<unsigned short> *Data)
{
    if (numOfDownLink > 8) return false;
    if (N > 5) return false;

    WORD* addr  = new WORD[12];
    WORD* data  = new WORD[12];
    for(int i =0;i<12;i++){
        addr[i] = 0;
        data[i] = 0;
    }

    unsigned short startAddr = 0;

    switch (numOfDownLink)
    {
        case 1: startAddr= DL_1;
            break;
        case 2: startAddr= DL_2;
            break;
        case 3: startAddr= DL_3;
            break;
        case 4: startAddr= DL_4;
            break;
        case 5: startAddr= DL_5;
            break;
        case 6: startAddr= DL_6;
            break;
        case 7: startAddr= DL_7;
            break;
        case 8: startAddr= DL_8;
            break;
    }

    addr[0] = startAddr;
    data[0] = 154;
    addr[1] = startAddr + 1;
    data[1] = N;
    int p=0;
    for(int i=0;i<N*2;i++){
        if(i%2 == 0){
            addr[2+i] = startAddr + 2 + i;
            data[2+i] = Addr->at(p);
        }else{
            addr[2+i] = startAddr + 2 + i;
            data[2+i] = Data->at(p);
            p++;
        }
    }
    addr[10] = startAddr + 11;
    data[10] = 0;
    addr[11] = startAddr + 10;
    data[11] = 0;

    for(int i=0;i<12;i++){
        qDebug()<<addr[i]<<"<="<<data[i];
    }
    USB_Write_Data(USB_Device,addr,12, data);

    return true;
    delete[] addr;
    delete[] data;
}

bool MCHS_Imitator::SendWrite154(int numOfDownLink, int N, QList<unsigned short> *Addr, QList<unsigned short> *Data)
{
    QList<unsigned short> *addr4 = new QList<unsigned short>();
    QList<unsigned short> *data4 = new QList<unsigned short>();
    for(int i=0;i<(4*(N/4));i++){
        if(i%4 != 3){
            addr4->append(Addr->at(i));
            data4->append(Data->at(i));
        }
        else{
            addr4->append(Addr->at(i));
            data4->append(Data->at(i));
            if(!SendWrite4(numOfDownLink,4,addr4,data4)) return false;
            addr4->clear();
            data4->clear();
        }
    }
    addr4->clear();
    data4->clear();
    if(N%4>0){
        for(int i=(N/4)*4;i<((N/4)*4)+N%4;i++){
            addr4->append(Addr->at(i));
            data4->append(Data->at(i));
        }
        if(!SendWrite4(numOfDownLink,N%4,addr4,data4)) return false;
    }

    delete addr4;
    delete data4;
    return true;
}

bool MCHS_Imitator::SendRead8(int numOfDownLink, int N, QList<unsigned short> *Addr, QList<unsigned short> *Data)
{
    if (numOfDownLink > 8) return false;
    if (N > 9) return false;

    WORD* addr  = new WORD[12];
    WORD* data  = new WORD[12];
    WORD* addrRead = new WORD[62];

    for(int i =0;i<12;i++){
        addr[i] = 0;
        data[i] = 0;
    }
    unsigned int* InData = new unsigned int[N]; // Массив прочитанных данных
    unsigned short startAddr = 0;
    unsigned short startAddrRead = 0;
    switch (numOfDownLink)
    {
        case 1: startAddr       = DL_1;
                startAddrRead   = DL_1 + 17;
            break;
        case 2: startAddr       = DL_2;
                startAddrRead   = DL_2 + 17;
            break;
        case 3: startAddr       = DL_3;
                startAddrRead   = DL_3 + 17;
            break;
        case 4: startAddr       = DL_4;
                startAddrRead   = DL_4 + 17;
            break;
        case 5: startAddr       = DL_5;
                startAddrRead   = DL_5 + 17;
            break;
        case 6: startAddr       = DL_6;
                startAddrRead   = DL_6 + 17;
            break;
        case 7: startAddr       = DL_7;
                startAddrRead   = DL_7 + 17;
            break;
        case 8: startAddr       = DL_8;
                startAddrRead   = DL_8 + 17;
            break;
    }

    addr[0] = startAddr;
    data[0] = 145;
    addr[1] = startAddr + 1;
    data[1] = N;

    for(int i=0;i<N;i++){
        addr[2+i] = startAddr + 2 + i;
        data[2+i] = Addr->at(i);
    }

    addr[10] = startAddr + 10;
    data[10] = 0;
    addr[11] = startAddr + 11;
    data[11] = 0;

    for(int i=0;i<12;i++){
        qDebug()<<addr[i]<<"<="<<data[i];
    }
   USB_Write_Data(USB_Device,addr,11,data);

   for(int i=0;i<N;i++){
       addrRead[i] = startAddrRead + i;
   }

   USB_Read_Data(USB_Device,addrRead,N,InData);

   for(int i=0;i<N;i++){
       Data->append(InData[i]);
   }
   delete[] addr;
   delete[] addrRead;
   delete[] data;
    return true;
}

bool MCHS_Imitator::SendRead145(int numOfDownLink, int N, QList<unsigned short> *Addr, QList<unsigned short> *Data)
{
    if (numOfDownLink > 8) return false;
    QList<unsigned short> *addr8 = new QList<unsigned short>();
    for(int i=0;i<N;i++){

        if(i%8 != 7){
            addr8->append(Addr->at(i));
        }
        else{
            addr8->append(Addr->at(i));
            if(!SendRead8(numOfDownLink,8,addr8,Data)) return false;
            addr8->clear();
        }
    }
    addr8->clear();

    if(N%8>0){
        for(int i=(N/8)*4;i<((N/8)*8)+N%8;i++)
            addr8->append(Addr->at(i));
        if(!SendRead8(numOfDownLink,N%8,addr8,Data)) return false;
    }
    delete addr8;
    return true;

}

bool MCHS_Imitator::Init_Waiting193(int numOfDownLink)
{
    if (numOfDownLink > 8) return false;

    WORD* addr  = new WORD[9];
    WORD* data  = new WORD[9];

    for(int i =0;i<9;i++){
        addr[i] = 0;
        data[i] = 0;
    }

    addr[0] = 9221; // Отключаем Ген-р Случайных Интервалов
    data[0] = 0;

    addr[1] = 9225; // Вкл Ген-р Равномерных Сигналов
    data[1] = 1;

    addr[2] = 9224;  // Устанавливаем частоту равномерных сигналов (старшее слово)
    data[2] = 0;

    addr[3] = 9223; // Устанавливаем частоту равномерных сигналов (младшее слово)
    data[3] = 0;

    addr[4] = 9482; // Отключаем Формирователь Блокировок
    data[4] = 0;

    addr[5] = 9994; // Отключаем Служебный Генератор
    data[5] = 0;

    addr[6] = 1795; // Прописываем команду, которая будет выдана по приходу NIM
    data[6] = 193;

    addr[7] = 36864+11; // Переводим МЧС в режим "Запуск от Генераторов"
    data[7] = 1;

    addr[8] = 36864+10; // Переводим МЧС в режим "Запуск от Генераторов"
    data[8] = 1;




    // Печатаем в Консоль, то что послали в МЧС
    qDebug()<<"Init_Waiting193";
    for(int i=0;i<9;i++){
        qDebug()<<addr[i]<<"<="<<data[i];
    }
    USB_Write_Data(USB_Device,addr,9,data);
}

bool MCHS_Imitator::ReadDataFromDownLink(int numOfDownLink, QList<unsigned short> *Data)
{
    if (numOfDownLink > 8) return false;
    int N = 64;

    WORD* addrRead = new WORD[N];

    unsigned int* InData = new unsigned int[N]; // Массив прочитанных данных
    unsigned short startAddrRead = 0;
    switch (numOfDownLink)
    {
        case 1: startAddrRead   = DL_1 + 17;
                break;
        case 2: startAddrRead   = DL_2 + 17;
            break;
        case 3: startAddrRead   = DL_3 + 17;
            break;
        case 4: startAddrRead   = DL_4 + 17;
            break;
        case 5: startAddrRead   = DL_5 + 17;
            break;
        case 6: startAddrRead   = DL_6 + 17;
            break;
        case 7: startAddrRead   = DL_7 + 17;
            break;
        case 8: startAddrRead   = DL_8 + 17;
            break;
    }

   for(int i=0;i<64;i++){
       addrRead[i] = startAddrRead + i;
   }

   USB_Read_Data(USB_Device,addrRead,N,InData);

   for(int i=0;i<64;i++){
       Data->append(InData[i]);
   }
   delete[] addrRead;
   return true;
}

/*===============================================================================================*\
 █████████████████████████████████████████████████████████████████████████████████████████████████
 ████████████████████████████████████████────█────██───█───███████████████████████████████████████
 ████████████████████████████████████████─██─█─██──██─██─█████████████████████████████████████████
 ████████████████████████████████████████────█─██──██─██───███████████████████████████████████████
 ████████████████████████████████████████─██─█─██──██─████─███████████████████████████████████████
 ████████████████████████████████████████─██─█────██───█───███████████████████████████████████████
 █████████████████████████████████████████████████████████████████████████████████████████████████
\*===============================================================================================*/

ADIS::ADIS()
{

}

bool ADIS::setData_FL(MCHS_Imitator* MCHS_Imitator, int numOfDownLink, int data)
{

    QList<unsigned short> *Addr = new QList<unsigned short>();
    QList<unsigned short> *Data = new QList<unsigned short>();

    QList<unsigned short> *Addr1 = new QList<unsigned short>();
    QList<unsigned short> *Data1 = new QList<unsigned short>();

    QList<unsigned short> *ReadAddr = new QList<unsigned short>();
    QList<unsigned short> *ReadData = new QList<unsigned short>();



    for(int i=0;i<512;i++){
        if(i%512==0){
            Addr->append(9216+i);
            Data->append(data);
        }
        else{
            Addr->append(9216+i);
            Data->append(0);
        }
    }
   /* for(int i=0;i<512;i++){

            Addr->append(9216+i);
            Data->append(data);

    }*/

     MCHS_Imitator->CLink_TxRx(numOfDownLink,202,Addr,Data);


     Addr1->append(9984);
     Data1->append(1);
     Addr1->append(12800);
     Data1->append(1);

     MCHS_Imitator->CLink_TxRx(numOfDownLink,202,Addr1,Data1);


}

































