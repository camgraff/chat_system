#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
using namespace std;

//port number
#define PORT 8888

//maximum number of clients that can simultaeously connect to the server
#define MAX_CLIENTS 4

int main(int argc, char* argv[]) {
    char buffer[1024] = {0};

    //socket setup
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(PORT); 

    //socket file descriptor
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);

    //bind the socket
    bind(serverfd, (sockaddr*)&serverAddr, sizeof(serverAddr));

    //start listening
    cout << "Listening for client connection..." << endl;
    listen(serverfd, MAX_CLIENTS);
    //accept the client connection
    int newSocket = accept(serverfd, (sockaddr*)&serverAddr, (socklen_t*) sizeof(serverAddr));
    if (newSocket == -1) {
        cout << "Error accepting client request. Exiting..." << endl;
        exit(0);
    } else {
        cout << "Connected with client!" << endl; 
    }
    
}