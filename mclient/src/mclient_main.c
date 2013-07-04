/* encoding: UTF-8 */

#include <sys/types.h>

#include <unistd.h>

#include <stdio.h>
#include <stddef.h>

#include <string.h>

#include <time.h>
#include <math.h>

#include "mclient_common.h"
#include "mclient_main.h"
#include "mclient_socket.h"

static int g_mtrx[ROWS][COLUMNS];
static int g_mtrx_col_index[COLUMNS];
static int g_mtrx_col_ind_size = COLUMNS;

static void print_mtrx(void)
{
    int row;
    for (row = 0; row < ROWS; row++)
    {
        int col;
        for (col = 0; col < COLUMNS; col++)
        {
            printf("%d ", g_mtrx[row][col]);
        }
        printf("\n");
    }
}

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
            fscanf(p_mtrx_f, "%d", &g_mtrx[row][col]);

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

static int get_random_time(void)
{
    return MIN_SLEEP_TIME_MLS + rand() * (MAX_SLEEP_TIME_MLS - MIN_SLEEP_TIME_MLS);
}

static void init_random(void)
{
    srand(time(0));
}

static int get_col_mtrx_index(void)
{
    int index_of_mtrx_col_index = (int)(rand() * g_mtrx_col_ind_size);
    int mtrx_col_index = g_mtrx_col_index[index_of_mtrx_col_index];
    g_mtrx_col_index[index_of_mtrx_col_index] = g_mtrx_col_index[g_mtrx_col_ind_size - 1];
    return mtrx_col_index;
}

static enum mclient_error_code_e begin_process(int socket_fd_remote, mclients_types_t mclient_type)
{
    mclient_error_code_t err_code = MCLIENT_SUCCESS;
    send_client_type_data_t send_client_type_data = (send_client_type_data_t) { 
        .header = "typ",
        .mclient_type = mclient_type
    };
    recv_mtrx_row_data_request_t recv_mtrx_row_request;

    /* Send type of client */
    if (-1 == mclient_socket_send(socket_fd_remote,
                                  &send_client_type_data,
                                  sizeof(send_client_type_data)))
    {
        err_code = MCLIENT_SEND_IPC_SOCKET_ERROR;
        goto error;
    }

    init_random();

    /* endless loop for recieving commands from GateWay until 'end' will be occured */
    while (1)
    {
        int ind;

        /* Recieve command from GateWay */
        if (-1 == mclient_socket_receive(socket_fd_remote,
                                         &recv_mtrx_row_request,
                                         sizeof(recv_mtrx_row_request)))
        {
            err_code = MCLIENT_RECV_IPC_SOCKET_ERROR;
            goto error;
        }

        /* If command if "end" then finish processing */
        if (!strcmp(recv_mtrx_row_request.header, "end") &&
            -1 == recv_mtrx_row_request.row)
        {
            break;
        }
        else
        {
            /* If command is not "mtr" then generate error */
            if (strcmp(recv_mtrx_row_request.header, "mtr"))
            {
                err_code = MCLIENT_RECV_WRONG_DATA_FORMAT_ERROR;
                goto error;
            }
        }

        for (ind = 0; ind < COLUMNS; ind++)
        {
            g_mtrx_col_index[ind] = ind;
        }

        while (g_mtrx_col_ind_size > 0)
        {
            int col_mtrx_ind;
            send_mtrx_data_t send_mtrx_data = (send_mtrx_data_t) {
                .header = "mtr"
            };

            sleep(get_random_time());

            col_mtrx_ind = get_col_mtrx_index();

            send_mtrx_data.mtrx = (mtrx_fmt_t) {
                .col_pos = col_mtrx_ind,
                .value = g_mtrx[recv_mtrx_row_request.row][col_mtrx_ind]
            };

            /* Send matrix value */
            mclient_socket_send(socket_fd_remote, &send_mtrx_data, sizeof(send_mtrx_data_t));

            --g_mtrx_col_ind_size;
        }

        g_mtrx_col_ind_size = COLUMNS;
    }

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