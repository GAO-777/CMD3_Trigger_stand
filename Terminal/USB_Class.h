
#include "drv/ftd2xx.h"

//#include "queue.h"

#define   INIT_CMD_SIZE            5
#define   MAX_TOTAL_PACKET         256-20
#define   MAX_TXD_PACKET           256
#define   MAX_RXD_PACKET           100
#define   ALMOST_FULL_SIZE         20
#pragma once
class USB_Class
{
     unsigned long  NumOfWritten;
     unsigned long  NumOfRead;
//     unsigned char  TXD_Buffer[MAX_TXD_PACKET];
//    queue<unsigned char> Buffer;
public:
     FT_HANDLE FT_Handle;
     FT_STATUS FT_Status;
     FT_DEVICE ftDevice;
     DWORD deviceID;
     char SerialNumber[16];
     char Description[64];
     bool DeviceIsDelected;

     USB_Class();
     bool Get_Status();
     DWORD DeviceDetail(int);
     bool DeviceInfo();
     void Select_Device(const char *serial_number);
     int  Number_Of_Device();
     int  Number_Of_Queue_Data();
     bool Open_Device();
     bool Close_Device();
     bool Reset_Device(int sleep);
     bool Write_Data(unsigned char* Source, int Size);
     bool Read_Data(unsigned char* Dest, int Size);
     void  SetTimeouts(int, int);
     UCHAR  GetBitMode();
     ~USB_Class();
};

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //
//                  Функции определенные вне класса
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //

int GetDeviceInfoList(FT_STATUS* ftStatus,FT_DEVICE_LIST_INFO_NODE *devInfo); // Выдает список подлюченных устройств
int USB_Send_Data_Protocol(USB_Class *USB, WORD ServiceType, WORD *Addr, WORD*Data, int size);
int USB_Recive_Data_Protocol(USB_Class *USB, BYTE *RxBuffer);
int USB_Read_Data(USB_Class *USB, WORD *a, int size, unsigned int *Data);
int USB_Write_Data(USB_Class *USB, WORD *a, int size, WORD *Data);


 
