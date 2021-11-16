/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    // file descriptors
    int sockfd, newsockfd, portno;
    // sixe of the address of the client
    socklen_t clilen;
    // server reads character from socket connection into here
    char buffer[256];
    // struct containing an internet address. Defined in netinet/in.h
    // server contains address of server, cli contains address of client
    struct sockaddr_in serv_addr, cli_addr;
    // number of characters read or written
    int n;
    // ensures a port is provided
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    // Creating a new socket. Returns an entry into the file descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    // Initializes serv_addr to all zeros
    bzero((char *)&serv_addr, sizeof(serv_addr));
    // Port number that the server will use to listen for connections
    portno = atoi(argv[1]);
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
    // Block until a client connects to the server. Returns a new file descriptor
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    while (strncmp(buffer, "kill", strlen("kill")) != 0)
    {
        // initializes buffer to be all zeros
        bzero(buffer, 256);
        // read in data from client. Blocks until something is sent from client
        n = read(newsockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket\r\n");
        printf("Here is the message: %s", buffer);
        if (strncmp(buffer, "test", strlen("test")) == 0) {
            printf("testing someting\r\n");
        }
        // Send message back to client
        n = write(newsockfd, "I got your message", 18);
        if (n < 0)
            error("ERROR writing to socket\r\n");
    }
    
    
    // Close sockets
    close(newsockfd);
    close(sockfd);
    return 0;

}