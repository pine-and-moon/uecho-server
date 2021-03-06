#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "errors.h"
#include "log.h"

#define BUFFER_SIZE 1500
#define PORT 60000


void mainloop(int server_socket);
void create_sock(int * sock);
void create_addr(struct sockaddr_in * sock_addr, uint32_t host, uint16_t port);


int main(int argc, char const *argv[])
{
    int server_socket;
    struct sockaddr_in server_addr;

    create_sock(&server_socket);

    create_addr(&server_addr, INADDR_ANY, PORT);
    
    if (bind(server_socket, (const struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        error_handle(2);
    }

    write_log("server start!\n");

    mainloop(server_socket);

    close(server_socket);
    return 0;
}


void create_sock(int * sock)
{
    *sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (*sock == -1)
    {
        error_handle(1);
    }
}

void create_addr(struct sockaddr_in * sock_addr, uint32_t host, uint16_t port)
{
    memset(sock_addr, 0, sizeof(struct sockaddr_in));
    sock_addr->sin_family = AF_INET;
    sock_addr->sin_addr.s_addr = htonl(host);
    sock_addr->sin_port = htons(port);
}

void mainloop(int server_socket)
{
    socklen_t client_addr_len;
    size_t str_len;
    uint8_t massage[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    for(;;)
    {
        client_addr_len = sizeof(client_addr);
        str_len = recvfrom(server_socket, massage, BUFFER_SIZE, 0,
            (struct sockaddr*)&client_addr, &client_addr_len);
        sendto(server_socket, massage, str_len, 0, 
            (struct sockaddr*)&client_addr, client_addr_len);
        write_log(
            "%s:%u sent %lu byte(s)\n",
            inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port),
            str_len
        );
    }
}