#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <set>
using namespace std;

//port number
#define PORT 8888

//maximum number of clients that can simultaeously connect to the server
#define MAX_CLIENTS 4

int main(int argc, char* argv[]) {
    //message buffer
    char buffer[1024] = {0};

    //set of client sockets
    set<int> clientSockets;       

    //set of client socket descriptors (used with select())
    fd_set clientSds;

    //socket to recieve client connections
    int recvSocket = socket(AF_INET, SOCK_STREAM, 0);
    //use this option for testing so we can reuse the same socket quickly
    int optval = true;
    setsockopt(recvSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));

    //socket setup
    sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));      //need to reset serverAddr on sonsecutive runs
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serverAddr.sin_port = htons(PORT); 

    //bind the socket
    //int binding = bind(recvSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bind(recvSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Error binding server socket [" << strerror(errno) << "]. Exiting..." << endl;
        exit(0);
    } 

    //start listening
    cout << "Listening for client connection..." << endl;
    listen(recvSocket, MAX_CLIENTS);

    while (1) {
        //clear the buffer
        memset(&buffer, 0, sizeof(buffer));
        //clear socket descriptors
        FD_ZERO(&clientSds);
        //add sockets to set of socket descriptors
        FD_SET(recvSocket, &clientSds);
        int maxSd = recvSocket;      //largest file descriptor
        for (int cli : clientSockets) {
            //add active sockets to the set of socket descriptors
            FD_SET(cli, &clientSds);
            maxSd = max(maxSd, cli);
        }

        //wait for client socket activity
        select(maxSd+1, &clientSds, NULL, NULL, NULL);

        //if activity occurred on recvSocket, then accept the connection
        if (FD_ISSET(recvSocket, &clientSds)) {
            if (clientSockets.size() == MAX_CLIENTS) {
                cout << "Maximum number of clients has been reached." << endl;
            } else {
                int addressLen = sizeof(serverAddr);
                int newSocket = accept(recvSocket, (struct sockaddr*)&serverAddr, (socklen_t*)&addressLen);
                if (newSocket < 0) {
                    cout << newSocket <<endl;
                    cout << "Error accepting client request [" << strerror(errno) << "]. Exiting..." << endl;
                    exit(0);
                } else {
                    //recieve userID from client
                    recv(newSocket, &buffer, sizeof(buffer), 0);
                    string temp(buffer);
                    strcpy(buffer, (temp + " connected.").c_str());
                    cout << buffer << endl; 
                }

                //push new client socket to set of sockets
                clientSockets.emplace(newSocket);

                //send new client info to all other active clients 
                for (int cli : clientSockets) {
                    send(cli, &buffer, strlen(buffer), 0);
                }
            }
        } else {
            //otherwise recieve the client message
            for (int cli : clientSockets) {
                if (FD_ISSET(cli, &clientSds)) {
                    //clear the buffer
                    memset(&buffer, 0, sizeof(buffer));
                    recv(cli, &buffer, sizeof(buffer), 0);
                    cout << buffer << endl;
                    //send this message to the other clients
                    for (int other : clientSockets) {
                        if (cli != other) 
                            send(other, &buffer, strlen(buffer), 0);
                    }
                }
            }
        }
    }


    //close socket
    close(recvSocket);
}