#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 9001
#define MAX_BUFF 1024

int main(int argc, char *argv[])
{
	int socketID;
	struct sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	//server.sin_addr.s_addr = INADDR_ANY;

	socketID = socket(AF_INET, SOCK_STREAM, 0);
	if(socketID==-1){
		perror("Error in socket creation");
		exit(1);
	}
	printf("Socket created\n");

	if(inet_pton(AF_INET, argv[1], &server.sin_addr)<=0)
    {
        perror("Error in inet_pton");
        exit(1);
    }

	int connection_status = connect(socketID, (struct sockaddr *)&server, sizeof(server));
	if(connection_status!=0){
		perror("Error in connecting to server");
		exit(1);
	}
	printf("Connected to server\n");

	char filenameToSend[1024] = {0};
	printf("Enter the filename: ");
	scanf("%s", filenameToSend);

	char file_path[1500];
	strcpy(file_path, "../local_drive/");
	strcat(file_path, filenameToSend);

	int filename_transfer_status = send(socketID, filenameToSend, strlen(filenameToSend), 0);
	if(filename_transfer_status==-1){
		perror("Error in sending filename to server");
		exit(1);
	}
	printf("Filename sent\n");

	char file_found_status[20] = {0};
	read(socketID, file_found_status, 20);
	if(strcmp(file_found_status, "File_not_found")==0){
		printf("File not found in shared drive\n Closing client\n");
		close(socketID);
		exit(1);
	}

	FILE* fp = fopen(file_path, "wb");
	if(fp==NULL){
		perror("Cannot download file");
		exit(1);
	}

	int x;
	char content[1025] = {0};
	x = recv(socketID, content, MAX_BUFF, 0);
	while(x>0){
		int file_write_status = fwrite(content, 1, x, fp);
		if(file_write_status!=x){
			perror("Error in downloading file");
			exit(1);
		}
		memset(content, 0, MAX_BUFF);
		x = recv(socketID, content, MAX_BUFF, 0);
	}

	printf("File downloaded\n");
	fclose(fp);
	//Send filename and Download file from server.

	printf("Closing connection\n");
	close(socketID);
}
