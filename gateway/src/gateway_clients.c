/* encoding: UTF-8 */

#include "gateway_clients.h"
#include "gateway_socket.h"

typedef struct gateway_clients_s {
    int socket_fd;
    gateway_clients_types_t client_type;
    unsigned int register_number;
    int registered_flag;
    /* TODO: handler */
} gateway_clients_t;

static gateway_clients_t gateway_clients[GATEWAY_CLIENTS_COUNT];
static unsigned int gateway_clients_count;

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
    gateway_clients[client_type] = (gateway_clients_t) {
        .socket_fd = socket_fd_remote,
        .client_type = client_type,
        .register_number = register_number,
        .registered_flag = 1
    };
    ++gateway_clients_count;
}

void gateway_clients_remove_all_clients(void)
{
    unsigned int client_num;
    for (client_num = 0; client_num < GATEWAY_CLIENTS_COUNT; client_num++)
    {
        if (gateway_clients[client_num].registered_flag)
        {
            gateway_socket_close_socket(gateway_clients[client_num].socket_fd);
            gateway_clients[client_num].registered_flag = 0;
        }
    }
}