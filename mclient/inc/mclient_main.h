/* encoding: UTF-8 */

#ifndef MCLIENT_MAIN_H
#define MCLIENT_MAIN_H

#define MIN_SLEEP_TIME_MLS 200
#define MAX_SLEEP_TIME_MLS 400

#define ROWS 3
#define COLUMNS 3

/* do not change numbers of types of clients */
typedef enum mclients_types_e {
    MCLIENT_A = 0x0,
    MCLIENT_C = 0x1
} mclients_types_t;

typedef struct mtrx_val_pos_s {
    int row;
    int col;
} mtrx_val_pos_t;

typedef struct mtrx_fmt_s {
    mtrx_val_pos_t pos;
    int value;
} mtrx_fmt_t;

typedef struct send_data_s {
    char header[4];
    union {
        mclients_types_t mclient_type;
        mtrx_fmt_t mtrx;
    } data;
} send_data_t;

#endif /* MCLIENT_MAIN_H */