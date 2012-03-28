#include "UDPfunctions.h"


/* creates TCP and UDP sockets */
void CreateSocket(SOCKET *sd, int protocol)
{
	if(protocol == UDP)
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
	memset((char *)&UI->server, 0, sizeof(UI->server));
	UI->server.sin_family = PF_INET;
	UI->server.sin_port = htons(UI->port);

	memset((char *)&TI->server, 0, sizeof(TI->server));
	TI->server.sin_family = AF_INET;
	TI->server.sin_port = htons(UI->port);

	if(type == TCP)
	{
		UI->server.sin_addr.s_addr = htonl(INADDR_ANY);
		TI->server.sin_addr.s_addr = htonl(INADDR_ANY);
	}
}

/* so when this function gets called, we get host twice? */
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

	memset((char*)UDPread.buf, 0, BUFSIZE);
	UDPread.len = BUFSIZE;


	if(WSARecvFrom(UI->sd, &UDPread, 1, NULL, &flags, NULL, NULL, &UI->overlapped, NULL) == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		if(err != ERROR_IO_PENDING)
		{
			// error during read
		}
	}
	

	/*
	* THIS IS NOT CORRECT NEED TO FIND RIGHT PLAYSOUND
	*/
	PlaySound(UDPread.buf, NULL, SND_MEMORY);

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

	memset((char*)TCPread.buf, 0, BUFSIZE);
	TCPread.len = BUFSIZE;

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
// reads a wav file, sends packetized buffers to the server, 
int UDPsend(UDPinfo *UI)
{
	WSABUF filecontents;
	DWORD bytetoSend;
	DWORD flags;
	FILE *fp;
	int byteSent;
	int bufpos = 0;
	int numpackets;

	fp = wavOpen(&UI->WH);

	filecontents.buf = (char *) malloc (BUFSIZE);
	filecontents.buf = UI->WH.soundBuffer;
	filecontents.len = BUFSIZE;
	numpackets = (UI->WH.dataSize / BUFSIZE);
	bytetoSend = (numpackets * SOUND_DATA_SIZE)+(numpackets * 44);// do math for this ...soundbuffer/512 then add header to each of those

	while(bytetoSend > byteSent)
	{
		filecontents.buf = packetize(&UI->WH, &bufpos);
		byteSent += sendto(UI->sd, filecontents.buf, BUFSIZE, flags, (struct sockaddr *)&UI->server, UI->server_len); 
		
	}
	return 0;
}


int TCPcontrolsend(TCPinfo *TI, UDPinfo* UI, int filesize)
{
	WSABUF size;
	int err;

	size.len = BUFSIZE;
	memset((char*)size.buf, 0, BUFSIZE);

	sprintf(size.buf, "%d", filesize);

	err = send(TI->sd, size.buf, BUFSIZE, NULL);

	return err;
}


/*
* opens a wave file and extracts all of its data
*/
FILE *wavOpen(wavheader *WH)
{
	FILE *fp;
	DWORD size;
	char id[4];
	short formatTag, channels, blockAlign, bitsPerSample; 
	DWORD formatLength, sampleRate, avgBytesSec, dataSize, i; 
	char *soundBuffer;


	fp = fopen("sound.wav", "rb");
	if(fp)
	{
		fread(id,sizeof(BYTE),4,fp); 
		if (!strcmp(id,"RIFF")) 
		{ 
			//Windows media file
			fread(&size,sizeof(DWORD),1,fp); //size of file
			fread(id,sizeof(BYTE),4,fp); //reading "WAVE"
			if(!strcmp(id, "WAVE"))
			{
				//.WAV file
				fread(id, sizeof(BYTE), 4, fp); // reading "fmt " 
				fread(&formatLength, sizeof(DWORD),1,fp); //16 bytes for file format
				fread(&formatTag, sizeof(short), 1, fp); // 16 bits. 1 means no compression, other is diff format 
				fread(&channels, sizeof(short),1,fp); //16 bits, number of channels - 1 mono, 2 stereo 
				fread(&sampleRate, sizeof(DWORD), 1, fp); //16 bits for sample rate
				fread(&avgBytesSec, sizeof(short), 1, fp); // 32 bits for average bytes per second
				fread(&blockAlign, sizeof(short), 1, fp); //16 bits for block alignment 
				fread(&bitsPerSample, sizeof(short), 1, fp); //16 bits for bits per sample (8 or 16 bit sound)
				fread(id, sizeof(BYTE), 4, fp); //read in that data is next 
				fread(&dataSize, sizeof(DWORD), 1, fp); //how many bytes of sound 
				soundBuffer = (char *) malloc (sizeof(char) * dataSize); //set aside sound buffer space 
				fread(soundBuffer, sizeof(BYTE), dataSize, fp); //read in all sound data

			}
			else
			{
				perror("not a .WAV file");
			}
		} 
		else
		{
			perror("not a RIFF file");
		}
	}

	WH->avgBytesSec = avgBytesSec;
	WH->bitsPerSample = bitsPerSample;
	WH->blockAlign = blockAlign;
	WH->channels = channels;
	WH->dataSize = dataSize;
	WH->formatLength = formatLength;
	WH->formatTag = formatTag;
	WH->size = size;
	WH->soundBuffer = (char *) malloc (sizeof(char) * dataSize);
	WH->soundBuffer = soundBuffer;
	return fp;
}
//takes some of the sound data and adds a header to it so that the server can play it
char *packetize(wavheader *WH, int *bufpos)
{
	char *buf;
	char *soundData;
	int i = 0;
	int j = 0;

	buf = (char *) malloc(BUFSIZE);
	soundData = (char *) malloc(SOUND_DATA_SIZE);



	for(i = *bufpos; i < (*bufpos + SOUND_DATA_SIZE); i++)
	{
		soundData[j] = WH->soundBuffer[i];
		j++;
	}

	sprintf(buf,"%s %d %s %s %d %d %d %d %d %d %d %s %s","RIFF", 
			WH->size, "WAVE", "fmt ", WH->formatLength, WH->formatTag,
			WH->channels, WH->sampleRate, WH->avgBytesSec, WH->blockAlign,
			WH->bitsPerSample, 'data', soundData);

	*bufpos += SOUND_DATA_SIZE;
	return buf;
}
	


