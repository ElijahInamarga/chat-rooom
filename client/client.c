#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 256

int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(socket_fd == -1) {
        printf("Error creating socket\n");
        return -1;
    }

    // destination address
    struct sockaddr_in server_addr = {AF_INET, htons(9999), 0};

    int connect_status = connect(socket_fd, (struct sockaddr *)&server_addr,
                                 sizeof(server_addr));
    if(connect_status == -1) {
        printf("Error connecting to server\n");
        return -1;
    }

    struct pollfd fds[2] = {{0, POLLIN, 0}, {socket_fd, POLLIN, 0}};

    // 0000 0000 0000 0001 == data available
    for(;;) {
        char buffer[256] = {0};

        poll(fds, 2, 50000);

        // read stdin and send to server
        if(fds[0].revents & POLLIN) {
            read(0, buffer, 255);
            send(socket_fd, buffer, BUFFER_SIZE - 1, 0);
        }

        // read server input and print to stdout
        if(fds[1].revents & POLLIN) {
            if(recv(socket_fd, buffer, BUFFER_SIZE - 1, 0) == 0) {
                return 0;
            }
            printf("%s\n", buffer);
        }
    }
    close(socket_fd);

    return 0;
}
