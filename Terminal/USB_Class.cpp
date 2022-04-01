#include "USB_Class.h"
#include <QDebug>
/* TODO : USB_JTAG */
USB_Class::USB_Class()
{
    DeviceIsDelected = false;
    FT_Status      = FT_OK;
}

/* TODO : Get_Status */
bool USB_Class::Get_Status()
{
     DWORD RxBytes,TxBytes;
     DWORD EventDWord;

     FT_GetStatus(FT_Handle,&RxBytes,&TxBytes,&EventDWord);
     if (EventDWord & FT_EVENT_MODEM_STATUS) return true;
     else                                    return false;
}
bool USB_Class::DeviceInfo()
{

    FT_Status = FT_GetDeviceInfo( FT_Handle, &ftDevice, &deviceID, SerialNumber, Description, NULL );
    if (FT_Status == FT_OK) return true;
    else return false;

}

DWORD USB_Class::DeviceDetail(int Number)
{
     DWORD numDevs;
// create the device information list
     FT_Status = FT_CreateDeviceInfoList(&numDevs);
     return numDevs;
}

/* TODO : Select_Device */
void USB_Class::Select_Device(const char* serial_number)
{
     memcpy(SerialNumber,serial_number,16);
     DeviceIsDelected = true;
}

/* TODO : Number_Of_Device */
int  USB_Class::Number_Of_Device()
{
     unsigned long numDevs=0;
     FT_Status=FT_ListDevices(&numDevs,NULL,FT_LIST_NUMBER_ONLY);
     return numDevs;
}


/* TODO : Number_Of_Queue_Data */
int  USB_Class::Number_Of_Queue_Data()
{
     unsigned long numData=0;
     FT_Status=FT_GetQueueStatus(FT_Handle,&numData);
     return numData;
}

/* TODO : Open_Device */
bool USB_Class::Open_Device()
{
    if( DeviceIsDelected == false){
        FT_Status=FT_Open(0,&FT_Handle);
        if(FT_Status!=FT_OK)  return false;
        FT_SetLatencyTimer(FT_Handle,0x02);
        return true;
    }
    else{
        FT_Status=FT_OpenEx(SerialNumber,FT_OPEN_BY_SERIAL_NUMBER,&FT_Handle);
        if(FT_Status!=FT_OK)  return false;
        FT_SetLatencyTimer(FT_Handle,0x02);
        return true;
    }
}

/* TODO : Close_Device */
bool USB_Class::Close_Device()
{
     FT_Status=FT_Close(FT_Handle);
     if(FT_Status!=FT_OK)  return false;
     return true;
}

/* TODO : Reset_Device */
bool USB_Class::Reset_Device(int sleep)
{
     Close_Device();
     Sleep(sleep);
     Open_Device();
     if(FT_Status!=FT_OK)  return false;
     return true;
}
/* TODO : Write_Data */
bool USB_Class::Write_Data(unsigned char* Source, int Size)
{
     FT_Status=FT_Write(FT_Handle, Source,Size,&NumOfWritten);
     if(FT_Status!=FT_OK)  return false;
     return true;
}

/* TODO : Read_Data */
bool  USB_Class::Read_Data(unsigned char* Dest, int Size)
{
     FT_Status=FT_Read(FT_Handle,Dest,Size,&NumOfRead);
     if(FT_Status!=FT_OK)  return false;
     return true;
}
void  USB_Class::SetTimeouts(int TxTimeout, int RxTimeout)
{
     FT_SetTimeouts(FT_Handle, RxTimeout, TxTimeout);
}
UCHAR  USB_Class::GetBitMode()
{
     UCHAR BitMode;
     if(FT_GetBitMode(FT_Handle, &BitMode) !=FT_OK) return 0;

     return BitMode;
}

/*UCHAR  USB_Class::SetBitMode()
{
     UCHAR BitMode;
     if(FT_GetBitMode(FT_Handle, &BitMode) !=FT_OK) return 0;

     return BitMode;
}  */

