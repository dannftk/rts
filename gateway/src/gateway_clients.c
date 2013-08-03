/* encoding: UTF-8 */

#include <sys/types.h>

#include <string.h>
#include <stdio.h>

#include "gateway_clients.h"
#include "gateway_dispatcher.h"
#include "gateway_scheduler.h"
#include "gateway_socket.h"
#include "gateway_tasks.h"

gateway_clients_t g_gateway_clients[GATEWAY_CLIENT_TYPES_COUNT];
unsigned int g_gateway_clients_registered, g_gateway_clients_active;
fd_set g_read_client_sockets_fds;
int g_socket_client_fd_max;

static void gateway_clients_deactivate_client(gateway_client_types_t client_type)
{
    if (g_gateway_clients[client_type].registered_flag)
    {
        FD_CLR(g_gateway_clients[client_type].socket_fd, &g_read_client_sockets_fds);
        g_gateway_clients[client_type].active_flag = 0;
        --g_gateway_clients_active;

        if (g_gateway_clients[client_type].socket_fd == g_socket_client_fd_max)
        {
            gateway_client_types_t client_type_2;

            g_socket_client_fd_max = -1;
            for (client_type_2 = GATEWAY_CLIENT_TYPE_MCLIENT_A;
                 client_type_2 < GATEWAY_CLIENT_TYPES_COUNT;
                 client_type_2 = GATEWAY_CLIENTS_TYPES_NEXT(client_type_2))
            {
                if (g_gateway_clients[client_type].socket_fd != g_gateway_clients[client_type_2].socket_fd && 
                    g_gateway_clients[client_type_2].active_flag && 
                    g_gateway_clients[client_type_2].socket_fd > g_socket_client_fd_max)
                {
                    g_socket_client_fd_max = g_gateway_clients[client_type_2].socket_fd;
                }
            }
        }
    }
}

void gateway_clients_deactivate_registered_clients(void)
{
    gateway_client_types_t client_type;

    for (client_type = GATEWAY_CLIENT_TYPE_MCLIENT_A;
         client_type < GATEWAY_CLIENT_TYPES_COUNT;
         client_type = GATEWAY_CLIENTS_TYPES_NEXT(client_type))
    {
        gateway_clients_deactivate_client(client_type);
    }
}

static void mclient_A_handler(void)
{
    recv_mtrx_data_t recv_mtrx_data;
    task_data_t task_data;

    if (-1 == gateway_socket_receive(g_gateway_clients[GATEWAY_CLIENT_TYPE_MCLIENT_A].socket_fd,
                                     &recv_mtrx_data,
                                     sizeof(recv_mtrx_data)))
    {
        GATEWAY_COMMON_ASSERT(0);
    }

    memset(&task_data, 0, sizeof(task_data));

    if (!strcmp(recv_mtrx_data.header, "mtr"))
    {
        task_data.task_type = PUT_MTRX_A_VALUE;
        task_data.data.mtrx = recv_mtrx_data.mtrx;
        gateway_scheduler_add_task_to_queue(task_data);
    }
    else
    {
        GATEWAY_COMMON_ASSERT(0);
    }
}

static void mclient_C_handler(void)
{
    recv_mtrx_data_t recv_mtrx_data;
    task_data_t task_data;
    
    if (-1 == gateway_socket_receive(g_gateway_clients[GATEWAY_CLIENT_TYPE_MCLIENT_C].socket_fd,
                                     &recv_mtrx_data,
                                     sizeof(recv_mtrx_data)))
    {
        GATEWAY_COMMON_ASSERT(0);
    }

    memset(&task_data, 0, sizeof(task_data));
    
    if (!strcmp(recv_mtrx_data.header, "mtr"))
    {
        task_data.task_type = PUT_MTRX_C_VALUE;
        task_data.data.mtrx = recv_mtrx_data.mtrx;
        gateway_scheduler_add_task_to_queue(task_data);
    }
    else
    {
        GATEWAY_COMMON_ASSERT(0);
    }
}

static void vclient_b_handler(void)
{
    recv_vector_data_t recv_vector_data;
    task_data_t task_data;
    
    memset(&task_data, 0, sizeof(task_data));
    
    if (-1 == gateway_socket_receive(g_gateway_clients[GATEWAY_CLIENT_TYPE_VCLIENT_b].socket_fd,
                                     &recv_vector_data,
                                     sizeof(recv_vector_data)))
    {
        GATEWAY_COMMON_ASSERT(0);
    }
    
    if (!strcmp(recv_vector_data.header, "vec"))
    {
        task_data.task_type = PUT_VECTOR_b_VALUE;
        task_data.data.vector = recv_vector_data.vector;
        gateway_scheduler_add_task_to_queue(task_data);
    }
    else
    {
        GATEWAY_COMMON_ASSERT(0);
    }
}

