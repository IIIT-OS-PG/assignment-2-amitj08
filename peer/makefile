#	2019201037 Amit Jindal
CC = g++ 
CFLAGS = -Wall
DEPS = clientconfig.h
OBJ = torrentCreator.o peer_as_client.o upload_file.o stop_share.o download_file.o peer_as_server.o
%.o: %.cpp $(DEPS)
		$(CC) $(CFLAGS) -c -o $@ $< -lssl -lcrypto -lpthread

peer: $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto -lpthread

clean:
	rm -rf *.o client
