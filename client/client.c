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

	socketID = socket(AF_INET, SOCK_STREAM, 0);
	if(socketID==-1){
		perror("Error in socket creation");
		exit(1);
	}
	printf("Socket created\n");

	if(argc!=2){
		printf("Please provide IP address as an argument to the client.\nFor example, for local server, execute ./client 127.0.0.1\nElse, specify the IP address of the sever: ./client 192.168.xxx.xxx\n");
		exit(0);
	}

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

	char file_found_status[2] = {0};
	recv(socketID, file_found_status, 2, 0);
	if(strcmp(file_found_status, "0")==0){
		printf("File not found in shared drive\nClosing client\n");
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

	fclose(fp);
	
	// char file_downloaded[2] = {0};
	// strcpy(file_downloaded, "Y");
	// send(socketID, file_downloaded, 2, 0);
	// char success_status[2] = {0};
	// recv(socketID, success_status, 2, 0);
	// if(strcmp(success_status, "1")==0){
	// 	printf("File downloaded successfully\n");
	// }
	// else{
	// 	printf("File could not be downloaded completely\n");
	// }

	printf("Closing connection\n");
	close(socketID);
}
