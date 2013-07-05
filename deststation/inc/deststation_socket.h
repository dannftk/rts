/* encoding: UTF-8 */

#ifndef DESTSTATION_SOCKET_H
#define DESTSTATION_SOCKET_H

#define IPC_SOCKET_NAME "rtos_sock"

int deststation_socket_create_socket(void);
int deststation_socket_connect_socket(int socket_fd_remote);
void deststation_socket_close_socket(int socket_fd);
ssize_t deststation_socket_send(int socket_fd_remote, void *buffer, size_t send_bytes);
ssize_t deststation_socket_receive(int socket_fd_remote, void *buffer, size_t max_recv_bytes, int flags);

#endif /* DESTSTATION_SOCKET_H */