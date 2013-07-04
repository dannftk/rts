/* encoding: UTF-8 */

#ifndef VCLIENT_CLIENT_H
#define VCLIENT_CLIENT_H

#include <stddef.h>

#define IPC_SOCKET_NAME "rtos_sock"

int vclient_socket_create_socket(void);
int vclient_socket_connect_socket(int socket_fd_remote);
void vclient_socket_close_socket(int socket_fd);
ssize_t vclient_socket_send(int socket_fd_remote, void *buffer, size_t send_bytes);
ssize_t vclient_socket_receive(int socket_fd_remote, void *buffer, size_t max_recv_bytes);

#endif /* VCLIENT_CLIENT_H */