static void vclient_d_handler(void)
{
    recv_vector_data_t recv_vector_data;
    task_data_t task_data;
    
    memset(&task_data, 0, sizeof(task_data));

    if (-1 == gateway_socket_receive(g_gateway_clients[GATEWAY_CLIENT_TYPE_VCLIENT_d].socket_fd,
                                     &recv_vector_data,
                                     sizeof(recv_vector_data)))
    {
        GATEWAY_COMMON_ASSERT(0);
    }

    if (!strcmp(recv_vector_data.header, "vec"))
    {
        task_data.task_type = PUT_VECTOR_d_VALUE;
        task_data.data.vector = recv_vector_data.vector;
        gateway_scheduler_add_task_to_queue(task_data);
    }
    else
    {
        GATEWAY_COMMON_ASSERT(0);
    }
}

static void deststation_handler(void)
{
    recv_vector_pos_from_deststation_data_t recv_vector_pos_from_deststation_data;

    char header_vector_data_request[4];
    send_mtrx_row_data_request_t send_mtrx_row_data;

    if (-1 == gateway_socket_receive(g_gateway_clients[GATEWAY_CLIENT_TYPE_DESTSTATION].socket_fd,
                                     &recv_vector_pos_from_deststation_data,
                                     sizeof(recv_vector_pos_from_deststation_data)))
    {
        GATEWAY_COMMON_ASSERT(0);
    }

    if (!strcmp(recv_vector_pos_from_deststation_data.header, "pos") &&
        -1 != recv_vector_pos_from_deststation_data.vector_val_pos)
    {
        send_mtrx_row_data = (send_mtrx_row_data_request_t) {
            .header = "mtr",
            .row = recv_vector_pos_from_deststation_data.vector_val_pos
        };

        if (-1 == gateway_socket_send(g_gateway_clients[GATEWAY_CLIENT_TYPE_MCLIENT_A].socket_fd,
                                      &send_mtrx_row_data,
                                      sizeof(send_mtrx_row_data)))
        {
            GATEWAY_COMMON_ASSERT(0);
        }

        if (-1 == gateway_socket_send(g_gateway_clients[GATEWAY_CLIENT_TYPE_MCLIENT_C].socket_fd,
                                      &send_mtrx_row_data,
                                      sizeof(send_mtrx_row_data)))
        {
            GATEWAY_COMMON_ASSERT(0);
        }

        strcpy(header_vector_data_request, "vec");

        if (-1 == gateway_socket_send(g_gateway_clients[GATEWAY_CLIENT_TYPE_VCLIENT_b].socket_fd,
                                      &header_vector_data_request,
                                      sizeof(header_vector_data_request)))
        {
            GATEWAY_COMMON_ASSERT(0);
        }

        if (-1 == gateway_socket_send(g_gateway_clients[GATEWAY_CLIENT_TYPE_VCLIENT_d].socket_fd,
                                      &header_vector_data_request,
                                      sizeof(header_vector_data_request)))
        {
            GATEWAY_COMMON_ASSERT(0);
        }

        gateway_dispatcher_run_dispatcher();
    }
    else if (!strcmp(recv_vector_pos_from_deststation_data.header, "end") &&
             -1 == recv_vector_pos_from_deststation_data.vector_val_pos)
    {
        send_mtrx_row_data = (send_mtrx_row_data_request_t) {
            .header = "end",
            .row = -1
        };

        if (-1 == gateway_socket_send(g_gateway_clients[GATEWAY_CLIENT_TYPE_MCLIENT_A].socket_fd,
                                      &send_mtrx_row_data,
                                      sizeof(send_mtrx_row_data)))
        {
            GATEWAY_COMMON_ASSERT(0);
        }

        if (-1 == gateway_socket_send(g_gateway_clients[GATEWAY_CLIENT_TYPE_MCLIENT_C].socket_fd,
                                      &send_mtrx_row_data,
                                      sizeof(send_mtrx_row_data)))
        {
            GATEWAY_COMMON_ASSERT(0);
        }

        strcpy(header_vector_data_request, "end");

        if (-1 == gateway_socket_send(g_gateway_clients[GATEWAY_CLIENT_TYPE_VCLIENT_b].socket_fd,
                                      &header_vector_data_request,
                                      sizeof(header_vector_data_request)))
        {
            GATEWAY_COMMON_ASSERT(0);
        }

        if (-1 == gateway_socket_send(g_gateway_clients[GATEWAY_CLIENT_TYPE_VCLIENT_d].socket_fd,
                                      &header_vector_data_request,
                                      sizeof(header_vector_data_request)))
        {
            GATEWAY_COMMON_ASSERT(0);
        }

        gateway_clients_deactivate_registered_clients();
    }
    else
    {
        GATEWAY_COMMON_ASSERT(0);
    }
}

