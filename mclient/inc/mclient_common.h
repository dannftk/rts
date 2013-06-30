/* encoding: UTF-8 */

#ifndef MCLIENT_COMMON_H
#define MCLIENT_COMMON_H

typedef enum mclient_error_code_e {
    MCLIENT_MAIN_SUCCESS = 0,
    MCLIENT_MAIN_WRONG_NUMBER_CLI_PARAMS_ERROR,
    MCLIENT_MAIN_CANT_OPEN_MATRIX_FILE_ERROR,
    MCLIENT_MAIN_CREATE_MTRX_SOCK_ERROR,
    MCLIENT_MAIN_CONNECT_MTRX_SOCK_ERROR,
    MCLIENT_MAIN_SEND_MTRX_SOCK_ERROR
} mclient_error_code_t;


#define MCLIENT_MAIN_ASSERT(condition) assert(condition)

#define MCLIENT_MAIN_MEM_ALLOC(pointer, bytes_count)            \
    do {                                                        \
        if (NULL == pointer)                                    \
        {                                                       \
            pointer = malloc(bytes_count);                      \
            if (NULL == pointer)                                \
            {                                                   \
                MCLIENT_MAIN_ASSERT(0);                         \
            }                                                   \
        }                                                       \
        else                                                    \
        {                                                       \
            MCLIENT_MAIN_ASSERT(0);                             \
        }                                                       \
    } while(0)

#define MCLIENT_MAIN_DEALLOC_MEM(pointer)                       \
    do {                                                        \
        if (pointer)                                            \
        {                                                       \
            free(pointer);                                      \
            pointer = NULL;                                     \
        }                                                       \
        else                                                    \
        {                                                       \
            MCLIENT_MAIN_ASSERT(0);                             \
        }                                                       \
    } while(0)


#endif /* MCLIENT_COMMON_H */