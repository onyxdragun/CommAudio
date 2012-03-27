#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define TCP 0
#define UDP 1
#define BUFSIZE 512

typedef struct
{
	char *hostname;
	int port;
	SOCKET sd;
	struct sockaddr_in	server;
	struct sockaddr_in	client;
	struct hostent *hp;
	int server_len;
	int client_len;
	WSAOVERLAPPED overlapped;
} UDPinfo;

typedef struct
{
	SOCKET sd;
	SOCKET new_sd;
	struct hostent *hp;
	struct sockaddr_in	server;
	struct sockaddr_in	client;
	WSAOVERLAPPED overlapped;
}TCPinfo;

SOCKET CreateSocket(SOCKET *sd, int protocol);
void StoreServerInfo(TCPinfo *TI, UDPinfo *UI, int type);
void GetHost(TCPinfo *TI, UDPinfo *UI);
void CopyAddress(UDPinfo *UI, TCPinfo *TI);
void BindSockets(UDPinfo *UI, TCPinfo *TI);
void ListenForConnections(TCPinfo *TI, int numOfConnection);
int UDPread(UDPinfo *UI);
int TCPread(TCPinfo *TI);
int UDPsend(UDPinfo *UI, FILE *fp);
int TCPsend(TCPinfo *TI, UDPinfo* UI, int filesize);
void TCPconnect(TCPinfo *TI);
