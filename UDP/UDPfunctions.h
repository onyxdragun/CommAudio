#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define TCP 0
#define UDP 1
#define BUFSIZE 256
#define SOUND_DATA_SIZE 211

typedef struct
{
	char *id[4];
	DWORD size;
	DWORD formatLength;
	short formatTag;
	short channels;
	short blockAlign;
	short bitsPerSample;
	DWORD sampleRate;
	DWORD avgBytesSec;
	DWORD dataSize;
	DWORD i;
	char *soundBuffer;
} wavheader;

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
	wavheader WH;
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




void CreateSocket(SOCKET *sd, int protocol);
void StoreServerInfo(TCPinfo *TI, UDPinfo *UI, int type);
void GetHost(TCPinfo *TI, UDPinfo *UI);
void CopyAddress(UDPinfo *UI, TCPinfo *TI);
void BindSockets(UDPinfo *UI, TCPinfo *TI);
void ListenForConnections(TCPinfo *TI, int numOfConnection);
/*
* not implemented properly
*/
int UDPread(UDPinfo *UI);
int TCPread(TCPinfo *TI);
int UDPsend(UDPinfo *UI, TCPinfo *TI);
int TCPcontrolsend(TCPinfo *TI, UDPinfo* UI, int filesize);
void TCPconnect(TCPinfo *TI);
FILE *wavOpen(wavheader *WH);
char *packetize(wavheader *WH, int *bufpos);
void TCPaccept(TCPinfo *TI);
