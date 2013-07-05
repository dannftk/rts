/* encoding: UTF-8 */

#include <sys/types.h>

#include <unistd.h>

#include <stdio.h>
#include <stddef.h>

#include <string.h>

#include <time.h>
#include <math.h>

#include "vclient_common.h"
#include "vclient_main.h"
#include "vclient_socket.h"

#define MICROSECONDS_IN_MILLISECONDS 1000

static int g_vector[VECTOR_SIZE];
static int g_vector_index[VECTOR_SIZE];
static int g_vector_ind_size;

static void print_vector(void)
{
    int pos;
    for (pos = 0; pos < VECTOR_SIZE; pos++)
    {
        printf("%d ", g_vector[pos]);
    }
    printf("\n");
}

static enum vclient_error_code_e read_vector_from_file(char *vector_fp)
{
    vclient_error_code_t err_code = VCLIENT_SUCCESS;
    FILE *p_vector_f;

    p_vector_f = fopen(vector_fp, "r");
    if (NULL == p_vector_f)
    {
        err_code = VCLIENT_CANT_OPEN_VECTOR_FILE_ERROR;
    }
    else
    {
        int pos = 0;
        while (!(feof(p_vector_f) || pos == VECTOR_SIZE))
        {
            g_vector[pos] = pos;
            fscanf(p_vector_f, "%d", &g_vector[pos]);

            ++pos;
        }
        fclose(p_vector_f);
    }

    return err_code;
}

static int get_random_time(void)
{
    return MIN_SLEEP_TIME_MLS + rand() % (MAX_SLEEP_TIME_MLS - MIN_SLEEP_TIME_MLS);
}

static void init_random(void)
{
    srand(time(0));
}

static int get_vector_index()
{
    int index_of_vector_index = rand() % g_vector_ind_size;
    int vector_index = g_vector_index[index_of_vector_index];
    g_vector_index[index_of_vector_index] = g_vector_index[g_vector_ind_size - 1];
    return vector_index;
}

static enum vclient_error_code_e begin_process(int socket_fd_remote, vclients_types_t vclient_type)
{
    vclient_error_code_t err_code = VCLIENT_SUCCESS;
    send_client_type_data_t send_client_type_data = (send_client_type_data_t) {
        .header = "typ",
        .vclient_type = vclient_type
    };

    /* Send type of client */
    if (-1 == vclient_socket_send(socket_fd_remote,
                                  &send_client_type_data,
                                  sizeof(send_client_type_data)))
    {
        err_code = VCLIENT_SEND_IPC_SOCKET_ERROR;
        goto error;
    }
    printf("Type 'VClient' has been successfully sent to GateWay\n");

    init_random();

    while (1)
    {
        char header_info_vector_data_request[4];
        int ind;

        printf("Wait new commands from GateWay...\n");
        fflush(stdout);
        /* Recieve command from GateWay */
        if (-1 == vclient_socket_receive(socket_fd_remote,
                                         header_info_vector_data_request,
                                         sizeof(header_info_vector_data_request)))
        {
            err_code = VCLIENT_RECV_IPC_SOCKET_ERROR;
            goto error;
        }

        /* If command if "end" then finish processing */
        if (!strcmp(header_info_vector_data_request, "end"))
        {
            printf("Received command 'end' from GateWay. Finish processing and close remote connection\n");
            break;
        }
        else
        {
            if (strcmp(header_info_vector_data_request, "vec"))
            {
                printf("Received unknown command from GateWay. Exit with error\n");
                err_code = VCLIENT_RECV_WRONG_DATA_FORMAT_ERROR;
                goto error;
            }
        }

        printf("*****************\n");
        printf("Recieved command 'vec' with request of vector\n");

        for (ind = 0; ind < VECTOR_SIZE; ind++)
        {
            g_vector_index[ind] = ind;
        }

        g_vector_ind_size = VECTOR_SIZE;

        while (g_vector_ind_size > 0)
        {
            int vector_index;
            send_vector_data_t send_vector_data = (send_vector_data_t) {
                .header = "vec"
            };

            vector_index = get_vector_index();

            send_vector_data.vector = (vector_fmt_t) {
                .pos = vector_index,
                .value = g_vector[vector_index]
            };

            printf("Data for sending was configured. HEAD = %s, POS = %d, VAL = %d\n. Wait for sending...\n",
                   send_vector_data.header,
                   send_vector_data.vector.pos,
                   send_vector_data.vector.value);

            usleep(get_random_time() * MICROSECONDS_IN_MILLISECONDS);

            /* Send vector value */
            if (-1 == vclient_socket_send(socket_fd_remote, &send_vector_data, sizeof(send_vector_data)))
            {
                VCLIENT_COMMON_ASSERT(0);
            }
            printf("Data has been successfully sent\n");

            --g_vector_ind_size;
        }

        printf("\nAll elements of vector was successfully sent\n");

    }

    error:

    printf("Finishing processing of 'VClient'\n");

    return err_code;
}

int main(int const argc, char const *argv[])
{
    char *vector_fp = NULL;
    size_t vector_fp_len;

    vclient_error_code_t err_code = VCLIENT_SUCCESS;

    printf("VClient parameters:\n");

    int i;
    for (i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    printf("\n");

    if (3 != argc)
    {
        err_code = VCLIENT_WRONG_NUMBER_CLI_PARAMS_ERROR;
        goto error;
    }

    vector_fp_len = strlen(argv[1]);
    VCLIENT_COMMON_MEM_ALLOC(vector_fp, (vector_fp_len + 1) * sizeof(char));
    strcpy(vector_fp, argv[1]);

    err_code = read_vector_from_file(vector_fp);
    VCLIENT_COMMON_DEALLOC_MEM(vector_fp);
    if (VCLIENT_SUCCESS == err_code)
    {
        int socket_fd_remote;

        socket_fd_remote = vclient_socket_create_socket();
        if (-1 == socket_fd_remote)
        {
            err_code = VCLIENT_CREATE_IPC_SOCKET_ERROR;
            goto error;
        }
        printf("Socket for connection with GateWay has been created. SOCKET_FD = %d\n", socket_fd_remote);
        
        if (-1 == vclient_socket_connect_socket(socket_fd_remote))
        {
            err_code = VCLIENT_CONNECT_IPC_SOCKET_ERROR;
            goto error;
        }
        printf("VClient was successfully connected to remote GateWay server. SOCKET_FD = %d\n", socket_fd_remote);

        printf("Begin processing...\n");
        err_code = begin_process(socket_fd_remote, (vclients_types_t)atoi(argv[2]));

        vclient_socket_close_socket(socket_fd_remote);
        printf("Socket for remote connection with GateWay has just been closed. SOCKET_FD = %d\n", socket_fd_remote);
    }

    error:

    printf("Finishing 'VClient'\n");

    return err_code;
}