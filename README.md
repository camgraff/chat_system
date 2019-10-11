>>To start the chat system:

1. cd into the project directory and type 'make' to compile the server and client.

2. Use './server' to start the server. You can also specify a port number using './server [PORT]'. If not specified, PORT defaults to 8000. 

3. Open a new terminal window and cd to project directory.

4. Use './client' to start the client. You can specify a server IP and port number using './client [SERVER_IP] [PORT]'. You cannot only specify one argument. You must specify SERVER_IP and PORT or omit both arguments. SERVER_IP defaults to localhost and PORT default to 8000.

NOTE: The easiest way to start the chat system is by omitting all arguments. (i.e. just use ./server to start the server and ./client to connect the client to server.)

5. The client will connect to the server and prompt for a user ID. This is the name that will appear next to your messages for other client in the chat room. 

6. You can disconnect the client from the server using ctrl+d. You can also terminate the server which will subsequently close all client connections using ctrl+d in the server terminal window.

7. Default max number of clients that can simultaneously connect to the server is 4. You can change this my changing the value of MAX_CLIENTS in server.cpp.
