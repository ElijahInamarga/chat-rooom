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
    int passive_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(passive_socket_fd == -1) {
        printf("Error creating server socket\n");
        return -1;
    }

    // server address
    struct sockaddr_in server_addr = {AF_INET, htons(9999), 0};

    int bind_status = bind(passive_socket_fd, (struct sockaddr *)&server_addr,
                           sizeof(server_addr));
    if(bind_status != 0) {
        printf("Error binding server socket to IP and port\n");
        return -1;
    }

    listen(passive_socket_fd, 10);

    int active_socket_fd = accept(passive_socket_fd, NULL, NULL);

    struct pollfd fds[2] = {{0, POLLIN, 0}, {active_socket_fd, POLLIN, 0}};

    // 0000 0000 0000 0001 == data available
    for(;;) {
        char buffer[BUFFER_SIZE] = {0};
        poll(fds, 2, 50000);

        // read stdin and send to client
        if(fds[0].revents & POLLIN) {
            read(0, buffer, 255);
            send(active_socket_fd, buffer, BUFFER_SIZE - 1, 0);
        }

        // read client socket and print to stdout
        if(fds[1].revents & POLLIN) {
            if(recv(active_socket_fd, buffer, BUFFER_SIZE - 1, 0) == 0) {
                return 0;
            }
            printf("%s\n", buffer);
        }
    }

    close(passive_socket_fd);
    return 0;
}
