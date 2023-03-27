/*
Ændret til genaflevering
Debug-statements fjernet (char-udskrift af modtagne bits)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h> 
#include "iknlib.h"
#include <string>

#define STRBUFSIZE 1000

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/**
 * @brief Receives a file from a server socket
 * @param serverSocket Socket stream to server
 * @param fileName Name of file. Might include path on server!
 */

void receiveFile(int serverSocket, const char* fileName, long fileSize)
{
	printf("Receiving: '%s', size: %li\n", fileName, fileSize);

}

int main(int argc, char *argv[])
{
	printf("Starting client...\n");

	if (argc < 3){
	    error( "ERROR usage: ""hostname"",  ""filename""\n");
	}
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[BUFSIZE];

	portno = PORT;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	    error("ERROR opening socket");

	server = gethostbyname(argv[1]);
	if (server == NULL) 
	    error("ERROR no such host");

	printf("Server at: %s, filename: %s\n",argv[1], argv[2]);

	printf("Connect...\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	    error("ERROR connecting");

	printf("writing file named: %s\n",argv[2]);
	char *newBuffer = argv[2];
	writeTextTCP(sockfd,newBuffer);

	
    bzero(buffer,sizeof(buffer));


	char AmountOfBytesToRecieve[7];
	readTextTCP(sockfd,AmountOfBytesToRecieve, 7);
	printf("bytesToRecieve %s\n", AmountOfBytesToRecieve);
	int BytesToRecieveInteger = std::stoi(AmountOfBytesToRecieve);
	printf("bytesToRecieve converted to integer %d\n", BytesToRecieveInteger);

	FILE* fd = fopen(argv[2], "wb");
	int i = 0;
	if (fd == NULL) {
		printf("error opening file\n");
	}

	while(BytesToRecieveInteger > 0) {
		i++;
		read(sockfd,buffer,1000);

		if (BytesToRecieveInteger > 1000)
		{
			fwrite(buffer, sizeof(char), 1000, fd);
		}
		else 
		{
			fwrite(buffer, sizeof(char), BytesToRecieveInteger, fd);
		}

		printf("\nReceiving chunk %d, %d bytes remaining", i, BytesToRecieveInteger);

		bzero(buffer,1000);
		BytesToRecieveInteger -=1000;
	}
	usleep(5000000);
	fclose(fd);

    printf("Closing client...\n\n");
	close(sockfd);
	return 0;
}






