/* encoding: UTF-8 */

#ifndef VCLIENT_MAIN_H
#define VCLIENT_MAIN_H

#define MIN_SLEEP_TIME_MLS 100
#define MAX_SLEEP_TIME_MLS 300

#define VECTOR_SIZE 3

/* do not change numbers of types of clients */
typedef enum vclients_types_e {
    VCLIENT_b = 0x0,
    VCLIENT_d = 0x1
} vclients_types_t;

typedef struct vector_fmt_s {
    int pos;
    int value;
} vector_fmt_t;

typedef struct send_data_s {
    char header[4];
    union {
        vclients_types_t vclient_type;
        vector_fmt_t vector;
    } data;
} send_data_t;

#endif /* VCLIENT_MAIN_H */