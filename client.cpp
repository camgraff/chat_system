#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <sys/fcntl.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 
#include <future>
using namespace std;

//port number
#define PORT 8888

string waitForInput();

int main(int argc, char* argv[]) {
    //message buffer
    char buffer[1024];
    //socket setup
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    //socket option for testing
    int optval = true;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));

    sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));      //need to reset serverAddr on consecutive runs
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_port = htons(PORT); 
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    //connect to the server
    if (connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Failed to connect to server. Exiting..." << endl;
        exit(0);
    } else {
        cout << "Connected to server!" <<endl;
    }

    //prompt client for their username
    string userID;
    cout << "Enter your desired user ID: ";
    getline(cin, userID);
     //copy userID into buffer
    strcpy(buffer, userID.c_str());
    //send the buffer to server
    send(serverSocket, &buffer, strlen(buffer), 0);

    //set socket to nonblocking mode for asynchronous calls
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);

    while(1) {
        //asynchronous call for input message while client waits to receive server messages
        future<string> fut = async(waitForInput);

        //keep checking for server messages until client enters a message to send
        //recv() no longer blocks since we put the socket in nonblock mode
        while (fut.wait_for(chrono::seconds(0)) != future_status::ready) {
            //clear the buffer
            memset(&buffer, 0, sizeof(buffer));
            //get message from the server
            if (recv(serverSocket, &buffer, sizeof(buffer), 0) >= 0) {
                cout << buffer << endl;
            }
        }

        //clear the buffer
        memset(&buffer, 0, sizeof(buffer));

        string msg = fut.get();

        //copy message into buffer
        strcpy(buffer, (userID + ": " + msg).c_str());
        //send the buffer to server
        send(serverSocket, &buffer, strlen(buffer), 0);
    }

    close(serverSocket);
}

string waitForInput() {
    string msg;
    if (!(getline(cin, msg))) {
        cout << "Exiting..." << endl;
        exit(0);
    } else {
        return msg;
    }
}