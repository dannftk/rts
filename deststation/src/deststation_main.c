/* encoding: UTF-8 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "deststation_common.h"
#include "deststation_main.h"
#include "deststation_socket.h"

static int g_vector_res[VECTOR_SIZE];
static int g_vector_res_ind[VECTOR_SIZE];
static int g_vector_res_ind_size = VECTOR_SIZE;

static void print_vector(void)
{
    int pos;
    for (pos = 0; pos < VECTOR_SIZE; pos++)
    {
        printf("%d ", g_vector_res[pos]);
    }
    printf("\n");
}

static int get_random_time(void)
{
    return MIN_SLEEP_TIME_MLS + rand() * (MAX_SLEEP_TIME_MLS - MIN_SLEEP_TIME_MLS);
}

static int get_random_vector_index(void)
{
    int index_of_vector_index = (int)(rand() * g_vector_res_ind_size);
    int vector_index = g_vector_res_ind[index_of_vector_index];
    g_vector_res_ind[index_of_vector_index] = g_vector_res_ind[g_vector_res_ind_size - 1];
    return vector_index;
}

static void init_random(void)
{
    srand(time(0));
}

static enum deststation_error_code_e begin_process(int socket_fd_remote)
{
    int ind;
    deststation_error_code_t err_code = DESTSTATION_SUCCESS;
    send_client_type_data_to_gateway_t send_client_type_data = (send_client_type_data_to_gateway_t) {
        .header = "typ",
        .client_type = CLIENT_TYPE_DESTSTATION
    };
    char header_info_for_start_from_gateway[4];
    char header_info_for_end_to_gateway[4];

    /* Send type of client to GateWay */
    if (-1 == deststation_socket_send(socket_fd_remote, &send_client_type_data, sizeof(send_client_type_data)))
    {
        err_code = DESTSTATION_SEND_IPC_SOCKET_ERROR;
        goto error;
    }

    /* Wait for command 'start' from GateWay */
    if (-1 == deststation_socket_receive(socket_fd_remote,
                                         header_info_for_start_from_gateway,
                                         sizeof(header_info_for_start_from_gateway)))
    {
        err_code = DESTSTATION_RECV_IPC_SOCKET_ERROR;
        goto error;
    }

    if (strcmp(header_info_for_start_from_gateway, "sta"))
    {
        err_code = DESTSTATION_RECV_WRONG_DATA_FORMAT_ERROR;
        goto error;
    }

    for (ind = 0; ind < g_vector_res_ind_size; ind++)
    {
        g_vector_res_ind[ind] = ind;
    }

    init_random();

    while (g_vector_res_ind_size > 0)
    {
        int sleep_time, vector_ind;
        send_vector_val_pos_data_to_gateway_t send_vector_val_pos_data = (send_vector_val_pos_data_to_gateway_t) {
            .header = "pos"
        };
        recv_vector_val_data_from_gateway_t recv_vector_val_data;
        
        vector_ind = get_random_vector_index();
        send_vector_val_pos_data.vector_val_pos = vector_ind;

        /* Request for value by index */
        if (-1 == deststation_socket_send(socket_fd_remote,
                                          &send_vector_val_pos_data,
                                          sizeof(send_vector_val_pos_data)))
        {
            err_code = DESTSTATION_SEND_IPC_SOCKET_ERROR;
            goto error;
        }

        sleep_time = get_random_time();
        printf("Sleeping %d\n ms", sleep_time);

        sleep(sleep_time);

        /* Get result of request */
        if (-1 == deststation_socket_receive(socket_fd_remote, 
                                             &recv_vector_val_data,
                                             sizeof(recv_vector_val_data)))
        {
            err_code = DESTSTATION_RECV_IPC_SOCKET_ERROR;
            goto error;
        }

        if (strcmp(recv_vector_val_data.header, "vec"))
        {
            err_code = DESTSTATION_RECV_WRONG_DATA_FORMAT_ERROR;
            goto error;
        }

        g_vector_res[vector_ind] = recv_vector_val_data.vector_val;

        print_vector();
        printf("-------------\n");

        --g_vector_res_ind_size;
    }

    /* Send 'end' command to GateWay */
    if (-1 == deststation_socket_send(socket_fd_remote,
                                      &header_info_for_end_to_gateway,
                                      sizeof(header_info_for_end_to_gateway)))
    {
        err_code = DESTSTATION_SEND_IPC_SOCKET_ERROR;
        goto error;
    }

    error:

    return err_code;
}

int main(int const argc, char const *argv[])
{
    deststation_error_code_t err_code = DESTSTATION_SUCCESS;
    int socket_fd_remote;

    socket_fd_remote = deststation_socket_create_socket();
    if (-1 == socket_fd_remote)
    {
        err_code = DESTSTATION_CREATE_IPC_SOCKET_ERROR;
        goto error;
    }
    
    if (-1 == deststation_socket_connect_socket(socket_fd_remote))
    {
        err_code = DESTSTATION_CONNECT_IPC_SOCKET_ERROR;
    }
    else
    {
        /* Begin process */
        err_code = begin_process(socket_fd_remote);
    }

    deststation_socket_close_socket(socket_fd_remote);

    error:

    return err_code;
}