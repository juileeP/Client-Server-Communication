#include "stdafx.h"
#define _WINDOWS // comment this line out for linux
#ifdef _WINDOWS
#ifndef WIN32_LEAN_AND_MEAN   
#define WIN32_LEAN_AND_MEAN   
#endif   
#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>
#ifndef socket_t 
#define socklen_t int
#endif   
#pragma comment(lib, "iphlpapi.lib")  
#pragma comment(lib, "ws2_32.lib")
// #pragma comment(lib, "ws2.lib")   // for windows mobile 6.5 and 
#include <errno.h> 
#include <sys/types.h> 
#ifndef SOCKET 
#define  SOCKET int
#endif   
#endif    
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>   
#include <time.h>   



void main( int argc, char *argv[])
{
	if (argc!=2)
	{
		printf("usage: udpTest 1 for server and udpTest 0 for client\n");
		exit(0);
	}
	int	serverOrClient = atoi(argv[1]);
	bool isServer;
	if (serverOrClient==1)
	{
		printf("running as server \n");
		isServer = true;
	}
	else
	{
		printf("running as client\n");
	isServer = false;
	}

#ifdef _WINDOWS
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) 
	{
		printf("WSAStartup failed: %d\n", iResult);
		exit(-1);
	} 
#endif

SOCKET UDPSock;
UDPSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  /* IPPROTO_UDP - so this will be a UDP Socket*/ 

int UDPPort;
if (isServer){
			UDPPort = 10000;
			printf("Running as server - Author : Juilee Patankar\n");
		}
		else
		{
			UDPPort = 10001;
			printf("Running as client - Author : Juilee Patankar\n");
		}



/* set up socket */
struct sockaddr_in My;
memset(&My,0,sizeof(My));                  // clear memory
My.sin_family = AF_INET;		           // address family. must be this for ipv4, or AF_INET6 for ipv6
My.sin_addr.s_addr = htonl(INADDR_ANY);    // allows socket to work send and receive on any of the machines interfaces (which machine is used to send?)
My.sin_port = htons(UDPPort);		       // the port on this host

/* BIND THE SOCKET TO Port  */
int ret=bind(UDPSock, (struct sockaddr *)&My, sizeof(struct sockaddr));  // ask OS to let us use the socket (why might it say we cannot

printf("bind returned %d if not zero, then there was a problem\n",ret);

if(!isServer) 
{
char ToIPAddress[80];
sprintf(ToIPAddress,"127.0.0.1");  // loop back address (the pkt will come back to this host)
int WriteToPort = 10000; // the client will write to 10000
struct sockaddr_in to;
memset(&to,0,sizeof(to));
to.sin_addr.s_addr = inet_addr(ToIPAddress);     // we specifed the address as a string, inet_addr translates to a number in the correct byte order
to.sin_family = AF_INET;                         // ipv4
to.sin_port = htons(WriteToPort);                // set port address (is this the sender's port or the receiver's port

/* make message */
char pkt[300];
sprintf(pkt,"hello there\n");

/* send message */
ret = sendto(UDPSock, (char *)pkt, sizeof(pkt), 0, (struct sockaddr *)&to, sizeof(struct sockaddr));
printf("sendto returned %d it should be   the number of bytes sent \n",ret);

/* wait for return message */
fd_set readFd;
struct timeval SelectTime;
SelectTime.tv_sec = 2; /* set timeout to 2 seconds. If no messages arrives in 2 seconds, then give up */
SelectTime.tv_usec=0;
FD_ZERO(&readFd);
FD_SET(UDPSock,&readFd);
ret = select(255,&readFd,NULL,NULL,&SelectTime); /* wait for message */
if (FD_ISSET(UDPSock, &readFd)==1) 
{	/* is message arrived before timeout */
ret = recv(UDPSock, (char *)pkt, 300,0);
if (ret>0) 
{
printf("received:  %s",pkt);
}
else
{
   #ifdef _WINDOWS
   if (WSAECONNRESET==WSAGetLastError())
   printf("possibly sent a packet to an unopened port\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
   else 
                    printf("revfrom error %d %d\n",ret, WSAGetLastError());
                    #endif
          }     
		}
		else
		{	/* if message failed to arrive before timeout */
			printf("no response from %s\n",ToIPAddress);
		}


		}

		else
		{
		char buf[300];
		struct sockaddr_in from;
		int len = sizeof(struct sockaddr);


		/* wait for message. This will wait forever for a message. If a timeout is needed, then use Select, as shown above */
		ret = recvfrom(UDPSock, (char *)buf, 300, 0,(struct sockaddr *)&from,(socklen_t *)&len);
		printf("recv returned %d\n",ret);
		printf("received data from %s from port %d\n",inet_ntoa(from.sin_addr),ntohs(from.sin_port));

		/* make message to send */
		sprintf(buf,"go away\n");
		printf("%s",buf);
	
		/* send message */
		struct sockaddr_in to;
		memset(&to,0,sizeof(to));
		to.sin_addr.s_addr = from.sin_addr.s_addr;
		to.sin_family = AF_INET;
		to.sin_port = from.sin_port; // set port address
		ret = sendto(UDPSock, (char *)buf, 300, 0, (struct sockaddr *)&to, sizeof(struct sockaddr));
		printf("sendto returned %d\n",ret);
 }

}