/* TODO : ~USB_Class */
USB_Class::~USB_Class()
{
     Close_Device();
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //
//                  Функции определенные вне класса
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //
int GetDeviceInfoList(FT_STATUS *ftStatus, FT_DEVICE_LIST_INFO_NODE *devInfo){
    DWORD numDevs;
    // create the device information list
    *ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (*ftStatus == FT_OK) {
        printf("Number of devices is %d\n",numDevs);
    }
    if (numDevs > 0) {
        // allocate storage for list based on numDevs
        devInfo =
                (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
        // get the device information list
        *ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
    }
    return static_cast<int>(numDevs);
}


int USB_Send_Data_Protocol(USB_Class *USB, WORD ServiceType, WORD *Addr, WORD*Data, int size)
{
  DWORD TxBytes=0,Lenght=0;
  int i,t;
  BYTE *TxBuffer;

  i = size*2;
  if(i>1024) i=1024;
  TxBuffer = new BYTE [1024];

  TxBytes = 16+size*4+8;       // Total bytes to be transmitted
  if(TxBytes>1024) return -1;
  Lenght  = TxBytes-12;     // Bytes number to be checked by hardware
  // Header bytes does not included in count, while trailer does

// Заполняем  TxBuffer
  //1. Спец. символ начала посылки
  for(i=0;i<12;i++)
    TxBuffer[i] = 0x55;
  //2. Длина посылки
  TxBuffer[12] = (BYTE)Lenght;          // LSB
  TxBuffer[13] = (BYTE)(Lenght>>8);     // MSB
  //3. Команда
  TxBuffer[14] = (BYTE)ServiceType;     // LSB
  TxBuffer[15] = (BYTE)(ServiceType>>8);  // MSB
  //4. Адрес и данные
  for(i=0;i<size;i++) //Указываем число слов данных отправляемых по USB
  { TxBuffer[16+i*4]   = (BYTE)Addr[i];      // адрес ресурса
    TxBuffer[16+i*4+1] = (BYTE)(Addr[i]>>8); // внутри блока
    TxBuffer[16+i*4+2] = (BYTE)Data[i];         // данные
    TxBuffer[16+i*4+3] = (BYTE)(Data[i]>>8);}   // для этого адреса
  //5. Спец. символ конца посылки
  for(i=16+size*4;i<16+size*4+8;i++)
    TxBuffer[i] = 0xAA;

//отправляем данные по USB блок
  (*USB).Write_Data(TxBuffer,TxBytes);

  delete TxBuffer;

  return TxBytes;
}

int USB_Recive_Data_Protocol(USB_Class *USB, BYTE *RxBuffer)
{
   int BytesReceived;

 // Пороверяем сколько байт доступно на чтение
   BytesReceived = (*USB).Number_Of_Queue_Data();

   if(BytesReceived>0)
   {   if(BytesReceived>1024) BytesReceived=1024;
       (*USB).Read_Data(RxBuffer,BytesReceived);
       return BytesReceived; }
   else return -1;
}
int USB_Read_Data(USB_Class *USB, WORD *a, int size, unsigned int *Data)
{
    WORD ServiceType;
    WORD *Addr;

    int TxBytes,RxBytes,i,n;
    BYTE *RxData;

    Addr   = a;
    RxData = new BYTE[16+size*4+8];

// выбор типа сервиса (чтение)
    ServiceType = 0xABCD;

    TxBytes = USB_Send_Data_Protocol(USB, ServiceType, Addr, Addr, size);
    ::Sleep(10);
    RxBytes = USB_Recive_Data_Protocol(USB, RxData);

    //Вывод данных
    if(RxBytes>0)
      for(i=0;i<size;i++)
      {  Data[i] = (WORD)RxData[16+i*4+2] + (WORD)(RxData[16+i*4+3]<<8);}

    return RxBytes;
}

int USB_Write_Data(USB_Class *USB, WORD *a, int size, WORD *Data)
{
    WORD ServiceType;
    WORD *Addr;
    int TxBytes,RxBytes;
    BYTE *RxData;

    Addr   = a;
    RxData = new BYTE[16+size*4+8];

// выбор типа сервиса (запись)
    ServiceType = 0xCDAB;

    TxBytes = USB_Send_Data_Protocol(USB, ServiceType, Addr, Data, size);
    ::Sleep(10);
    RxBytes = USB_Recive_Data_Protocol(USB, RxData);

    delete[] RxData;

    return TxBytes;
}
