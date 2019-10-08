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
    bzero((char*)&serverAddr, sizeof(serverAddr));      //needed to reset serverAddr on sonsecutive runs
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serverAddr.sin_port = htons(PORT); 

    //socket file descriptor
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) {
        cout << "Error creating server socket. Exiting..." << endl;
        exit(0);
    }

    //bind the socket
    int binding = bind(serverfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (binding == -1) {
        cout << "Error binding server socket. Exiting..." << endl;
        exit(0);
    }

    //start listening
    cout << "Listening for client connection..." << endl;
    listen(serverfd, MAX_CLIENTS);
    //accept the client connection
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        cout << "Error accepting client request: " << strerror(errno) << " Exiting..." << endl;
        exit(0);
    }
    cout << "Connected with client!" << endl; 


    //close socket
    close(serverfd);
}