static void (*get_client_handler_by_client_type(gateway_client_types_t client_type))(void)
{
    switch (client_type)
    {
        case GATEWAY_CLIENT_TYPE_MCLIENT_A:
            return mclient_A_handler;
        case GATEWAY_CLIENT_TYPE_MCLIENT_C:
            return mclient_C_handler;
        case GATEWAY_CLIENT_TYPE_VCLIENT_b:
            return vclient_b_handler;
        case GATEWAY_CLIENT_TYPE_VCLIENT_d:
            return vclient_d_handler;
        case GATEWAY_CLIENT_TYPE_DESTSTATION:
            return deststation_handler;
        default:
            GATEWAY_COMMON_ASSERT(0);
            break;
    }
    return NULL;
}

enum gateway_client_types_e gateway_clients_request_client_type(int socket_fd_remote)
{
    gateway_client_types_t client_type;
    recv_client_type_data_t recv_client_type_data;
    
    if (-1 == gateway_socket_receive(socket_fd_remote,
                                     &recv_client_type_data,
                                     sizeof(recv_client_type_data)))
    {
        client_type = -1;
    }
    else
    {
        client_type = strcmp(recv_client_type_data.header, "typ") ? -1 : recv_client_type_data.client_type;
    }

    return client_type;
}

void gateway_clients_register_client(int socket_fd_remote,
                                     gateway_client_types_t client_type,
                                     unsigned int register_number)
{
    g_gateway_clients[client_type] = (gateway_clients_t) {
        .socket_fd = socket_fd_remote,
        .client_type = client_type,
        .register_number = register_number,
        .registered_flag = 1,
        .run_handler = get_client_handler_by_client_type(client_type)
    };
    ++g_gateway_clients_registered;
}

static void gateway_clients_remove_registered_client(gateway_client_types_t client_type)
{
    if (g_gateway_clients[client_type].registered_flag)
    {
        gateway_socket_close_socket(g_gateway_clients[client_type].socket_fd);
        g_gateway_clients[client_type] = (gateway_clients_t) {
            .socket_fd = 0,
            .registered_flag = 0,
            .run_handler = NULL
        };
        --g_gateway_clients_registered;
    }
}

void gateway_clients_remove_registered_clients(void)
{
    gateway_client_types_t client_type;
    for (client_type = GATEWAY_CLIENT_TYPE_MCLIENT_A;
         client_type < GATEWAY_CLIENT_TYPES_COUNT;
         client_type = GATEWAY_CLIENTS_TYPES_NEXT(client_type))
    {
        gateway_clients_remove_registered_client(client_type);
    }
}

static void gateway_clients_activate_registered_client(gateway_client_types_t client_type)
{
    if (g_gateway_clients[client_type].registered_flag)
    {
        FD_SET(g_gateway_clients[client_type].socket_fd, &g_read_client_sockets_fds);
        g_gateway_clients[client_type].active_flag = 1;

        if (g_gateway_clients[client_type].socket_fd > g_socket_client_fd_max)
        {
            g_socket_client_fd_max = g_gateway_clients[client_type].socket_fd;
        }
        ++g_gateway_clients_active;
    }
}

void gateway_clients_activate_registered_clients(void)
{
    gateway_client_types_t client_type;

    FD_ZERO(&g_read_client_sockets_fds);
    g_socket_client_fd_max = -1;
    for (client_type = GATEWAY_CLIENT_TYPE_MCLIENT_A;
         client_type < GATEWAY_CLIENT_TYPES_COUNT;
         client_type = GATEWAY_CLIENTS_TYPES_NEXT(client_type))
    {
        gateway_clients_activate_registered_client(client_type);
    }
}

void gateway_clients_reload_fd_set(void)
{
    gateway_client_types_t client_type;

    FD_ZERO(&g_read_client_sockets_fds);
    
    g_socket_client_fd_max = -1;
    for (client_type = GATEWAY_CLIENT_TYPE_MCLIENT_A;
         client_type < GATEWAY_CLIENT_TYPES_COUNT;
         client_type = GATEWAY_CLIENTS_TYPES_NEXT(client_type))
    {
        if (g_gateway_clients[client_type].active_flag)
        {
            FD_SET(g_gateway_clients[client_type].socket_fd, &g_read_client_sockets_fds);
            if (g_gateway_clients[client_type].socket_fd > g_socket_client_fd_max)
            {
                g_socket_client_fd_max = g_gateway_clients[client_type].socket_fd;
            }
        }
    }
}