/* encoding: UTF-8 */
#include <stdio.h>

#include "gateway_main.h"
#include "gateway_common.h"
#include "gateway_socket.h"

int main(int const argc, char const *argv[])
{
    int socket_fd_local, socket_fd_remote;
    gateway_error_code_t err_code = GATEWAY_SUCCESS;

    socket_fd_local = gateway_socket_create_socket();
    if (-1 == socket_fd_local)
    {
        err_code = GATEWAY_CREATE_IPC_SOCKET_ERROR;
        goto error;
    }

    if (-1 == gateway_socket_bind_socket(socket_fd_local, IPC_SOCKET_NAME))
    {
        err_code = GATEWAY_BIND_IPC_SOCKET_ERROR;
        goto error;
    }

    if (-1 == gateway_socket_listen_socket(socket_fd_local))
    {
        err_code = GATEWAY_LISTEN_IPC_SOCKET_ERROR;
        goto error;
    }

    socket_fd_remote = gateway_socket_accept_socket(socket_fd_local);
    if (-1 == socket_fd_remote)
    {
        err_code = GATEWAY_ACCEPT_IPC_SOCKET_ERROR;
        goto error;
    }

    gateway_socket_close_socket(socket_fd_remote);

    error:

    gateway_socket_close_socket(socket_fd_local);

    return err_code;
}