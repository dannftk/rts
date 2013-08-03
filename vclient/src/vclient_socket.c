/* encoding: UTF-8 */

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <string.h>

#include "vclient_socket.h"

int vclient_socket_create_socket(void)
{
    return socket(AF_UNIX, SOCK_STREAM, 0);
}

int vclient_socket_connect_socket(int socket_fd_remote)
{
    struct sockaddr_un socket_addr_remote;
    socklen_t socket_addr_len;

    memset(&socket_addr_remote, 0, sizeof(socket_addr_remote));

    socket_addr_remote.sun_family = AF_UNIX;
    strcpy(socket_addr_remote.sun_path, IPC_SOCKET_NAME);
    socket_addr_len = strlen(socket_addr_remote.sun_path) + 1 + sizeof(socket_addr_remote.sun_family);

    return connect(socket_fd_remote, (struct sockaddr *)&socket_addr_remote, socket_addr_len);
}

void vclient_socket_close_socket(int socket_fd)
{
    close(socket_fd);
}

ssize_t vclient_socket_send(int socket_fd_remote, void *buffer, size_t send_bytes)
{
    return send(socket_fd_remote, buffer, send_bytes, 0);
}

ssize_t vclient_socket_receive(int socket_fd_remote, void *buffer, size_t max_recv_bytes)
{
    return recv(socket_fd_remote, buffer, max_recv_bytes, 0);
}