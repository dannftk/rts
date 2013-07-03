/* encoding: UTF-8 */

#ifndef DESTSTATION_MAIN_H
#define DESTSTATION_MAIN_H

#define MIN_SLEEP_TIME_MLS 200
#define MAX_SLEEP_TIME_MLS 400

#define VECTOR_SIZE 3

typedef struct send_data_s {
    char header[4];
    int vector_val_pos;
} send_data_t;

typedef struct recv_data_s {
	char header[4];
	int vector_val;
} recv_data_t;

#endif /* DESTSTATION_MAIN_H */