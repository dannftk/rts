/* encoding: UTF-8 */

#include <sys/types.h>

#include <stdio.h>
#include <stddef.h>

#include <string.h>

#include <time.h>
#include <math.h>

#include "vclient_common.h"
#include "vclient_main.h"
#include "vclient_socket.h"

static vector_ftm_t g_vector_fmt[VECTOR_SIZE];
static int g_vector_size = VECTOR_SIZE;

#if 0
static void print_vector(void)
{
    int pos;
    for (pos = 0; pos < VECTOR_SIZE; pos++)
    {
        printf("%d ", g_vector_fmt[pos]);
    }
    printf("\n");
}
#endif

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
            g_vector_fmt[pos].pos = pos;
            fscanf(p_vector_f, "%d", &g_vector_fmt[pos].value);

            ++pos;
        }
        fclose(p_vector_f);
    }

    return err_code;
}

static void init_random(void)
{
    srand(time(0));
}

static int get_vector_index(int vector_size)
{
    return (int)(rand() * vector_size);
}

static enum vclient_error_code_e begin_process(int socket_fd_remote, vclients_types_t vclient_type)
{
    vclient_error_code_t err_code = VCLIENT_SUCCESS;
    send_data_t send_data = (send_data_t){ 
        .header = "sta"
    };

    if (-1 == vclient_socket_send(socket_fd_remote, &vclient_type, sizeof(vclient_type)))
    {
        err_code = VCLIENT_SEND_IPC_SOCKET_ERROR;
        goto error;
    }

    init_random();

    strcpy(send_data.header, "num");
    
    while (g_vector_size > 0)
    {
        int vector_ind = get_vector_index(g_vector_size);
        send_data.data = g_vector_fmt[vector_ind];

        vclient_socket_send(socket_fd_remote, &send_data, sizeof(send_data));

        g_vector_fmt[vector_ind] = g_vector_fmt[g_vector_size - 1];
        --g_vector_size;
    }

    strcpy(send_data.header, "end");
    vclient_socket_send(socket_fd_remote, send_data.header, sizeof(send_data.header));

    error:

    return err_code;
}

int main(int const argc, char const *argv[])
{
    char *vector_fp = NULL;
    size_t vector_fp_len;

    vclient_error_code_t err_code = VCLIENT_SUCCESS;

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
        
        if (-1 == vclient_socket_connect_socket(socket_fd_remote))
        {
            err_code = VCLIENT_CONNECT_IPC_SOCKET_ERROR;
            goto error;
        }

        err_code = begin_process(socket_fd_remote, (vclients_types_t)atoi(argv[2]));

        vclient_socket_close_socket(socket_fd_remote);
    }

    error:

    return err_code;
}