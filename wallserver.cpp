/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <queue>
#include <iostream>
#include <string>
#include <fcntl.h>

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void printWall(int* socket, queue<string> que) {
    string header = "Wall Contents\n";
    string subHeader = "-------------\n";
    string empty = "[NO MESSAGES – WALL EMPTY]\n\n";
    write(*socket, header.c_str(), strlen(header.c_str()));
    write(*socket, subHeader.c_str(), strlen(subHeader.c_str()));
    if (que.empty())
        write(*socket, empty.c_str(), strlen(empty.c_str()));
    else {
        while (!que.empty()) {
            write(*socket, que.front().c_str(), strlen(que.front().c_str()));
            write(*socket, "\n", 1);
            que.pop();
        }
        write(*socket, "\n", 1);
    }
}

void addToBoard(queue<string>* que, string message, int maxSize) {
    if (que->size() >= maxSize) {
        que->pop();
    }
    que->push(message);
}

int main(int argc, char *argv[])
{
    // file descriptors
    int sockfd;
    int newsockfd = -1000;
    int portno = 5514;
    // sixe of the address of the client
    socklen_t clilen;
    // server reads character from socket connection into here
    char buffer[256];
    char name[256];
    char message[256];
    queue<string> bulletinBoard;
    int maxLength = 20;
    // struct containing an internet address. Defined in netinet/in.h
    // server contains address of server, cli contains address of client
    struct sockaddr_in serv_addr, cli_addr;
    // number of characters read or written
    int n;

    // check if length or port were provided
    if (argc >= 2)
        maxLength = atoi(argv[1]);
    if (argc >= 3)
        portno = atoi(argv[2]);
    
    // Creating a new socket. Returns an entry into the file descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    // Initializes serv_addr to all zeros
    bzero((char *)&serv_addr, sizeof(serv_addr));
    // Port number that the server will use to listen for connections

    // Trivial
    serv_addr.sin_family = AF_INET;
    // IP address of the machine the server is running on
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Port number must be converted to network of byte order
    serv_addr.sin_port = htons(portno);
    // Binding a socket to an address
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    // begins listening for connections
    listen(sockfd, 5);
    // Size of the client
    clilen = sizeof(cli_addr);
    // newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    // cout << "Client connected" << endl;
    // printWall(&newsockfd, bulletinBoard);

    while (1) {
        // initializes buffer to be all zeros
        bzero(buffer, 256);
        bzero(name, 256);
        bzero(message, 256);

        // check if file server is conencted to the client or not
        if (write(newsockfd, "Enter command: ", 15) < 0) {
            // cout << "No client connection" << endl;
            // file descriptor is invalid or closed
            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
            // cout << "Client connected" << endl;
            if (newsockfd < 0)
                error("ERROR on accept");
            // Initial printing of the Wall
            printWall(&newsockfd, bulletinBoard);
            write(newsockfd, "Enter command: ", 15);
        }

        // read in data from client. Blocks until something is sent from client
        n = read(newsockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket\r\n");

        // printf("%s", buffer);

        if (strncmp(buffer, "post", strlen("post")) == 0) {
            write(newsockfd, "Enter Name: ", 12);
            read(newsockfd, name, 256);
            write(newsockfd, "Post [Max length ", 17);

            // Get max length
            int max = 80 - strlen(name) - 1;

            write(newsockfd, to_string(max).c_str(), strlen(to_string(max).c_str()));
            write(newsockfd, "]: ", 3);
            read(newsockfd, message, 256);
            if (strlen(message) > max + 1) {
                write(newsockfd, "Error: message is too long!\n\n", 29);
            } else {
                write(newsockfd, "Successfully tagged the wall.\n\n", 31);

                string n, m;
                n = string(name);
                m = string(message);

                // Remove null terminators
                n.pop_back();
                m.pop_back();

                string completeMessage = n + ": " + m;

                addToBoard(&bulletinBoard, completeMessage, maxLength);
                printWall(&newsockfd, bulletinBoard);
            }

        } else if (strncmp(buffer, "clear", strlen("clear")) == 0) {
            // Empty board
            while(!bulletinBoard.empty()) bulletinBoard.pop();

            string output = "Wall cleared.\n\n";
            write(newsockfd, output.c_str(), strlen(output.c_str()));

            printWall(&newsockfd, bulletinBoard);

        } else if (strncmp(buffer, "kill", strlen("kill")) == 0) {
            string output = "Closing socket and terminating server. Bye!\n";
            write(newsockfd, output.c_str(), strlen(output.c_str()));
            break;
        } else if (strncmp(buffer, "quit", strlen("quit")) == 0) {
            string output = "Come back soon. Bye!\n";
            write(newsockfd, output.c_str(), strlen(output.c_str()));
            close(newsockfd);
        } else {
            write(newsockfd, "Command not recognized\n", 23);
        }
    }
    
    
    // Close sockets
    close(newsockfd);
    close(sockfd);
    return 0;

}