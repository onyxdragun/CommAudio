#include "UDPfunctions.h"


/* creates TCP and UDP sockets */
void CreateSocket(SOCKET *sd, int protocol)
{
	if(protocol == UDP)
	{
		if((*sd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
		{
			fprintf(stdout, "error opening socket");
		}
	}
	else
	{
		if((*sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			fprintf(stdout, "error opening socket");
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
		fprintf(stdout, "error in hostname");
		exit(16);
	}
	if((TI->hp = gethostbyname(UI->hostname)) == NULL)
	{
		fprintf(stdout, "error in hostname");
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
		fprintf(stdout, "bind fail");
		exit(2);
	}
	if(bind(TI->sd, (struct sockaddr *)&TI->server, sizeof(TI->server)) == SOCKET_ERROR)
	{
		fprintf(stdout, "bind fail");
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

	UDPread.buf = (char *) malloc (BUFSIZE);
	UDPread.len = BUFSIZE;


	if(recvfrom(UI->sd, UDPread.buf, BUFSIZE, flags,NULL,NULL) == SOCKET_ERROR)
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
	DWORD flags= 0;
	int err;

	TCPread.buf = (char *) malloc (BUFSIZE);
	TCPread.len = BUFSIZE;

	if(recv(TI->new_sd, TCPread.buf,BUFSIZE, flags) == SOCKET_ERROR)
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
int UDPsend(UDPinfo *UI, TCPinfo *TI)
{
	WSABUF filecontents;
	DWORD bytetoSend;
	DWORD flags = 0;
	FILE *fp;
	int byteSent = 0;
	int bufpos = 0;
	int numpackets;


	memset((void *)&UI->WH, 0, sizeof(wavheader));

	fp = wavOpen(&UI->WH);

	filecontents.buf = (char *) malloc (BUFSIZE);
	filecontents.buf = UI->WH.soundBuffer;
	filecontents.len = BUFSIZE;
	numpackets = (UI->WH.dataSize / BUFSIZE);
	bytetoSend = (numpackets * SOUND_DATA_SIZE)+(numpackets * 44);// do math for this ...soundbuffer/512 then add header to each of those

	TCPcontrolsend(TI, UI, UI->WH.size);

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
	size.buf = (char *) malloc (BUFSIZE);

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
	BYTE id[4];
	char *id2;
	int succ;
	short formatTag = 0;
	short channels = 0;
	short blockAlign;
	short bitsPerSample = 0 ; 
	DWORD formatLength = 0;
	DWORD dataSize = 0;
	DWORD i  = 0;
	DWORD sampleRate = 0;
	DWORD avgBytesSec = 0;
	char *soundBuffer;

	id2 = (char *) malloc (4);
	fp = fopen("sound.wav", "rb");
	if(fp)
	{
		fread(id2,sizeof(BYTE),4,fp);
		if(id2[0] == 'R')
			if(id2[1] == 'I')
				if(id2[2] == 'F')
					if(id2[3] == 'F')
					{ 
						//Windows media file
						fread(&size,sizeof(DWORD),1,fp); //size of file
						fread(id2,sizeof(BYTE),4,fp); //reading "WAVE"
						if(id2[0] == 'W')
							if(id2[1] == 'A')
								if(id2[2] == 'V')
									if(id2[3] == 'E')
									{ 
										//.WAV file
										fread(id2, sizeof(BYTE), 4, fp); // reading "fmt " 
										if(id2[0] == 'f')
											if(id2[1] == 'm')
												if(id2[2] == 't')
													if(id2[3] == ' ')
													{ 
														fread(&formatLength, sizeof(DWORD),1,fp); //16 bytes for file format
														fread(&formatTag, sizeof(short), 1, fp); // 16 bits. 1 means no compression, other is diff format 
														fread(&channels, sizeof(short),1,fp); //16 bits, number of channels - 1 mono, 2 stereo 
														fread(&sampleRate, sizeof(DWORD), 1, fp); //16 bits for sample rate
														fread(&avgBytesSec, sizeof(DWORD), 1, fp); // 32 bits for average bytes per second
														fread(&blockAlign, sizeof(short), 1, fp); //16 bits for block alignment 
														fread(&bitsPerSample, sizeof(short), 1, fp); //16 bits for bits per sample (8 or 16 bit sound)
														fread(id2, sizeof(BYTE), 4, fp); //read in that data is next 
														if(id2[0] == 'd')
															if(id2[1] == 'a')
																if(id2[2] == 't')
																	if(id2[3] == 'a')
																	{ 
																		fread(&dataSize, sizeof(DWORD), 1, fp); //how many bytes of sound 
																		soundBuffer = (char *) malloc (sizeof(char) * dataSize); //set aside sound buffer space 
																		fread(soundBuffer, sizeof(BYTE), dataSize, fp); //read in all sound data
																	}
													}

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

	sprintf(buf,"%s%d%s%s%d%d%d%d%d%d%d%s%s","RIFF", 
			WH->size, "WAVE", "fmt ", WH->formatLength, WH->formatTag,
			WH->channels, WH->sampleRate, WH->avgBytesSec, WH->blockAlign,
			WH->bitsPerSample, "data", soundData);

	*bufpos += SOUND_DATA_SIZE;
	return buf;
}
	


void TCPaccept(TCPinfo *TI)
{
	if((TI->new_sd = accept (TI->sd, NULL, NULL)) == INVALID_SOCKET)
	{
		fprintf(stdout, "accept failed");
		exit(3);
	}
	fprintf(stdout, "connected");
}