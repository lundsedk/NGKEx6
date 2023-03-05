/* A simple server in the internet domain using TCP
The port number is passed as an argument 
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm 

Modified: Michael Alrøe
Extended to support file server!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "iknlib.h"

#define STRBUFSIZE 256

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
void sendFile(int clientSocket, const char* fileName, long fileSize)
{
	printf("Sending: %s, size: %li\n", fileName, fileSize);
   
}

int readSize = 1000;

int main(int argc, char *argv[])
{

	printf("Starting server...\n");

	if (argc < 2) 
	{
		error("ERROR usage: port");
	}

	char readText[readSize];
	int inSocket = socket(AF_INET, SOCK_STREAM,0);
	//	AF_INIT for IPv4, SOCK_STREAM for TCP, 0 for OS to chose protocol
	if(inSocket<0){ error("ERROR opening socket");}

	sockaddr_in serverAddress;
	sockaddr_in clientAddress;
	int portnumber;


	//readTextTCP(inSocket, readText, readSize);

	bzero((char *) &serverAddress, sizeof(serverAddress)); //reset server address
	portnumber = atoi(argv[1]); //portnumber = first argument 
	serverAddress.sin_family = AF_INET; //IPv4
	serverAddress.sin_addr.s_addr = INADDR_ANY; //accept any message
	serverAddress.sin_port = htons(portnumber); //converts portnumber to network byte order
	if (bind(inSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) 
		{error("ERROR on binding");}

	printf("Listen...\n");
	listen(inSocket,2); //max 2 pending connections. ready to accept incoming connections
	int connectedSocket;
	socklen_t clientAddressLength = sizeof(clientAddress);

	char readBuffer[readSize];
	char writeBuffer[readSize] = "test";
	
	

	bzero(readBuffer,sizeof(readBuffer));//reset readBuffer


	FILE* fp = fopen("donkey.jpg", "rb");
    fseek(fp, 0, SEEK_END);
    long image_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* image_data = (char*) malloc(image_size);
    fread(image_data, 1, image_size, fp);
    fclose(fp);

	//bzero(readBuffer,sizeof(readBuffer));//reset readBuffer

	while(1) 
	{

		printf("Accept...\n");
		connectedSocket = accept(inSocket, (struct sockaddr *) &clientAddress, &clientAddressLength);
		if (connectedSocket< 0) error("ERROR on accept");
		else printf("Accepted\n");

		readTextTCP(connectedSocket,readBuffer,sizeof(readBuffer)); //read 2nd argument from client which is file name
		int compare = strncmp(readBuffer, "donkey.jpg", 11); //compare file asked for with our servers file
		if (compare == 0) {
			printf("sending donkey picture\n");
		}
		else {
			printf("server doesn't have the picture called: %s\n", readBuffer);
				
		}


			//bzero(readBuffer,sizeof(readBuffer));//reset readBuffer
			//read(connectedSocket,readBuffer,sizeof(readBuffer));
			//readTextTCP(inSocket,readBuffer,sizeof(readBuffer));

			snprintf((char*)readBuffer, sizeof(readBuffer), "Recieved: %s\n",(char*)readBuffer);
			printf("%s\n",readBuffer);
			char fileSize[20];

			
			sprintf(fileSize, "%ld", image_size);
			sendFile(connectedSocket, fileSize,strlen(fileSize));
			writeTextTCP(connectedSocket,fileSize);
		
			long bytesLeft = image_size;

			while(bytesLeft > 0) //write until empty. short sleep to make sure reader can react
			{

				write(connectedSocket, image_data +(image_size-bytesLeft), bytesLeft > 1000 ? 1000 : bytesLeft);
				usleep(5);
				
				bytesLeft -= 1000;
			}

			
			
			close(connectedSocket);
	}
	close(inSocket);

	
	return 0; 
}