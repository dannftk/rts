/* encoding: UTF-8 */

#ifndef GATEWAY_CLIENTS_H
#define GATEWAY_CLIENTS_H

/* do not change numbers of types of clients */
typedef enum gateway_clients_types_e {
    GATEWAY_CLIENTS_MCLIENT_A = 0x0,
    GATEWAY_CLIENTS_MCLIENT_C = 0x1,
    GATEWAY_CLIENTS_VCLIENT_b = 0x2,
    GATEWAY_CLIENTS_VCLIENT_d = 0x3,
    GATEWAY_CLIENTS_COUNT
} gateway_clients_types_t;

typedef struct gateway_clients_s {
    int socket_fd;
    gateway_clients_types_t client_type;
    unsigned int register_number;
    int registered_flag;
    /* TODO: handler */
} gateway_clients_t;

enum gateway_clients_types_e gateway_clients_request_client_type(int socket_fd_remote);
void gateway_clients_register_client(int socket_fd_remote,
                                     gateway_clients_types_t client_type,
                                     unsigned int register_number);
void gateway_clients_remove_registered_clients(void);

#endif /* GATEWAY_CLIENTS_H */