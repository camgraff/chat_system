CC = g++
CFLAGS= -std=c++17 -Werror

server = server
client = client

all: $(server) $(client)

clean: 
	rm -f $(server) $(client)

run1: $(server)
	./$(server)

run2: $(client)
	./$(client)

$(server): $(server).cpp
	$(CC) $(CFLAGS) -o $(server) $(server).cpp

$(client): $(client).cpp
	$(CC) $(CFLAGS) -o $(client) $(client).cpp
