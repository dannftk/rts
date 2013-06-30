/* encoding: UTF-8 */

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <string.h>

#include "gateway_common.h"
#include "gateway_socket.h"

int gateway_socket_create_socket(void)
{
    return socket(AF_UNIX, SOCK_STREAM, 0);
}

int gateway_socket_bind_socket(int socket_fd_local, char const *socket_path)
{
    struct sockaddr_un local_socket_addr;
    socklen_t socket_addr_len;

    memset(&local_socket_addr, 0, sizeof(local_socket_addr));

    local_socket_addr.sun_family = AF_UNIX;
    strcpy(local_socket_addr.sun_path, socket_path);
    unlink(local_socket_addr.sun_path);
    socket_addr_len = strlen(local_socket_addr.sun_path) + 1 + sizeof(local_socket_addr.sun_family);

    return bind(socket_fd_local, (struct sockaddr *)&local_socket_addr, socket_addr_len);
}

int gateway_socket_listen_socket(int socket_fd_local)
{
    return listen(socket_fd_local, 5);
}

int gateway_socket_accept_socket(int socket_fd_local)
{
    struct sockaddr_un remote_socket_addr;
    socklen_t socket_addr_len = sizeof(remote_socket_addr);

    return accept(socket_fd_local, (struct sockaddr *)&remote_socket_addr, (unsigned int*)&socket_addr_len);
}

void gateway_socket_close_socket(int socket_fd)
{
    close(socket_fd);
}