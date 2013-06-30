/* encoding: UTF-8 */

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "mclient_main.h"

#define ROWS 3
#define COLUMNS 3

#define MCLIENT_MAIN_ASSERT(condition) assert(condition)

#define MCLIENT_MAIN_MEM_ALLOC(pointer, bytes_count)    \
    do {                                                \
        if (NULL == pointer)                            \
        {                                               \
            pointer = malloc(bytes_count);              \
            if (NULL == pointer)                        \
            {                                           \
                MCLIENT_MAIN_ASSERT(0);                 \
            }                                           \
        }                                               \
        else                                            \
        {                                               \
            MCLIENT_MAIN_ASSERT(0);                     \
        }                                               \
    } while(0)

#define MCLIENT_MAIN_DEALLOC_MEM(pointer)               \
    do {                                                \
        if (pointer)                                    \
        {                                               \
            free(pointer);                              \
            pointer = NULL;                             \
        }                                               \
        else                                            \
        {                                               \
            MCLIENT_MAIN_ASSERT(0);                     \
        }                                               \
    } while(0)

typedef enum mclient_main_error_code_e {
    MCLIENT_MAIN_SUCCESS = 0,
    MCLIENT_MAIN_WRONG_NUMBER_CLI_PARAMS_ERROR,
    MCLIENT_MAIN_CANT_OPEN_MATRIX_FILE_ERROR
} mclient_main_error_code_t;

static int g_mtrx[ROWS][COLUMNS];

static void print_mtrx(int rows, int cols)
{
    int i;
    for (i = 0; i < rows; i++)
    {
        int j;
        for (j = 0; j < cols; j++)
        {
            printf("%d ", g_mtrx[i][j]);
        }
        printf("\n");
    }
}

static enum mclient_main_error_code_e read_mtrx_from_file(char *mtrx_fp,
                                                          int *rows,
                                                          int *columns)
{
    mclient_main_error_code_t err_code = MCLIENT_MAIN_SUCCESS;
    FILE *p_mtrx_f;

    *rows = 0;
    *columns = 0;

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

        *rows = row;
        *columns = 0 == row ? col : COLUMNS;

        fclose(p_mtrx_f);
    }

    return err_code;
}

int main(int const argc, char const *argv[])
{
    char *mtrx_fp = NULL;
    int mtrx_rows, mtrx_cols;
    size_t mtrx_fp_len;

    mclient_main_error_code_t err_code = MCLIENT_MAIN_SUCCESS;

    if (2 != argc)
    {
        err_code = MCLIENT_MAIN_WRONG_NUMBER_CLI_PARAMS_ERROR;
        goto error;
    }

    mtrx_fp_len = strlen(argv[1]);
    MCLIENT_MAIN_MEM_ALLOC(mtrx_fp, (mtrx_fp_len + 1) * sizeof(char));
    strcpy(mtrx_fp, argv[1]);

    err_code = read_mtrx_from_file(mtrx_fp, &mtrx_rows, &mtrx_cols);
    MCLIENT_MAIN_DEALLOC_MEM(mtrx_fp);
    if (MCLIENT_MAIN_SUCCESS == err_code)
    {
        print_mtrx(mtrx_rows, mtrx_cols);
    }

    error:

    return err_code;
}