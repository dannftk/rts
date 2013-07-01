/* encoding: UTF-8 */

#ifndef GATEWAY_CLIENTS_H
#define GATEWAY_CLIENTS_H

#include <sys/types.h>

/* do not change numbers of types of clients */
typedef enum gateway_clients_types_e {
    GATEWAY_CLIENTS_MCLIENT_A = 0x0,
    GATEWAY_CLIENTS_MCLIENT_C = 0x1,
    GATEWAY_CLIENTS_VCLIENT_b = 0x2,
    GATEWAY_CLIENTS_VCLIENT_d = 0x3,
    GATEWAY_CLIENTS_COUNT = 0x4
} gateway_clients_types_t;

typedef struct gateway_clients_s {
    int socket_fd;
    gateway_clients_types_t client_type;
    unsigned int register_number;
    int registered_flag;
    void (*run_handler)(void);
} gateway_clients_t;

typedef struct mtrx_val_pos_s {
    int row;
    int col;
} mtrx_val_pos_t;

typedef struct mtrx_fmt_s {
    mtrx_val_pos_t pos;
    int value;
} mtrx_fmt_t;

typedef struct vector_fmt_s {
    int pos;
    int value;
} vector_fmt_t;

typedef struct recv_vector_data_s {
    char header[4];
    vector_fmt_t vector;
} recv_vector_data_t;

typedef struct recv_mtrx_data_s {
    char header[4];
    mtrx_fmt_t mtrx;
} recv_mtrx_data_t;


#define GATEWAY_CLIENTS_TYPES_NEXT(client_type) (client_type + 1)

enum gateway_clients_types_e gateway_clients_request_client_type(int socket_fd_remote);
void gateway_clients_register_client(int socket_fd_remote,
                                     gateway_clients_types_t client_type,
                                     unsigned int register_number);
void gateway_clients_remove_registered_clients(void);
int gateway_clients_set_registered_clients_onto_fdset(fd_set *readfds);

#endif /* GATEWAY_CLIENTS_H */