/* encoding: UTF-8 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "gateway_common.h"
#include "gateway_clients.h"
#include "gateway_log.h"
#include "gateway_main.h"
#include "gateway_scheduler.h"
#include "gateway_socket.h"

extern gateway_clients_t g_gateway_clients[GATEWAY_CLIENT_TYPES_COUNT];
extern unsigned int g_gateway_clients_active;
extern fd_set g_read_client_sockets_fds;
extern int g_socket_client_fd_max;

static enum gateway_error_code_e begin_process(void)
{
    gateway_error_code_t err_code = GATEWAY_SUCCESS;
    int result;
    char header_to_start_deststation[4] = "sta";

    GATEWAY_LOG_LOG("Init Sync Queue Mutex for common using queue of "
                    "tasks between main thread and dispatcher thread\n");
    /* Initialize mutex for system */ 
    gateway_scheduler_init_sync();

    GATEWAY_LOG_LOG("Send command 'Start' to 'DestStation' for triggering process. "
                    "Sending data: HEAD = %s\n",
                    header_to_start_deststation);
    /* Send 'start' command to deststation */
    if (-1 == gateway_socket_send(g_gateway_clients[GATEWAY_CLIENT_TYPE_DESTSTATION].socket_fd,
                                  header_to_start_deststation,
                                  sizeof(header_to_start_deststation)))
    {
        err_code = GATEWAY_SEND_IPC_SOCKET_ERROR;
        goto error;
    }
    GATEWAY_LOG_LOG("Command 'Start' was successfully sent\n");

    GATEWAY_LOG_LOG("Activate registered clients\n");
    gateway_clients_activate_registered_clients();
    if (-1 == g_socket_client_fd_max)
    {
        GATEWAY_COMMON_ASSERT(0);
    }
    GATEWAY_LOG_LOG("Registered clients has been just successfully activated\n");

    GATEWAY_LOG_LOG("Proceed to listen of external sending of data or requests from clients\n\n");
    while (g_gateway_clients_active)
    {
        result = select(g_socket_client_fd_max + 1, &g_read_client_sockets_fds, NULL, NULL, 0);
        GATEWAY_LOG_LOG("<<<< Event has been caught!\n");
        if (result < 0)
        {
            GATEWAY_COMMON_ASSERT(0);
        }
        else
        {
            gateway_client_types_t client_type;
            for (client_type = GATEWAY_CLIENT_TYPE_MCLIENT_A;
                 client_type < GATEWAY_CLIENT_TYPES_COUNT;
                 client_type = GATEWAY_CLIENTS_TYPES_NEXT(client_type))
            {
                if (g_gateway_clients[client_type].active_flag &&
                    FD_ISSET(g_gateway_clients[client_type].socket_fd, &g_read_client_sockets_fds))
                {
                    GATEWAY_LOG_LOG("Client type %d sent message. Proceed handle message\n", client_type);
                    g_gateway_clients[client_type].run_handler();
                }
            }
            gateway_clients_reload_fd_set();
        }
    }

    error:

    gateway_scheduler_deinit_sync();

    return err_code;
}

int main(int const argc, char const *argv[])
{
    int socket_fd_local;
    gateway_error_code_t err_code = GATEWAY_SUCCESS;
    unsigned int client_reg_num;

    err_code = gateway_log_init_log();
    if (GATEWAY_SUCCESS != err_code)
    {
        goto error_log;
    }

    GATEWAY_LOG_LOG("Try to create local server socket for listening new external connections...\n");
    /* Create server-gateway socket */
    socket_fd_local = gateway_socket_create_socket();
    if (-1 == socket_fd_local)
    {
        err_code = GATEWAY_CREATE_IPC_SOCKET_ERROR;
        goto error_server;
    }
    GATEWAY_LOG_LOG("Local socket for external client connections has been successfully created. "
                    "SOCKET_FD = %d\n", socket_fd_local);

    GATEWAY_LOG_LOG("Try to bind local server socket...\n");
    /* Bind server-gateway socket */
    if (-1 == gateway_socket_bind_socket(socket_fd_local))
    {
        err_code = GATEWAY_BIND_IPC_SOCKET_ERROR;
        goto error_server;
    }
    GATEWAY_LOG_LOG("Local socket was successfully bound. SOCKET_FD = %d\n", socket_fd_local);

    GATEWAY_LOG_LOG("Listen socket for remote client connections\n");
    /* Listen this server-gateway socket for establishment of new connections */
    for (client_reg_num = 0; client_reg_num < GATEWAY_CLIENT_TYPES_COUNT; client_reg_num++)
    {
        int socket_fd_remote;
        gateway_client_types_t client_type;

        GATEWAY_LOG_LOG("*********\n");
        GATEWAY_LOG_LOG("Listen new...\n");
        if (-1 == gateway_socket_listen_socket(socket_fd_local))
        {
            err_code = GATEWAY_LISTEN_IPC_SOCKET_ERROR;
            goto error_client;
        }
        /* For each connection with new client we get new connection-oriented socket */
        socket_fd_remote = gateway_socket_accept_socket(socket_fd_local);

        GATEWAY_LOG_LOG("Caught connection!\n");
        if (-1 == socket_fd_remote)
        {
            err_code = GATEWAY_ACCEPT_IPC_SOCKET_ERROR;
            goto error_client;
        }
        GATEWAY_LOG_LOG("Client was successfully accepted for connection to GateWay server. "
                        "SOCKET_FD = %d\n", socket_fd_remote);

        GATEWAY_LOG_LOG("Try get its type...\n");
        /* Request client type from the new client */
        client_type = gateway_clients_request_client_type(socket_fd_remote);
        if (-1 == client_type)
        {
            GATEWAY_LOG_LOG("Unknown got client type. Exit with error\n");
            err_code = GATEWAY_RECV_IPC_SOCKET_ERROR;
            goto error_client;
        }
        GATEWAY_LOG_LOG("Success! Client type is %d\n", client_type);
        
        GATEWAY_LOG_LOG("Proceed to register client in the GateWay system. "
                        "SOCKET_FD = %d, client_type = %d, client_reg_num = %d\n",
                        socket_fd_remote,
                        client_type,
                        client_reg_num);
        /* Register new client in the RTS GateWay */
        gateway_clients_register_client(socket_fd_remote, client_type, client_reg_num);
    }

    GATEWAY_LOG_LOG("Begin processing...\n");
    /* Begin processing data */
    err_code = begin_process();

    error_client:

    GATEWAY_LOG_LOG("Remove all registered clients from GateWay system...\n");
    /* Remove all registered earlier clients and close their sockets */
    gateway_clients_remove_registered_clients();
    GATEWAY_LOG_LOG("All clients have been successfully removed from system 'GateWay'\n");

    error_server:

    GATEWAY_LOG_LOG("Close local server socket... SOCKET_FD = %d\n", socket_fd_local);
    /* Close server-gateway socket */
    gateway_socket_close_socket(socket_fd_local);
    GATEWAY_LOG_LOG("Local socket has just been closed\n");

    GATEWAY_LOG_LOG("Finish working system 'GateWay'\n");

    error_log:
    
    gateway_log_deinit_log();

    return err_code;
}
