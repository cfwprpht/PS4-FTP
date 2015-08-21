#include <stdlib.h>

#include "kernel.h"
#include "network.h"

#define DATA (void *)0x926300000

#define buffer DATA
#define bufferSize 4096

int _main(void) {
	// Pass address of a syscall gadget in rcx
	register f rcx asm("rcx");
	directSyscall = rcx;
	
	
	// Load modules
	int libc;
	char libcName[] = "libSceLibcInternal.sprx";
	loadModule(libcName, &libc);
	
	int libNet;
	char libNetName[] = "libSceNet.sprx";
	loadModule(libNetName, &libNet);
	
	
	// Resolve functions
	void *(*memset)(void *destination, int value, size_t num);
	RESOLVE(libc, memset);
	
	void *(*memcpy)(void *destination, const void *source, size_t num);
	RESOLVE(libc, memcpy);
	
	char *(*strcpy)(char *destination, const char *source);
	RESOLVE(libc, strcpy);
	
	int (*sceNetSocket)(const char *, int, int, int);
	RESOLVE(libNet, sceNetSocket);
	
	int (*sceNetSocketClose)(int);
	RESOLVE(libNet, sceNetSocketClose);
	
	int (*sceNetConnect)(int, struct sockaddr_in *, int);
	RESOLVE(libNet, sceNetConnect);
	
	int (*sceNetSend)(int, const void *, size_t, int);
	RESOLVE(libNet, sceNetSend);
	
	int (*sceNetBind)(int, struct sockaddr_in *, int);
	RESOLVE(libNet, sceNetBind);
	
	int (*sceNetListen)(int, int);
	RESOLVE(libNet, sceNetListen);
	
	int (*sceNetAccept)(int, struct sockaddr_in *, int *);
	RESOLVE(libNet, sceNetAccept);
	
	int (*sceNetRecv)(int, void *, size_t, int);
	RESOLVE(libNet, sceNetRecv);
	
	
	// Start listening for requests
	char socketName[] = "FTP";
	
	int server;
	int client;
	
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	
	int clientLength;
	
	serverAddress.sin_family = htons(AF_INET);
	serverAddress.sin_addr.s_addr = IN_ADDR_ANY;
	serverAddress.sin_port = htons(9023);
	memset(serverAddress.sin_zero, 0, sizeof(serverAddress.sin_zero));
	
	server = sceNetSocket(socketName, AF_INET, SOCK_STREAM, 0);
	sceNetBind(server, &serverAddress, sizeof(serverAddress));
	sceNetListen(server, 10);
	
	while(1) {
		client = sceNetAccept(server, &clientAddress, &clientLength);
		sceNetRecv(client, buffer, bufferSize, 0);
		sceNetSend(client, buffer, bufferSize, 0);
		sceNetSocketClose(client);
	}
	
	// Return to browser
	sceNetSocketClose(server);
	
	return 0;
}
