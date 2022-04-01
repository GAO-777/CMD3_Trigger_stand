#ifndef BINP_DEVICE_H
#define BINP_DEVICE_H

#include <Windows.h>
#include "USB_Class.h"
#include "drv/ftd2xx.h"

#include<QString>
#include<QList>
#include<QDebug>
// - - - Определение базовых адресов Down_Link'ов - - - //
#define DL_1 36864
#define DL_2 37120
#define DL_3 37376
#define DL_4 37632
#define DL_5 37888
#define DL_6 38144
#define DL_7 38400
#define DL_8 38656

class MCHS_Imitator
{
public:
    MCHS_Imitator(QString USB_SN);
    MCHS_Imitator();
    USB_Class* USB_Device;
    bool USB_DeviceStatus;
    void setSN(QString USB_SN);
    bool openUSB();
    bool closeUSB();
    void GeneratorsOff();

    int getStartAddrOfDownLink(int numOfDownLink);
    // - - - Функции, использующие 130 и 137 команды. Используются всеми платами, использующие C-Link - - - - - - - - - - - - - //
    bool USBtoCLink_RedirRead(int numOfDownLink, unsigned short CL_Addr, QList<unsigned short> *Data);  // чтение 62-x слов КЛ
    bool CLink_RedirWrite(int numOfDownLink, unsigned short CL_Addr, QList<unsigned short>* Data);  // запись данных в КЛ (8 слов)
    bool CLink_TxRx(int numOfDownLink, int opcode, QList<unsigned short>* Addr, QList<unsigned short>* Data);  // запись данных в КЛ (любого кол-во слов)
    bool CLink_sendOpcode(int numOfDownLink,int opcode);
    // - - - Функции, использующие 145 и 154 команды. Разработка Юдина Ю.В. - - - - - - - - - - - - - - - - - - - - - - - - - - //
    bool SendWrite4  (int numOfDownLink, int N, QList<unsigned short>* Addr, QList<unsigned short>* Data);  // запись данных (до 4 слов)
    bool SendWrite154(int numOfDownLink, int N, QList<unsigned short>* Addr, QList<unsigned short>* Data);  // запись любого кол-ва данных
    bool SendRead8   (int numOfDownLink, int N, QList<unsigned short>* Addr, QList<unsigned short> *Data);  // чтение данных (до 8 слов)
    bool SendRead145 (int numOfDownLink, int N, QList<unsigned short>* Addr, QList<unsigned short> *Data);  // чтение любого кол-ва данных

    bool Init_Waiting193(int numOfDownLink); // Инициальзация МЧС для ожижания 193 команды
    bool ReadDataFromDownLink (int numOfDownLink, QList<unsigned short> *Data);  // чтение данных по 193 команде
};

class ADIS
{
public:
  ADIS();
  bool setData_FL(MCHS_Imitator* MCHS_Imitator, int numOfDownLink,int data);
};

#endif // BINP_DEVICE_H
