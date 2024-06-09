#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

    int sockfd, portno, local_port, n;
    struct sockaddr_in serv_addr, local_addr;
    struct hostent *server;
    char buffer[255];
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s hostname server_port local_port\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[2]);
    local_port = atoi(argv[3]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error opening socket");
    }

    bzero((char *)&local_addr, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(local_port);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
    {
        error("Binding failed");
    }
    portno = atoi(argv[2]);
    local_port = atoi(argv[3]);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error opening socket");
    }

    // Bind socket to local port
    bzero((char *)&local_addr, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(local_port);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
    {
        error("Binding failed");
    }
    
    printf("Enter username: ");
    bzero(buffer, 255);
    fgets(buffer, 255, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    printf("Enter password: ");
    char password[255];
    bzero(password, 255);
    fgets(password, 255, stdin);
    password[strcspn(password, "\n")] = '\0'; 

    char registration_data[512];
    sprintf(registration_data, "%s:%s", buffer, password);
    n = write(sockfd, registration_data, strlen(registration_data));
    if (n < 0)
        error("Error sending registration data");

    bzero(buffer, 255);
    n = read(sockfd, buffer, 255);
    if (n < 0)
        error("Error reading registration response");
    printf("%s\n", buffer); 

    if (strstr(buffer, "Chat is currently full") != NULL)
    {
        printf("Chat is currently full. Please wait for others to join.\n");
        close(sockfd);
        exit(0);
    }

    // Start chat interaction
    printf("You: ");
    while (1)
    {
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; 

        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("Error writing to socket");

        if (strncmp(buffer, "Bye", 3) == 0)
            break;

        bzero(buffer, 255);
        n = read(sockfd, buffer, 255);
        if (n < 0)
            error("Error reading from socket");

        printf("Client: %s\n", buffer);
        printf("You: ");
    }

    close(sockfd);
    return 0;
}
