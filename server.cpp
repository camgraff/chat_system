#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <future>
#include <set>
#include <map>
using namespace std;

//port number
//#define PORT 8000

//maximum number of clients that can simultaeously connect to the server
#define MAX_CLIENTS 4

//message buffer
char buffer[1024] = {0};

//set of client sockets
set<int> clientSockets;

//port number
int PORT = 8888;

void waitForTermination();

int main(int argc, char* argv[]) {
    if (argc == 2) PORT = atoi(argv[1]);
    else if (argc == 1) PORT = 8000;
    else {
        cout << "To start the server: ./server [PORT]\nPORT is the port number that the server will run on.\nWhen not specified PORT defaults to 8000." << endl;
        exit(0);
    }

    //map of client sockets to user IDs
    map<int, string> userIDs;      

    //set of client socket descriptors (used with select())
    fd_set clientSds;

    //socket to recieve client connections
    int recvSocket = socket(AF_INET, SOCK_STREAM, 0);
    int optval = true;
    //use this socket option for testing so we can reuse the same socket
    setsockopt(recvSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));

    //socket setup
    sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));      //need to reset serverAddr on consecutive runs
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serverAddr.sin_port = htons(PORT); 

    //bind the socket
    if (bind(recvSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Error binding server socket [" << strerror(errno) << "]. Exiting..." << endl;
        exit(0);
    } 

    //start listening
    cout << "Server is online! Use ctrl+d to terminate.\nListening for client connection..." << endl;
    listen(recvSocket, MAX_CLIENTS);

    //asynchronous call for server termination signal
    future<void> fut = async(waitForTermination);

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
            int addressLen = sizeof(serverAddr);
            int newSocket = accept(recvSocket, (struct sockaddr*)&serverAddr, (socklen_t*)&addressLen);
            //if already have max number of clients, tell the connecting client, then close the connection
            if (clientSockets.size() == MAX_CLIENTS) {
                strcpy(buffer, "Cannot connect another client. Maximum number of connections has been reached.");
                send(newSocket, &buffer, strlen(buffer), 0);
                close(newSocket);
                cout << buffer << endl;
            } else {
                if (newSocket < 0) {
                    cout << newSocket <<endl;
                    cout << "Error accepting client request [" << strerror(errno) << "]. Exiting..." << endl;
                    exit(0);
                } else {
                    //recieve userID from client
                    recv(newSocket, &buffer, sizeof(buffer), 0);
                    string userID(buffer);

                    strcpy(buffer, (userID + " connected.").c_str());
                    cout << buffer << endl; 

                    //if this is the 1st client, tell them to wait until another client joins
                    if (clientSockets.empty()) {
                        //clear the buffer
                        memset(&buffer, 0, sizeof(buffer));
                        strcpy(buffer, "No other users online. Please wait for another user to join.");
                        send(newSocket, &buffer, strlen(buffer), 0);
                    }
                    //else send new client info to all other active clients 
                     else {
                         for (int cli : clientSockets) {
                            send(cli, &buffer, strlen(buffer), 0);
                        }
                    }

                    //push new client socket to set of sockets
                    clientSockets.emplace(newSocket);

                    //add user ID to map
                    userIDs.emplace(newSocket, userID);
                }
            }
        }  
        //otherwise recieve the client message
        else {
            for (int cli : clientSockets) {
                if (FD_ISSET(cli, &clientSds)) {

                    //clear the buffer
                    memset(&buffer, 0, sizeof(buffer));

                    //if recv returns 0, client has left the room, so close the socket and remove from set and map
                    if (recv(cli, &buffer, sizeof(buffer), 0) == 0) {
                        strcpy(buffer, (userIDs[cli] + " disconnected.").c_str());
                        close(cli);
                        clientSockets.erase(cli);
                        userIDs.erase(cli);
                    }
                    
                    cout << buffer << endl;

                    //send this message to the other clients
                    for (int other : clientSockets) {
                        if (cli != other) 
                            send(other, &buffer, strlen(buffer), 0);
                    }
                }
            }

            //if only 1 client left in room, tell them to wait
            if (clientSockets.size() == 1) {
                //clear the buffer
                memset(&buffer, 0, sizeof(buffer));
                strcpy(buffer, "No other users online. Please wait for another user to join.");
                send(*clientSockets.begin(), &buffer, strlen(buffer), 0);
            }
        }
    }

    //close socket
    close(recvSocket);
}

void waitForTermination() {
    string msg;
    //loop until ctrl+c or ctrl+d
    while (getline(cin, msg)) {}
    memset(&buffer, 0, sizeof(buffer));         //clear the buffer
    strcpy(buffer, "Server terminated by administrator. Exiting...");
    //send termination message to clients
    for (int cli : clientSockets) {
        send(cli, &buffer, strlen(buffer), 0);
    }
    cout << "Server terminating..." << endl;
    exit(0);
}