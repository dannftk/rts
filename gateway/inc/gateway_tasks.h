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
        mtrx_fmt_t mtrx;
        vector_fmt_t vector;
        mtrx_val_pos_t mtrx_val_pos;
        int value_position;
    } data;
} task_data_t;

void gateway_tasks_put_mtrx_A_value(mtrx_fmt_t mtrx);
void gateway_tasks_put_mtrx_C_value(mtrx_fmt_t mtrx);
void gateway_tasks_put_vector_b_value(vector_fmt_t vector);
void gateway_tasks_put_vector_d_value(vector_fmt_t vector);
void gateway_tasks_mul_mtrx_A_val_on_vector_b_val(mtrx_val_pos_t mtrx_val_pos);
void gateway_tasks_mul_mtrx_C_val_on_vector_d_val(mtrx_val_pos_t mtrx_val_pos);
void gateway_tasks_add_Ab_res_val_Cd_res_val(int value_position);
void gateway_tasks_send_value_to_destination_station(int value_position);
void gateway_tasks_send_end_signal_to_destination_station(void);
void gateway_tasks_handle_end_mtrx_A_signal(void);
void gateway_tasks_handle_end_mtrx_C_signal(void);
void gateway_tasks_handle_end_vector_b_signal(void);
void gateway_tasks_handle_end_vector_d_signal(void);

#endif /* GATEWAY_TASKS_H */