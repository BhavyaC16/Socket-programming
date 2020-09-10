CC = gcc
CFLAGS = -Wall

default:all
all:
	$(CC) $(CFLAGS) ./server/server.c -o ./server/server
	$(CC) $(CFLAGS) ./client/client.c -o ./client/client
clean:
	rm ./server/server ./client/client
