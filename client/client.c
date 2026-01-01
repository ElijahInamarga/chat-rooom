#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_IP_ADDR "192.168.0.248"
#define BUFFER_SIZE    256
#define PORT_NUM       8080
#define NUM_FDS        2

int start_session(int socketfd)
{
    // stdin and socket
    struct pollfd fds[NUM_FDS] = {{0, POLLIN, 0}, {socketfd, POLLIN, 0}};

    // 0b 0000 0000 0000 0001 == data available
    for(;;) {
        poll(fds, NUM_FDS, -1);
        char buffer[BUFFER_SIZE] = {0};

        // client to server
        if(fds[0].revents & POLLIN) {
            int bytes_read = read(0, buffer, sizeof(buffer) - 1);
            if(bytes_read > 0) {
                buffer[bytes_read] = '\0';
            }
            send(socketfd, buffer, bytes_read, 0);
        }

        // server to client
        if(fds[1].revents & POLLIN) {
            int bytes_read = recv(socketfd, buffer, sizeof(buffer) - 1, 0);
            if(bytes_read == 0) {
                printf("Server terminated\n");
                return 0;
            }

            if(bytes_read > 0) {
                buffer[bytes_read] = '\0';
            }

            printf("Server: %s", buffer);
        }
    }

    return 0;
}

void close_session(int socketfd)
{
    close(socketfd);
}

int connect_to_server()
{
    int socketfd;
    socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(socketfd == -1) {
        perror("Error creating socket\n");
        return -1;
    }

    // server info
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUM);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);

    int connect_status;
    connect_status =
        connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if(connect_status == -1) {
        perror("Error connecting socket to server\n");
        return -1;
    }

    printf("Connected to server\n\n");
    return socketfd;
}

int main()
{
    // connect to server
    int socketfd = connect_to_server();
    if(socketfd == -1) {
        printf("Could not connect to server\n");
        return -1;
    }

    // begin session with server
    int session_result = start_session(socketfd);

    close_session(socketfd);

    return session_result;
}
