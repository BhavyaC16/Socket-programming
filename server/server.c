#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include <libgen.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 9001
#define MAX_BUFF 1024

int main()
{
	int socketID;
	int client;
	struct sockaddr_in server;
	int address_length = sizeof(server);
	FILE* file;

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	socketID = socket(AF_INET, SOCK_STREAM, 0);
	if(socketID==-1){
		perror("Error in socket creation");
		exit(1);
	}
	printf("Socket created\n");

	int opt = 1;
	setsockopt(socketID, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	int bind_status = bind(socketID, (struct sockaddr *)&server, sizeof(server));
	if(bind_status!=0){
		perror("Error in socket binding");
		exit(1);
	}
	printf("Socket binded\n");

	int listening_status = listen(socketID, 0);
	if(listening_status!=0)
	{
		perror("Error in listening");
		exit(1);
	}
	printf("Server is ready\n");
	while(1)
	{
		client = accept(socketID, (struct sockaddr *)&server, (socklen_t *)&address_length);
		if(client<0){
			perror("Client connection failed");
			exit(1);
		}
		printf("Client connected\nWaiting for client to send filename\n");

		FILE* fp;

		int correct_filename = 0;
		while(correct_filename==0){
			char filenameRecd[1024] = {0};
			char file_path[1500];
			read(client, filenameRecd, 1024);
			strcpy(file_path, "../shared_drive/");
			strcat(file_path, filenameRecd);

			printf("Filename sent by client: %s\n", filenameRecd);
			printf("Path to file: %s\n", file_path);

			char file_found_status[2] = {0};
			fp = fopen(file_path, "rb");
			if(fp==NULL){
				strcpy(file_found_status, "0");
				send(client, file_found_status, 2, 0);
				printf("File not found in shared drive\n");
			}
			else{
				correct_filename = 1;
				strcpy(file_found_status, "1");
				send(client, file_found_status, 2, 0);
			}
		}

		printf("File opened\nStarting file transfer\n");
		char success_status[2] = {0};
		strcpy(success_status, "1");
		int x;
		char content[1025] = {0};
		x = fread(content, 1, MAX_BUFF, fp);
		while(x>0){
			int file_transfer_status = send(client, content, x, 0);
			if(file_transfer_status==-1){
				printf("Error in sending file");
				strcpy(success_status, "0");
				break;
			}
			memset(content, 0, MAX_BUFF);
			x = fread(content, 1, MAX_BUFF, fp);
		}

		fclose(fp);

		//char client_ack[2] = {0};
		//read(client, client_ack, 2);
		//printf("%s\n", client_ack);
		//send(client, success_status, 2, 0);
		//printf("send status\n");
		//if(strcmp(success_status, "1")==0){
		//	printf("File sent to client\nClosing connection");
		//}
		//else{
		//	printf("Encountered error while sending file\nClosing connection\n");
		//}
		close(client);
	}

	printf("Closing server\n");
	close(socketID);
}
