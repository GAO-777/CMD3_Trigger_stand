#ifndef ETHERNET_INTERFACE_H
#define ETHERNET_INTERFACE_H


#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <string>

/*
    В pro файл нужно добавить: win32: LIBS += -lws2_32
*/


using namespace std;
class Ethernet_Interface
{
public:
    Ethernet_Interface();
    bool init(string ipAddress, unsigned short port); // true -> успех, fasle -> проблема с подлючением (см. Message)
    void closeSocket();                               // закрывает сокет
    bool write(WORD *Addr, WORD *Data, int size);
    bool read(WORD *Addr, WORD *Data, int size);

public:
    std::string Message;        // Строка хранит сообщения об ошибках и т.п.
protected:
    struct sockaddr_in MServer;
    WSADATA MWSA;
    SOCKET  Socket;
    bool Exchange(char *array, int array_len, int slen, DWORD timeout);
};
void WordToByte(int W,char *C);



#endif // ETHERNET_INTERFACE_H

