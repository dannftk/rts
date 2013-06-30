/* encoding: UTF-8 */

#include "gateway_clients.h"
#include "gateway_socket.h"

gateway_clients_t g_gateway_clients[GATEWAY_CLIENTS_COUNT];
unsigned int g_gateway_clients_registred, g_gateway_clients_active;

enum gateway_clients_types_e gateway_clients_request_client_type(int socket_fd_remote)
{
    gateway_clients_types_t client_type;
    if (-1 == gateway_socket_receive(socket_fd_remote, &client_type, sizeof(client_type)))
    {
        client_type = -1;
    }
    return client_type;
}

void gateway_clients_register_client(int socket_fd_remote,
                                     gateway_clients_types_t client_type,
                                     unsigned int register_number)
{
    g_gateway_clients[client_type] = (gateway_clients_t) {
        .socket_fd = socket_fd_remote,
        .client_type = client_type,
        .register_number = register_number,
        .registered_flag = 1
    };
    ++g_gateway_clients_registred;
}

void gateway_clients_remove_registered_clients(void)
{
    unsigned int client_num;
    for (client_num = GATEWAY_CLIENTS_MCLIENT_A;
         client_num < GATEWAY_CLIENTS_COUNT;
         client_num = GATEWAY_CLIENTS_TYPES_NEXT(client_num))
    {
        if (g_gateway_clients[client_num].registered_flag)
        {
            gateway_socket_close_socket(g_gateway_clients[client_num].socket_fd);
            g_gateway_clients[client_num].registered_flag = 0;
        }
    }
}

int gateway_clients_set_registered_clients_onto_fdset(fd_set *readfds)
{
    int socket_client_fd_max = -1;
    gateway_clients_types_t client_num;

    for (client_num = GATEWAY_CLIENTS_MCLIENT_A;
         client_num < GATEWAY_CLIENTS_COUNT;
         client_num = GATEWAY_CLIENTS_TYPES_NEXT(client_num))
    {
        if (g_gateway_clients[client_num].registered_flag)
        {
            FD_SET(g_gateway_clients[client_num].socket_fd, readfds);
            if (g_gateway_clients[client_num].socket_fd > socket_client_fd_max)
            {
                socket_client_fd_max = g_gateway_clients[client_num].socket_fd;
            }
            ++g_gateway_clients_active;
        }
    }

    return socket_client_fd_max;
}