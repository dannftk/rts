/* encoding: UTF-8 */

#ifndef GATEWAY_TASKS_H
#define GATEWAY_TASKS_H

#include "gateway_clients.h"

typedef enum task_types_e {
    PUT_MTRX_A_VALUE,
    PUT_MTRX_C_VALUE,
    PUT_VECTOR_b_VALUE,
    PUT_VECTOR_d_VALUE,
    MUL_MTRX_A_VEC_b_VALUE,
    MUL_MTRX_C_VEC_d_VALUE,
    ADD_RES_Ab_RES_Cd_VALUE,
    SEND_VELUE_TO_DESTSTATION,
    END_DESTSTATION,
    END_MTRX_A,
    END_MTRX_C,
    END_VECTOR_b,
    END_VECTOR_d
} task_types_t;

typedef struct task_data_s {
    task_types_t task_type;
    union {
        vector_fmt_t vector;
        mtrx_fmt_t mtrx;
    } data;
} task_data_t;

#endif /* GATEWAY_TASKS_H */