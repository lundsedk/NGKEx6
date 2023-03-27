
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
char fileName[READSIZE];
char sendBuffer[READSIZE] = "test";									//remove "test"?
int inSocket = socket(AF_INET, SOCK_STREAM, 0);
sockaddr_in serverAddress;
sockaddr_in clientAddress;
int portnumber;
int connectedSocket;
long image_size = 0;
char* image_data;


int main(int argc, char *argv[])
{
	printf("Starting server...\n");

	if (argc < 2) 
	{
		error("ERROR usage: port");
	}

	if(inSocket<0){ error("ERROR opening socket");}						//check initialization of global var
	bzero((char *) &serverAddress, sizeof(serverAddress)); //reset server address
	portnumber = atoi(argv[1]); //portnumber = first argument 
	serverAddress.sin_family = AF_INET; //IPv4
	serverAddress.sin_addr.s_addr = INADDR_ANY; //accept any message
	serverAddress.sin_port = htons(portnumber); //converts portnumber to network byte order
	if (bind(inSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) 
		{error("ERROR on binding");}

	printf("Listening for connection...\n");


	listen(inSocket,2); //max 2 pending connections. ready to accept incoming connections
	socklen_t clientAddressLength = sizeof(clientAddress);


	while(1) {

		connectedSocket = accept(inSocket, (struct sockaddr *) &clientAddress, &clientAddressLength);		// ?
		if (connectedSocket< 0)
			error("ERROR on accept");
		else
			printf("Accepted (accept) active socket nr. %d\n", connectedSocket);
			fflush(stdout);

			transferFile();			//also readies the vars, image_size and image_data
			sendFile();

			printf("\nReturned from chechFileName");
			fflush(stdout);

		long bytesLeft = image_size;
		

		while(bytesLeft > 0) 
		{
			bytesLeft -= 1000;
		}

		close(connectedSocket);
	}
	close(inSocket);

	return 0; 
}



void transferFile() {
	printf("\ncheckFileName start");
	fflush(stdout);

	bzero(fileName,sizeof(fileName));								//reset readBuffer / fileName
	readTextTCP(connectedSocket,fileName,sizeof(fileName));

	FILE* fp = fopen(fileName, "rb");
	if (fp == nullptr ) {
		printf("\n file: \"%s\" could not be found", fileName);
		image_size = -1;
		image_data = nullptr;
	} else {
		printf("\n reading file: \"%s\" ", fileName);
		fflush(stdout);

		fseek(fp, 0, SEEK_END);
		image_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		image_data = (char*) malloc(image_size);
		fread(image_data, 1, image_size, fp);
		fclose(fp);
	}

	printf("\ncheckFileName end");
	fflush(stdout);
};



void sendFile()
{
	//send image size, 0-term chars
	int n = 0;			//error handling
	char fileSizeC[255];
	snprintf( fileSizeC, 255, "%ld", image_size );
	writeTextTCP(connectedSocket, fileSizeC);
	printf("\nsendFile: sent file size of %s", fileSizeC);

	unsigned int kiloChunks = image_size / 1000;
	unsigned int remainderChunk = image_size % 1000;
	long unsigned int sendingIndex = 0;

	// Send the kilochunks	
	for (; sendingIndex < kiloChunks ; ++sendingIndex ) {

		memcpy(sendBuffer, image_data + (sendingIndex * 1000),1000);

		n = write(connectedSocket, sendBuffer, 1000);				//send
			if (n == -1){
				printf("\nwrite() returned -1");
			} else {
				printf("\nSending %ld out of %d packets - %d bytes of 1000 sent", sendingIndex + 1, kiloChunks, n);
			}
				fflush(stdout);

			//this seems to crash around packet 112-113... if 5000 microsecs wait or more...

		usleep(5);
	}

	// Send the remainder (note that sendingIndex is still the maximum that made it leave the loop, ie. kiloChunks)
	printf("\nSending remainder, package of %d bytes of %s", remainderChunk, fileName);
	memcpy(sendBuffer, image_data + (sendingIndex * 1000), remainderChunk);		//prep writebuffer
	n = write(connectedSocket, sendBuffer, remainderChunk);				//send (ignoring everything after reminderChunk index)
		printf("\nData as char in final writebuffer: %s", sendBuffer);

}


