/* encoding: UTF-8 */

#ifndef GATEWAY_SOCKET_H
#define GATEWAY_SOCKET_H

#include "gateway_common.h"

#define IPC_SOCKET_NAME "rtos_sock"

int gateway_socket_create_socket(void);
int gateway_socket_bind_socket(int socket_fd_local, char const *socket_path);
int gateway_socket_listen_socket(int socket_fd_local);
int gateway_socket_accept_socket(int socket_fd_local);
void gateway_socket_close_socket(int socket_fd);

#endif /* GATEWAY_SOCKET_H */