/* encoding: UTF-8 */

#include <sys/socket.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "deststation_common.h"
#include "deststation_log.h"
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
        DESTSTATION_LOG_LOG("%d ", g_vector_res[pos]);
    }
    DESTSTATION_LOG_LOG("\n");
}

static int get_random_vector_index(void)
{
    int index_of_vector_index = rand() % g_vector_res_ind_size;
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
    send_vector_val_pos_data_to_gateway_t send_vector_val_pos_data = {
            .header = "pos"
    };
    send_client_type_data_to_gateway_t send_client_type_data = {
        .header = "typ",
        .client_type = CLIENT_TYPE_DESTSTATION
    };
    char header_info_for_start_from_gateway[4];

    /* Send type of client to GateWay */
    if (-1 == deststation_socket_send(socket_fd_remote, &send_client_type_data, sizeof(send_client_type_data)))
    {
        err_code = DESTSTATION_SEND_IPC_SOCKET_ERROR;
        goto error;
    }
    DESTSTATION_LOG_LOG("Type 'DestStation' has been successfully sent to GateWay\n");

    DESTSTATION_LOG_LOG("Wait new command 'Start' from GateWay...\n");
    /* Wait for command 'start' from GateWay */
    if (-1 == deststation_socket_receive(socket_fd_remote,
                                         header_info_for_start_from_gateway,
                                         sizeof(header_info_for_start_from_gateway),
                                         0x0))
    {
        err_code = DESTSTATION_RECV_IPC_SOCKET_ERROR;
        goto error;
    }

    if (strcmp(header_info_for_start_from_gateway, "sta"))
    {
        DESTSTATION_LOG_LOG("Received unknown command from GateWay. Exit with error\n");
        err_code = DESTSTATION_RECV_WRONG_DATA_FORMAT_ERROR;
        goto error;
    }

    DESTSTATION_LOG_LOG("*****************\n");
    DESTSTATION_LOG_LOG("Received command 'Start' for processing of sequence of requests\n");

    for (ind = 0; ind < VECTOR_SIZE; ind++)
    {
        g_vector_res_ind[ind] = ind;
    }

    init_random();

    while (g_vector_res_ind_size > 0)
    {
        int vector_ind;
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

        DESTSTATION_LOG_LOG("Data for sending request was configured. HEAD = %s, POS = %d\n. Wait for sending...\n",
        send_vector_val_pos_data.header,
        send_vector_val_pos_data.vector_val_pos);

        DESTSTATION_LOG_LOG("Sleeping %d ms\n", DESTSTATION_SLEEP_TIME_MS);

        usleep(DESTSTATION_SLEEP_TIME_MS * MICROSECONDS_IN_MILLISECONDS);

        DESTSTATION_LOG_LOG("Try get result from GateWay RTS...\n");
        fflush(stdout);
        /* Get result of request */
        if (-1 == deststation_socket_receive(socket_fd_remote,
                                             &recv_vector_val_data,
                                             sizeof(recv_vector_val_data),
                                             MSG_DONTWAIT))
        {
            DESTSTATION_LOG_LOG("Fatal Error. Real-Time error occured\n");
            err_code = DESTSTATION_RECV_IPC_SOCKET_ERROR;
            goto error_real_time;
        }
        DESTSTATION_LOG_LOG("Result has been receieved from GateWay\n");

        if (strcmp(recv_vector_val_data.header, "vec"))
        {
            DESTSTATION_LOG_LOG("Received unknown result from GateWay. Exit with error\n");
            err_code = DESTSTATION_RECV_WRONG_DATA_FORMAT_ERROR;
            goto error;
        }

        DESTSTATION_LOG_LOG("Result: %d for vector index %d\n", recv_vector_val_data.vector_val, vector_ind);

        g_vector_res[vector_ind] = recv_vector_val_data.vector_val;

        DESTSTATION_LOG_LOG("Vector:\n");
        print_vector();
        DESTSTATION_LOG_LOG("-------------\n");

        --g_vector_res_ind_size;
    }

    error_real_time:

    DESTSTATION_LOG_LOG("Processing of sequence of requests is finished. Send 'end' command to GateWay\n");

    send_vector_val_pos_data = (send_vector_val_pos_data_to_gateway_t) {
            .header = "end",
            .vector_val_pos = -1
    };

    /* Send 'end' command to GateWay */
    if (-1 == deststation_socket_send(socket_fd_remote,
                                      &send_vector_val_pos_data,
                                      sizeof(send_vector_val_pos_data)))
    {
        err_code = DESTSTATION_SEND_IPC_SOCKET_ERROR;
        goto error;
    }
    DESTSTATION_LOG_LOG("Command 'end' has been successfully sent to GateWay\n");

    error:

    DESTSTATION_LOG_LOG("Finishing processing of 'DestStation'\n");

    return err_code;
}

int main(int const argc, char const *argv[])
{
    deststation_error_code_t err_code = DESTSTATION_SUCCESS;
    int socket_fd_remote;

    err_code = deststation_log_init_log();
    if (DESTSTATION_SUCCESS != err_code)
    {
        goto error_log;
    }

    DESTSTATION_LOG_LOG("Try to create remote socket for connection with RTS GateWay...\n");
    socket_fd_remote = deststation_socket_create_socket();
    if (-1 == socket_fd_remote)
    {
        err_code = DESTSTATION_CREATE_IPC_SOCKET_ERROR;
        goto error;
    }
    DESTSTATION_LOG_LOG("Socket for connection with GateWay has been created. SOCKET_FD = %d\n", socket_fd_remote);

    DESTSTATION_LOG_LOG("Try to connect to remote RTS GateWay...\n");
    if (-1 == deststation_socket_connect_socket(socket_fd_remote))
    {
        err_code = DESTSTATION_CONNECT_IPC_SOCKET_ERROR;
    }
    else
    {
        DESTSTATION_LOG_LOG("Destination Station was successfully connected to remote GateWay server. SOCKET_FD = %d\n", socket_fd_remote);

        DESTSTATION_LOG_LOG("Begin processing...\n");
        /* Begin process */
        err_code = begin_process(socket_fd_remote);
    }

    deststation_socket_close_socket(socket_fd_remote);
    DESTSTATION_LOG_LOG("Socket for remote connection with GateWay has just been closed. SOCKET_FD = %d\n", socket_fd_remote);

    error:

    DESTSTATION_LOG_LOG("Finishing 'DestStation'\n");

    error_log:

    deststation_log_deinit_log();

    return err_code;
}
