#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 2

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Port No not provided. Program terminated.\n");
        exit(1);
    }

    int sockfd, newsockfd, portno, n;
    char buffer[255];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error opening socket");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Binding failed");
    }

    listen(sockfd, 5);
    printf("Server is listening on port %d...\n", portno);

    int client_sockets[MAX_CLIENTS] = {-1, -1};
    char client_usernames[MAX_CLIENTS][255];
    int current_clients = 0;

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
        {
            error("Error on accept");
        }

        if (fork() == 0)
        { // Child process
            close(sockfd);

            bzero(buffer, 255);
            n = read(newsockfd, buffer, 255);
            if (n < 0)
            {
                error("Error reading registration data");
            }

            char *username = strtok(buffer, ":");
            if (username == NULL)
            {
                error("Invalid registration data");
            }

            printf("Client '%s' connected\n", username);
            if (current_clients < MAX_CLIENTS)
            {
                // Register the client
                client_sockets[current_clients] = newsockfd;
                strcpy(client_usernames[current_clients], username);
                current_clients++;

                const char *registration_msg = "Registration successful. You are now in the chat.\n";
                write(newsockfd, registration_msg, strlen(registration_msg));

                // Check if enough clients have connected to start a chat
                if (current_clients == MAX_CLIENTS)
                {

                    const char *chat_start_msg = "Chat session started. You can now chat.\n";
                    write(client_sockets[0], chat_start_msg, strlen(chat_start_msg));
                    write(client_sockets[1], chat_start_msg, strlen(chat_start_msg));

                    while (1)
                    {
                        // Read message from Client 1
                        bzero(buffer, 255);
                        n = read(client_sockets[0], buffer, 255);
                        if (n < 0)
                        {
                            error("Error reading from Client 1");
                        }
                        printf("%s: %s\n", client_usernames[0], buffer);

                        n = write(client_sockets[1], buffer, strlen(buffer));
                        if (n < 0)
                        {
                            error("Error writing to Client 2");
                        }

                        bzero(buffer, 255);
                        n = read(client_sockets[1], buffer, 255);
                        if (n < 0)
                        {
                            error("Error reading from Client 2");
                        }
                        printf("%s: %s\n", client_usernames[1], buffer);

                        n = write(client_sockets[0], buffer, strlen(buffer));
                        if (n < 0)
                        {
                            error("Error writing to Client 1");
                        }

                        // Check for "Bye" message to exit chat loop
                        if (strncmp(buffer, "Bye", 3) == 0)
                        {
                            break;
                        }
                    }

                    current_clients = 0;
                    client_sockets[0] = -1;
                    client_sockets[1] = -1;
                }
            }
            else
            {

                const char *wait_msg = "Chat is currently full. Please wait for others to join.\n";
                write(newsockfd, wait_msg, strlen(wait_msg));
                close(newsockfd);
            }
            exit(0);
        }
        else
        {
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}
