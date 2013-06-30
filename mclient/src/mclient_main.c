/* encoding: UTF-8 */

#include <sys/types.h>

#include <stdio.h>
#include <stddef.h>

#include <string.h>

#include <time.h>
#include <math.h>

#include "mclient_common.h"
#include "mclient_main.h"
#include "mclient_socket.h"

static mtrx_ftm_t g_mtrx_fmt[ROWS * COLUMNS];
static int g_mtrx_size = ROWS * COLUMNS;

#if 0
static void print_mtrx(void)
{
    int row;
    for (row = 0; row < ROWS; row++)
    {
        int col;
        for (col = 0; col < COLUMNS; col++)
        {
            printf("%d ", g_mtrx_fmt[row * col + col]);
        }
        printf("\n");
    }
}
#endif

static enum mclient_error_code_e read_mtrx_from_file(char *mtrx_fp)
{
    mclient_error_code_t err_code = MCLIENT_SUCCESS;
    FILE *p_mtrx_f;

    p_mtrx_f = fopen(mtrx_fp, "r");
    if (NULL == p_mtrx_f)
    {
        err_code = MCLIENT_CANT_OPEN_MATRIX_FILE_ERROR;
    }
    else
    {
        int row = 0, col = 0;
        while (!(feof(p_mtrx_f) || row == ROWS))
        {
            g_mtrx_fmt[row * col + col].row = row;
            g_mtrx_fmt[row * col + col].col = col;
            fscanf(p_mtrx_f, "%d", &g_mtrx_fmt[row * col + col].value);

            ++col;
            if (col == COLUMNS)
            {
                ++row;
                col = 0;
            }
        }
        fclose(p_mtrx_f);
    }

    return err_code;
}

static void init_random(void)
{
    srand(time(0));
}

static int get_mtrx_index(int mtrx_size)
{
    return (int)(rand() * mtrx_size);
}

static enum mclient_error_code_e begin_process(int socket_fd_remote, mclients_types_t mclient_type)
{
    mclient_error_code_t err_code = MCLIENT_SUCCESS;
    send_data_t send_data = (send_data_t){ 
        .header = "num"
    };

    if (-1 == mclient_socket_send(socket_fd_remote, &mclient_type, sizeof(mclient_type)))
    {
        err_code = MCLIENT_SEND_IPC_SOCKET_ERROR;
        goto error;
    }

    init_random();

    while (g_mtrx_size > 0)
    {
        int mtrx_ind = get_mtrx_index(g_mtrx_size);
        send_data.data = g_mtrx_fmt[mtrx_ind];

        mclient_socket_send(socket_fd_remote, &send_data, sizeof(send_data));

        g_mtrx_fmt[mtrx_ind] = g_mtrx_fmt[g_mtrx_size - 1];
        --g_mtrx_size;
    }

    strcpy(send_data.header, "end");
    mclient_socket_send(socket_fd_remote, send_data.header, sizeof(send_data.header));

    error:

    return err_code;
}

int main(int const argc, char const *argv[])
{
    char *mtrx_fp = NULL;
    size_t mtrx_fp_len;

    mclient_error_code_t err_code = MCLIENT_SUCCESS;

    if (3 != argc)
    {
        err_code = MCLIENT_WRONG_NUMBER_CLI_PARAMS_ERROR;
        goto error;
    }

    mtrx_fp_len = strlen(argv[1]);
    MCLIENT_COMMON_MEM_ALLOC(mtrx_fp, (mtrx_fp_len + 1) * sizeof(char));
    strcpy(mtrx_fp, argv[1]);

    err_code = read_mtrx_from_file(mtrx_fp);
    MCLIENT_COMMON_DEALLOC_MEM(mtrx_fp);
    if (MCLIENT_SUCCESS == err_code)
    {
        int socket_fd_remote;

        socket_fd_remote = mclient_socket_create_socket();
        if (-1 == socket_fd_remote)
        {
            err_code = MCLIENT_CREATE_IPC_SOCKET_ERROR;
            goto error;
        }
        
        if (-1 == mclient_socket_connect_socket(socket_fd_remote))
        {
            err_code = MCLIENT_CONNECT_IPC_SOCKET_ERROR;
            goto error;
        }

        err_code = begin_process(socket_fd_remote, (mclients_types_t)atoi(argv[2]));

        mclient_socket_close_socket(socket_fd_remote);
    }

    error:

    return err_code;
}