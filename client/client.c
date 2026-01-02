#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define PORT_NUM    8080
#define NUM_FDS     2

char                  SERVER_IP_ADDR[64] = "";
volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig)
{
    keep_running = 0;
}

int start_session(int socketfd)
{
    // stdin and socket
    struct pollfd fds[NUM_FDS] = {{0, POLLIN, 0}, {socketfd, POLLIN, 0}};

    // 0b 0000 0000 0000 0001 == data available
    while(keep_running) {
        poll(fds, NUM_FDS, -1);
        char buffer[BUFFER_SIZE] = {0};

        // client to server
        if(fds[0].revents & POLLIN) {
            int bytes_read = read(0, buffer, sizeof(buffer) - 1);
            if(bytes_read > 0) {
                buffer[bytes_read] = '\0';
            }

            // ignore empty inputs
            if(bytes_read == 1 && (buffer[0] == '\n' || buffer[0] == '\r')) {
                printf("You: ");
                fflush(stdout);
                continue;
            }

            send(socketfd, buffer, bytes_read, 0);
            printf("You: ");
            fflush(stdout);
        }

        // server to client
        if(fds[1].revents & POLLIN) {
            int bytes_read = recv(socketfd, buffer, sizeof(buffer) - 1, 0);
            if(bytes_read == 0) {
                printf("STATUS: Server terminated\n");
                return 0;
            }

            if(bytes_read > 0) {
                buffer[bytes_read] = '\0';
            }

            printf("\rChat: %sYou: ", buffer);
            fflush(stdout);
        }
    }

    return 0;
}

void close_session(int socketfd)
{
    printf("\nSTATUS: Disconnected from server\n");
    close(socketfd);
}

int connect_to_server()
{
    int socketfd;
    socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(socketfd == -1) {
        perror("STATUS: Error creating socket\n");
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
        perror("STATUS: Error connecting socket to server\n");
        return -1;
    }

    printf("STATUS: Connected to server\n");
    printf("You: ");
    fflush(stdout);
    return socketfd;
}

int main()
{
    printf("STATUS: Enter server public IP: ");
    if(scanf("%63s", SERVER_IP_ADDR) != 1) {
        printf("STATUS: Invalid input\n");
        return -1;
    }

    // connect to server
    int socketfd = connect_to_server();
    if(socketfd == -1) {
        printf("STATUS: Could not connect to server\n");
        return -1;
    }

    signal(SIGINT, handle_sigint);

    // begin session with server
    int session_result = start_session(socketfd);

    close_session(socketfd);

    return session_result;
}
