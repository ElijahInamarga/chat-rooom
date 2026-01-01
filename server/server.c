#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define PORT_NUM    8080
#define BACKLOG_LEN 3
#define NUM_FDS     2

int start_session(int socketfd)
{
    // stdin and socket
    struct pollfd fds[NUM_FDS] = {{0, POLLIN, 0}, {socketfd, POLLIN, 0}};

    // 0b 0000 0000 0000 0001 == data available
    for(;;) {
        poll(fds, NUM_FDS, -1);
        char buffer[BUFFER_SIZE] = {0};

        // server to client
        if(fds[0].revents & POLLIN) {
            int bytes_read = read(0, buffer, sizeof(buffer) - 1);
            if(bytes_read > 0) {
                buffer[bytes_read] = '\0';
            }
            send(socketfd, buffer, bytes_read, 0);
        }

        // client to server
        if(fds[1].revents & POLLIN) {
            int bytes_read = recv(socketfd, buffer, sizeof(buffer) - 1, 0);
            if(bytes_read == 0) {
                printf("Client has left\n");
                return 0;
            }

            if(bytes_read > 0) {
                buffer[bytes_read] = '\0';
            }

            printf("Client: %s", buffer);
        }
    }

    return 0;
}

void close_session(int socketfd)
{
    close(socketfd);
}

int start_server()
{
    int socketfd;
    socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(socketfd == -1) {
        perror("Error creating passive socket\n");
        return -1;
    }

    // server info
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUM);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // allow immediate port reusability
    int opt = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Error setting port reusability\n");
    }

    int bind_status =
        bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if(bind_status == -1) {
        perror("Error binding passive socket to IP and port\n");
        return -1;
    }

    listen(socketfd, BACKLOG_LEN);
    printf("Server initiated\n\n");
    return socketfd;
}

void close_server(int passive_socketfd)
{
    close(passive_socketfd);
}

int main()
{
    // start server
    int passive_socketfd = start_server();
    if(passive_socketfd == -1) {
        printf("Could not start server\n");
        return -1;
    }

    // wait for clients
    int socketfd = accept(passive_socketfd, NULL, NULL);
    if(socketfd == -1) {
        printf("Could not accept connection\n");
        return -1;
    }
    printf("Client has connected\n\n");

    // begin session with client
    int session_result = start_session(socketfd);

    close_session(socketfd);

    close_server(passive_socketfd);

    return session_result;
}
