/* encoding: UTF-8 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "gateway_main.h"
#include "gateway_common.h"
#include "gateway_socket.h"
#include "gateway_clients.h"

extern gateway_clients_t g_gateway_clients[GATEWAY_CLIENTS_COUNT];
extern unsigned int g_gateway_clients_active;

static enum gateway_error_code_e begin_process(void)
{
    gateway_error_code_t err_code = GATEWAY_SUCCESS;
    fd_set read_client_sockets_fds;
    int result;
    int socket_client_fd_max;

    FD_ZERO(&read_client_sockets_fds);
    socket_client_fd_max = gateway_clients_set_registered_clients_onto_fdset(&read_client_sockets_fds);

    while (g_gateway_clients_active)
    {
        result = select(socket_client_fd_max + 1, &read_client_sockets_fds, NULL, NULL, 0);
        if (result < 0)
        {
            GATEWAY_COMMON_ASSERT(0);
        }
        else
        {
            gateway_clients_types_t client_num;
            for (client_num = GATEWAY_CLIENTS_MCLIENT_A;
                 client_num < GATEWAY_CLIENTS_COUNT;
                 client_num = GATEWAY_CLIENTS_TYPES_NEXT(client_num))
            {
                if (FD_ISSET(g_gateway_clients[client_num].socket_fd, &read_client_sockets_fds))
                {
                    g_gateway_clients[client_num].run_handler();
                }
            }
        }
    }

    return err_code;
}

int main(int const argc, char const *argv[])
{
    int socket_fd_local;
    gateway_error_code_t err_code = GATEWAY_SUCCESS;
    unsigned int client_num;

    /* Create server-gateway socket */
    socket_fd_local = gateway_socket_create_socket();
    if (-1 == socket_fd_local)
    {
        err_code = GATEWAY_CREATE_IPC_SOCKET_ERROR;
        goto error;
    }

    /* Bind server-gateway socket */
    if (-1 == gateway_socket_bind_socket(socket_fd_local))
    {
        err_code = GATEWAY_BIND_IPC_SOCKET_ERROR;
        goto error;
    }

    /* Listen this server-gateway socket for establishment of new connections */
    for (client_num = 0; client_num < GATEWAY_CLIENTS_COUNT; client_num++)
    {
        int socket_fd_remote;
        gateway_clients_types_t client_type;

        if (-1 == gateway_socket_listen_socket(socket_fd_local))
        {
            err_code = GATEWAY_LISTEN_IPC_SOCKET_ERROR;
            goto error_client;
        }

        /* For each connection with new client we get new connection-oriented socket */
        socket_fd_remote = gateway_socket_accept_socket(socket_fd_local);
        if (-1 == socket_fd_remote)
        {
            err_code = GATEWAY_ACCEPT_IPC_SOCKET_ERROR;
            goto error_client;
        }

        /* Request client type from the new client */
        client_type = gateway_clients_request_client_type(socket_fd_remote);
        if (-1 == client_type)
        {
            err_code = GATEWAY_RECV_IPC_SOCKET_ERROR;
            goto error_client;
        }
        
        /* Register new client in the RTS GateWay */
        gateway_clients_register_client(socket_fd_remote, client_type, client_num);
    }

    /* Begin processing data */
    err_code = begin_process();

    error_client:

    /* Remove all registered earlier clients and close their sockets */
    gateway_clients_remove_registered_clients();

    error:

    /* Close server-gateway socket */
    gateway_socket_close_socket(socket_fd_local);

    return err_code;
}