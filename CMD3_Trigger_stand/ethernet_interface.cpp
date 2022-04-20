#include "ethernet_interface.h"

Ethernet_Interface::Ethernet_Interface()
{
    Message = "";
}

bool Ethernet_Interface::init(string ipAddress, unsigned short port)
{
    /*
        Метод возвращает TRUE в случае, когда подлючение произошло успешно,
        возвращает FALSE в случае, когда при подлючении произошла ошибка.
        Код ошибки можно посмотреть в строке Message.
    */

    //Initialise winsock
    if (WSAStartup(MAKEWORD(2,2),&MWSA) != 0)
    {
        Message = WSAGetLastError();
        return false;
    }

    //Create a socket    IPPROTO_IPV4    IPPROTO_UDP
    if((Socket = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP )) == INVALID_SOCKET)
    {
        Message = WSAGetLastError();
        WSACleanup();
        return false;
    }
    //Prepare the sockaddr_in structure
    MServer.sin_family = AF_INET;
    MServer.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    MServer.sin_port = htons(port);

    return  true;
}

void Ethernet_Interface::closeSocket()
{
    closesocket(Socket);
    WSACleanup();
}

bool Ethernet_Interface::Exchange(char *array, int array_len, int slen, DWORD timeout)
{
    if (sendto(Socket, array, array_len, 0, (struct sockaddr*) &MServer, slen) == SOCKET_ERROR)
    {
        Message = WSAGetLastError();
        return false;
    }

    setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(DWORD));
    if( recvfrom(Socket, array, array_len, 0, (struct sockaddr*) &MServer, &slen) == SOCKET_ERROR)
    {
        Message = WSAGetLastError();
        return false;
    }
      return true;
}

bool Ethernet_Interface::write(WORD *Addr, WORD *Data, int size)
{
    int slen,i;
    char *array;
    int array_len;

    // установки для Ethernet обмена
    slen = sizeof(MServer) ;
    array_len = size*4+2;       // пересчитываем кол-во слов данных в байты с учетом адреса
    array = new char[array_len];
    // заполняем посылку
    WordToByte(3,array); // вносим команду записи
    for(i=0;i<size;i++)
    {       WordToByte(Addr[i],(array+i*4+2));   // адрес
            WordToByte(Data[i],(array+i*4+4));   // данные
    }

    return Exchange(array, array_len, slen, 200);
}

bool Ethernet_Interface::read(WORD *Addr, WORD *Data, int size)
{
    int Eth_Exchange_Status = 0;
    int slen,i;
    char *array;
    int array_len;

    // установки для Ethernet обмена
    slen = sizeof(MServer) ;
    array_len = size*4+2;   // пересчитываем кол-во слов данных в байты с учетом адреса
    array = new char[array_len];
    // заполняем посылку
    WordToByte(2,array); // вносим команду чтения
    for(i=0;i<size;i++)
    {       WordToByte(Addr[i],(array+i*4+2));   // адрес
            WordToByte(Data[i],(array+i*4+4));   // данные
    }

    bool Exchange_Status = Exchange(array, array_len, slen, 200);

    if(Exchange_Status)
        for(i=0;i<size;i++)
            Data[i] = (((WORD)array[i*4+4]<<8)&0xFF00)+(((WORD)array[i*4+5])&0xFF);

    return Eth_Exchange_Status;
}

void WordToByte(int W, char *C)
{
    int Data_msb,Data_lsb;

    Data_msb = (W>>8)&0xff;
    Data_lsb = W&0xff;

    C[0]  =  (char)Data_msb;
    C[1]  =  (char)Data_lsb;
}
