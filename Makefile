all: server client
server:
	gcc server.c  -lpthread -o server
client:
	gcc client.c  -lpthread -o client
clean:
	rm *.o server client -rf
