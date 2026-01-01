#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 256

int main()
{
    int passive_socketfd;
    passive_socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(passive_socketfd == -1) {
        printf("Error creating passive socket\n");
        return -1;
    }

    // server info
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    int bind_status = bind(passive_socketfd, (struct sockaddr *)&server_addr,
                           sizeof(server_addr));

    if(bind_status == -1) {
        printf("Error binding passive socket to IP and port\n");
        return -1;
    }

    listen(passive_socketfd, 3);

    int socketfd;
    socketfd = accept(passive_socketfd, NULL, NULL);
    if(socketfd == -1) {
        printf("Error accepting connection\n");
        return -1;
    }

    // stdin and socket
    struct pollfd fds[2] = {{0, POLLIN, 0}, {socketfd, POLLIN, 0}};

    // 0b 0000 0000 0000 0001 == data available
    for(;;) {
        poll(fds, 2, -1);
        char buffer[BUFFER_SIZE] = {0};

        // server to client
        if(fds[0].revents & POLLIN) {
            int bytes_read = read(0, buffer, sizeof(buffer) - 1);
            if(bytes_read == 0) {
                printf("Client has left\n");
                return 0;
            }

            if(bytes_read > 0) {
                buffer[bytes_read] = '\0';
            }

            send(socketfd, buffer, bytes_read, 0);
        }

        // client to server
        if(fds[1].revents & POLLIN) {
            int bytes_read = recv(socketfd, buffer, sizeof(buffer) - 1, 0);
            if(bytes_read > 0) {
                buffer[bytes_read] = '\0';
            }
            printf("Client response: %s\n", buffer);
        }
    }

    close(socketfd);
    close(passive_socketfd);

    return 0;
}
