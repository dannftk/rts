/* encoding: UTF-8 */

#include <sys/types.h>

#include <unistd.h>

#include <stdio.h>
#include <stddef.h>

#include <string.h>

#include <time.h>
#include <math.h>

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

static int get_random_vector_index()
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
    deststation_error_code_t err_code = DESTSTATION_SUCCESS;

    send_data_t send_data = (send_data_t) { 
        .header = "req"
    };
    recv_data_t recv_data;
    int ind;

    for (ind = 0; ind < g_vector_res_ind_size; ind++)
    {
        g_vector_res_ind[ind] = ind;
    }

    init_random();

    while (g_vector_res_ind_size > 0)
    {
        int vector_ind = get_random_vector_index();
        send_data.vector_val_pos = vector_ind;

        /* Request */
        if (-1 == deststation_socket_send(socket_fd_remote, &send_data, sizeof(send_data)))
        {
            err_code = DESTSTATION_SEND_IPC_SOCKET_ERROR;
            goto error;
        }

        sleep(get_random_time());

        /* Get result of request */
        if (-1 == deststation_socket_receive(socket_fd_remote, &recv_data, sizeof(recv_data)))
        {
            err_code = DESTSTATION_RECV_IPC_SOCKET_ERROR;
            goto error;
        }

        if (strcmp(recv_data.header, "val"))
        {
            err_code = DESTSTATION_RECV_WRONG_DATA_FORMAT;
            goto error;
        }

        g_vector_res[vector_ind] = recv_data.vector_val;

        print_vector();
        printf("-------------\n");

        --g_vector_res_ind_size;
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
        goto error;
    }

    err_code = begin_process(socket_fd_remote);

    deststation_socket_close_socket(socket_fd_remote);

    error:

    return err_code;
}