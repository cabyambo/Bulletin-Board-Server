#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdbool.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    // Address to the server we wish to connect to
    struct sockaddr_in serv_addr;
    struct hostent *server;
    bool closing = false;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    while (1)
    {
        // Prompting user for input
        printf("Enter command: ");
        // Clearing buffer
        bzero(buffer,256);
        // Get input from user
        fgets(buffer,255,stdin);

        if (strncmp(buffer, "clear", strlen("clear")) == 0) {
            printf("clear entered\r\n");
        } else if (strncmp(buffer, "post", strlen("post")) == 0) {
            printf("post entered\r\n");
        } else if (strncmp(buffer, "kill", strlen("kill")) == 0) {
            printf("Closing socket and terminating server. Bye!\r\n");
        } else if (strncmp(buffer, "quit", strlen("quit")) == 0) {
            printf("quit entered\r\n");
        } else {
            
        }
        
        // Write input to server
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket");
        // Clear input
        bzero(buffer, 256);
        // Read input from server
        n = read(sockfd,buffer,255);
        if (n < 0)
            error("ERROR reading from socket");
        // Display input from server
        printf("%s\r\n",buffer);

    }
    
    close(sockfd);
    return 0;
}