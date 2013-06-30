/* encoding: UTF-8 */

#include <stdio.h>

#include "gateway_main.h"
#include "gateway_common.h"
#include "gateway_socket.h"
#include "gateway_clients.h"

int main(int const argc, char const *argv[])
{
    int socket_fd_local;
    gateway_error_code_t err_code = GATEWAY_SUCCESS;
    unsigned int client_num;

    socket_fd_local = gateway_socket_create_socket();
    if (-1 == socket_fd_local)
    {
        err_code = GATEWAY_CREATE_IPC_SOCKET_ERROR;
        goto error;
    }

    if (-1 == gateway_socket_bind_socket(socket_fd_local))
    {
        err_code = GATEWAY_BIND_IPC_SOCKET_ERROR;
        goto error;
    }

    for (client_num = 0; client_num < GATEWAY_CLIENTS_COUNT; client_num++)
    {
        int socket_fd_remote;
        gateway_clients_types_t client_type;

        if (-1 == gateway_socket_listen_socket(socket_fd_local))
        {
            err_code = GATEWAY_LISTEN_IPC_SOCKET_ERROR;
            goto error_client;
        }

        socket_fd_remote = gateway_socket_accept_socket(socket_fd_local);
        if (-1 == socket_fd_remote)
        {
            err_code = GATEWAY_ACCEPT_IPC_SOCKET_ERROR;
            goto error_client;
        }

        client_type = gateway_clients_request_client_type(socket_fd_remote);
        if (-1 == client_type)
        {
            err_code = GATEWAY_RECV_IPC_SOCKET_ERROR;
            goto error_client;
        }
        
        gateway_clients_register_client(socket_fd_remote, client_type, client_num);
    }

    error_client:

    gateway_clients_remove_registered_clients();

    error:

    gateway_socket_close_socket(socket_fd_local);

    return err_code;
}