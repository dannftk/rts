/* encoding: UTF-8 */

#ifndef DESTSTATION_MAIN_H
#define DESTSTATION_MAIN_H

#define VECTOR_SIZE 7

#define DESTSTATION_SLEEP_TIME_MS 3500
#define MICROSECONDS_IN_MILLISECONDS 1000

#define CLIENT_TYPE_DESTSTATION 0x4

typedef struct send_client_type_data_to_gateway_s {
    char header[4];
    int client_type;
} send_client_type_data_to_gateway_t;

typedef struct send_vector_val_pos_data_to_gateway_s {
    char header[4];
    int vector_val_pos;
} send_vector_val_pos_data_to_gateway_t;

typedef struct recv_vector_val_data_from_gateway_s {
    char header[4];
    int vector_val;
} recv_vector_val_data_from_gateway_t;

#endif /* DESTSTATION_MAIN_H */