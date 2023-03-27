//	Genaflevering, vigtige ændringer markeret med "fix" i kommentarer


#include "file_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "iknlib.h"

#define READSIZE 1000

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/**
 * @brief Sends a file to a client socket
 * @param clientSocket Socket stream to client
 * @param fileName Name of file to be sent to client
 * @param fileSize Size of file
 */

//Global variables
FILE* fp;																// Fix: åbning og læsning af fil er i to funktioner, bruger global var til filnavn
char fileName[READSIZE];
int inSocket = socket(AF_INET, SOCK_STREAM, 0);
sockaddr_in serverAddress;
sockaddr_in clientAddress;
int portnumber;
int connectedSocket;
long image_size = 0;


int main(int argc, char *argv[])
{
	printf("Starting server...\n");

	if (argc < 2) 
	{
		error("ERROR usage: port");
	}

	if(inSocket<0){ error("ERROR opening socket");}						//check initialization of global var
	bzero((char *) &serverAddress, sizeof(serverAddress));				//reset server address
	portnumber = atoi(argv[1]);											//portnumber = first argument 
	serverAddress.sin_family = AF_INET;									//IPv4
	serverAddress.sin_addr.s_addr = INADDR_ANY;							//accept any message
	serverAddress.sin_port = htons(portnumber);							//converts portnumber to network byte order
	if (bind(inSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) 
		{error("ERROR on binding");}

	listen(inSocket,2); //max 2 pending connections. ready to accept incoming connections
	socklen_t clientAddressLength = sizeof(clientAddress);



	while(1) {
		connectedSocket = accept(inSocket, (struct sockaddr *) &clientAddress, &clientAddressLength);		// ?
		if (connectedSocket< 0) {
			error("ERROR on accept");
		} else {
			printf("Accepted (accept) active socket nr. %d\n", connectedSocket);
			fflush(stdout);
			transferFile();			
		}

		close(connectedSocket);
		fclose(fp);
	}

	close(inSocket);

	return 0; 
}

void transferFile() {												// Fix: ændret navn og funktion - transferFile håndterer:
																	//	Åbning/lukning af fil, sender fil-størrelse, og loop-logikken
	unsigned int kiloChunks;
	unsigned int remainderChunk;
	long unsigned int sendingIndex;
	int n = 0;			//error handling

	// Resetting file name, opening file
	bzero(fileName,sizeof(fileName));								
	readTextTCP(connectedSocket,fileName,sizeof(fileName));			

	fp = fopen(fileName, "rb");			
	if (fp == nullptr ) {
		printf("\n file: \"%s\" could not be found", fileName);
		fflush(stdout);
		image_size = -1;
		return;
	}

	// Initialize for the given file:
	fseek(fp, 0, SEEK_END);
	image_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	kiloChunks = image_size / 1000;								// How many chunks of 1000
	remainderChunk = image_size % 1000;							// Size of the remaining chunk (less than 1000)

	// Sending file size
	char fileSizeC[255];
	snprintf( fileSizeC, 255, "%ld", image_size );
	writeTextTCP(connectedSocket, fileSizeC);
	printf("\nsendFile: sent file size of %s", fileSizeC);

	// Sending file
	sendingIndex = 0;
	for (; sendingIndex < kiloChunks ; ++sendingIndex ) {
		printf("\nSending %ld out of %d packets", sendingIndex + 1, kiloChunks+1);
		transferChunk(1000);
	}
	transferChunk(remainderChunk);
		printf("\nSending %ld out of %d packets", sendingIndex + 1, kiloChunks+1);
};

void transferChunk(unsigned int chunkSize) {					// Sender 1000 (eller mindre) bytes, flytter fp offset
	char sendBuffer[1000];
	int n = 0;													// error handling

	fread(sendBuffer, 1, chunkSize, fp);						// Read file, move fp offset

	n = write(connectedSocket, sendBuffer, chunkSize);			// Send
	if (n == -1){
		printf("\nError on write, write() returned -1");
		fflush(stdout);
	}
}
