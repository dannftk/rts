/* encoding: UTF-8 */

#ifndef MCLIENT_MAIN_H
#define MCLIENT_MAIN_H

#define ROWS 3
#define COLUMNS 3

/* do not change numbers of types of clients */
typedef enum mclients_types_e {
    MCLIENT_A = 0x0,
    MCLIENT_C = 0x1
} mclients_types_t;

typedef struct mtrx_ftm_s {
    int row;
    int col;
    int value;
} mtrx_ftm_t;

typedef struct send_data_s {
    char header[4];
    mtrx_ftm_t data;
} send_data_t;

#endif /* MCLIENT_MAIN_H */