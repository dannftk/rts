/* encoding: UTF-8 */

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "mclient_common.h"
#include "mclient_main.h"
#include "mclient_socket.h"

static int g_mtrx[ROWS][COLUMNS];

#if 0
static void print_mtrx(void)
{
    int i;
    for (i = 0; i < ROWS; i++)
    {
        int j;
        for (j = 0; j < COLUMNS; j++)
        {
            printf("%d ", g_mtrx[i][j]);
        }
        printf("\n");
    }
}
#endif

static enum mclient_error_code_e read_mtrx_from_file(char *mtrx_fp)
{
    mclient_error_code_t err_code = MCLIENT_MAIN_SUCCESS;
    FILE *p_mtrx_f;

    p_mtrx_f = fopen(mtrx_fp, "r");
    if (NULL == p_mtrx_f)
    {
        err_code = MCLIENT_MAIN_CANT_OPEN_MATRIX_FILE_ERROR;
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

int main(int const argc, char const *argv[])
{
    char *mtrx_fp = NULL;
    size_t mtrx_fp_len;

    mclient_error_code_t err_code = MCLIENT_MAIN_SUCCESS;

    if (2 != argc)
    {
        err_code = MCLIENT_MAIN_WRONG_NUMBER_CLI_PARAMS_ERROR;
        goto error;
    }

    mtrx_fp_len = strlen(argv[1]);
    MCLIENT_MAIN_MEM_ALLOC(mtrx_fp, (mtrx_fp_len + 1) * sizeof(char));
    strcpy(mtrx_fp, argv[1]);

    err_code = read_mtrx_from_file(mtrx_fp);
    MCLIENT_MAIN_DEALLOC_MEM(mtrx_fp);
    if (MCLIENT_MAIN_SUCCESS == err_code)
    {
        unsigned int socket_fd_remote;

        socket_fd_remote = mclient_socket_create_socket();
        if (-1 == socket_fd_remote) 
        {
            err_code = MCLIENT_MAIN_CREATE_MTRX_SOCK_ERROR;
            goto error;
        }
        
        if (-1 == mclient_socket_connect_socket(socket_fd_remote))
        {
            err_code = MCLIENT_MAIN_CONNECT_MTRX_SOCK_ERROR;
            goto error;
        }

        mclient_socket_close_socket(socket_fd_remote);
    }

    error:

    return err_code;
}