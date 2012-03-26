#include "UDPfunctions.h"

void CreateSocket(SOCKET *sd, int protocol)
{
	if(protocol == 1)
	{
		if((*sd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
		{
			fprintf(stderr, "error opening socket");
		}
	}
	else
	{
		if((*sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			fprintf(stderr, "error opening socket");
		}
	}
}


void StoreServerInfo(TCPinfo *TI, UDPinfo *UI, int type)
{
	if(type == 1)
	{
		memset((char *)&UI->server, 0, sizeof(UI->server));
		UI->server.sin_family = PF_INET;
		UI->server.sin_port = htons(UI->port);

		memset((char *)&TI->server, 0, sizeof(TI->server));
		TI->server.sin_family = AF_INET;
		TI->server.sin_port = htons(UI->port);
	}
	else
	{
		memset((char *)&UI->server, 0, sizeof(UI->server));
		UI->server.sin_family = PF_INET;
		UI->server.sin_port = htons(UI->port);
		UI->server.sin_addr.s_addr = htonl(INADDR_ANY);

		memset((char *)&TI->server, 0, sizeof(TI->server));
		TI->server.sin_family = AF_INET;
		TI->server.sin_port = htons(UI->port);
		TI->server.sin_addr.s_addr = htonl(INADDR_ANY);
	}
}

void GetHost(TCPinfo *TI, UDPinfo *UI)
{
	if((UI->hp = gethostbyname(UI->hostname)) == NULL)
	{
		exit(16);
	}
	if((TI->hp = gethostbyname(UI->hostname)) == NULL)
	{
		exit(3);
	}
}

void CopyAddress(UDPinfo *UI, TCPinfo *TI)
{
	memcpy((char *)&UI->server.sin_addr, UI->hp->h_addr, UI->hp->h_length);
	memcpy((char *)&TI->server.sin_addr, TI->hp->h_addr, TI->hp->h_length);

	
	UI->client_len = sizeof(UI->client);
	UI->server_len = sizeof(UI->server);

}


void BindSockets(UDPinfo *UI, TCPinfo *TI)
{
	if(bind(UI->sd, (struct sockaddr *)&UI->server, sizeof(UI->server)) == SOCKET_ERROR)
	{
		exit(2);
	}
	if(bind(TI->sd, (struct sockaddr *)&TI->server, sizeof(TI->server)) == SOCKET_ERROR)
	{
		exit(2);
	}
}

void ListenForConnections(TCPinfo *TI, int numOfConnection)
{
	listen(TI->sd, numOfConnection);
}

int UDPread(UDPinfo *UI)
{
	WSABUF UDPread;
	int bytesRecv;
	DWORD flags = 0;
	int err;

	memset((char*)UDPread.buf, 0, 512);
	UDPread.len = 512;


	if(WSARecvFrom(UI->sd, &UDPread, 1, NULL, &flags, NULL, NULL, &UI->overlapped, NULL) == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		if(err != ERROR_IO_PENDING)
		{
		}
	}
	/***************************

	Audo playing code goes here

	****************************/


	bytesRecv = sizeof(UDPread.buf);

	free(UDPread.buf);
	return bytesRecv;
}

int TCPread(TCPinfo *TI)
{
	WSABUF TCPread;
	int bytesToRead;
	DWORD flags;
	int err;

	memset((char*)TCPread.buf, 0, 512);
	TCPread.len = 512;

	if(WSARecv(TI->new_sd, &TCPread, 1, NULL, &flags, &(TI->overlapped), NULL) == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		if(err != ERROR_IO_PENDING)
		{
			//error during read
		}
	}


	bytesToRead = atoi(TCPread.buf);

	return bytesToRead;
}


void TCPconnect(TCPinfo *TI)
{
	if(connect (TI->sd, (struct sockaddr *)&TI->server, sizeof(TI->server)) == -1)
	{
		exit(4);
	}
}

int UDPsend(UDPinfo *UI, FILE *fp)
{
	WSABUF filecontents;
	DWORD byteread;
	DWORD flags;
/*
	while(fscanf)
	{
		sendto(UI->sd, filecontents.buf, byteread, flags, (struct sockaddr *)&UI->server, UI->server_len); 	
	}
	*/
	return 0;
}
int TCPsend(TCPinfo *TI, UDPinfo* UI, int filesize)
{
	WSABUF size;
	int err;

	size.len = 512;
	memset((char*)size.buf, 0, 512);

	sprintf(size.buf, "%d", filesize);

	err = send(TI->sd, size.buf, 512, NULL);

	return err;
}