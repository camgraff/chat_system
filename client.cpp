#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 
using namespace std;

//port number
#define PORT 8888

int main(int argc, char* argv[]) {
    //socket setup
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; 
    //serverAddr.sin_addr.s_addr = inet_addr(serverAddr);
    serverAddr.sin_port = htons(PORT); 
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    //connect to the server
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cout << "Failed to connect to server. Exiting..." << endl;
        exit(0);
    } else {
        cout << "Connected to server!" <<endl;
    }

    close(sock);
}