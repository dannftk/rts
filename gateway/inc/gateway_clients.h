/* encoding: UTF-8 */

#ifndef GATEWAY_CLIENTS_H
#define GATEWAY_CLIENTS_H

#include <sys/types.h>

/* do not change numbers of types of clients */
typedef enum gateway_client_types_e {
    GATEWAY_CLIENT_TYPE_MCLIENT_A = 0x0,
    GATEWAY_CLIENT_TYPE_MCLIENT_C = 0x1,
    GATEWAY_CLIENT_TYPE_VCLIENT_b = 0x2,
    GATEWAY_CLIENT_TYPE_VCLIENT_d = 0x3,
    GATEWAY_CLIENT_TYPE_DESTSTATION = 0x4,
    GATEWAY_CLIENT_TYPES_COUNT = 0x5
} gateway_client_types_t;

#define GATEWAY_CLIENTS_TYPES_NEXT(client_type) (client_type + 1)

typedef struct gateway_clients_s {
    int socket_fd;
    gateway_client_types_t client_type;
    unsigned int register_number;
    int registered_flag;
    int active_flag;
    void (*run_handler)(void);
} gateway_clients_t;

/******/

typedef struct recv_client_type_data_s {
    char header[4];
    gateway_client_types_t client_type;
} recv_client_type_data_t;

/******/

typedef struct mtrx_fmt_s {
    int col_pos;
    int value;
} mtrx_fmt_t;

typedef struct recv_mtrx_data_s {
    char header[4];
    mtrx_fmt_t mtrx;
} recv_mtrx_data_t;

typedef struct send_mtrx_row_data_request_s {
    char header[4];
    int row;
} send_mtrx_row_data_request_t;

/******/

typedef struct vector_fmt_s {
    int pos;
    int value;
} vector_fmt_t;

typedef struct recv_vector_data_s {
    char header[4];
    vector_fmt_t vector;
} recv_vector_data_t;

/******/

typedef struct recv_vector_pos_from_deststation_data_s {
    char header[4];
    int vector_val_pos;
} recv_vector_pos_from_deststation_data_t;

typedef struct send_vector_value_to_deststation_data_s {
    char header[4];
    int vector_val;
} send_vector_value_to_deststation_data_t;

/******/

enum gateway_client_types_e gateway_clients_request_client_type(int socket_fd_remote);

void gateway_clients_register_client(int socket_fd_remote,
                                     gateway_client_types_t client_type,
                                     unsigned int register_number);

void gateway_clients_remove_registered_clients(void);
void gateway_clients_set_registered_clients_onto_fdset(void);

#endif /* GATEWAY_CLIENTS_H */