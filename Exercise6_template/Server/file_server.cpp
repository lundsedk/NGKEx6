/* A simple server in the internet domain using TCP
The port number is passed as an argument 
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm 

Modified: Michael Alrøe
Extended to support file server!
*/

#include <file_server.h>
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
char readBuffer[READSIZE];
char writeBuffer[READSIZE] = "test";									//remove "test"?
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
		// structure
			// listen
			// getfilename
			// testfilename
				//send size 0
				//send size, send filedata

		connectedSocket = accept(inSocket, (struct sockaddr *) &clientAddress, &clientAddressLength);		// ?
		if (connectedSocket< 0)
			error("ERROR on accept");
		else
			printf("Accepted (accept) active socket nr. %d\n", connectedSocket);
			fflush(stdout);

			checkFileName();			//also readies the vars, image_size and image_data
			sendFile();

			printf("\nReturned from chechFileName");
			fflush(stdout);

			//after sending, dealloc image data


		//snprintf((char*)readBuffer, sizeof(readBuffer), "Received: %s",(char*)readBuffer);
			//virker ikke pt. - ser også farligt ud ifl. compiler

		char fileSize[20];
		sprintf(fileSize, "%ld", image_size);
		sendFile(connectedSocket, fileSize,strlen(fileSize));
		writeTextTCP(connectedSocket,fileSize);
		//write(connectedSocket,writeBuffer,strlen((char*)writeBuffer));
		//writeTextTCP(connectedSocket, fileSize);
		//writeTextTCP(connectedSocket,image_data);
		
		//send(connectedSocket, image_data, image_size, 0);
		
		//sendFile(connectedSocket, image_data, image_size);
		long bytesLeft = image_size;
		//writeTextTCP("%ld", image_size);
		
		

		//writeBuffer[image_size] = image_data[0];
		//writeTextTCP(connectedSocket, writeBuffer);

		

		while(bytesLeft > 0) 
		{
			//writeTextTCP()


			//write(inSocket, image_data +(image_size-bytesLeft), bytesLeft > 1000 ? 1000 : bytesLeft);
			//writeTextTCP(connectedSocket, writeBuffer);
			//sendFile(connectedSocket, image_data +(image_size-bytesLeft), bytesLeft > 1000 ? 1000 : bytesLeft);
			//send(connectedSocket, image_data +(image_size-bytesLeft), bytesLeft > 1000 ? 1000 : bytesLeft, 0);

			bytesLeft -= 1000;
		}

		
		
		close(connectedSocket);
	}
	close(inSocket);

	return 0; 
}



void checkFileName() {
	printf("\ncheckFileName start");
	fflush(stdout);

	bzero(readBuffer,sizeof(readBuffer));								//reset readBuffer

	readTextTCP(connectedSocket,readBuffer,sizeof(readBuffer));


	FILE* fp = fopen("readBuffer", "rb");
	if (fp == nullptr ) {
		printf("\n file: \"%s\" could not be found", readBuffer);
		image_size = 0;
		image_data = nullptr;
	} else {
		printf("\n reading file: \"%s\" ", readBuffer);
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
	char fileSizeC[255];
	snprintf( fileSizeC, 255, "%d", image_size );
	writeTextTCP(connectedSocket, fileSizeC);
	printf("\nsendFile: sent file size of %s", fileSizeC);

	//easier as loop - find the number of 1000-chunks, loop them, then do the remainder
		//division of ints is "throws away remainder" - so... easy to implement?
	//afterwards, send the remainder, then stop

	unsigned int kiloChunks = image_size / 1000;
	long unsigned int sendingIndex = 0;

	for (; sendingIndex <= kiloChunks ; ++sendingIndex ) {
		
	}
	//use sendingindex as start, use (image_size % 1000) as end


}


