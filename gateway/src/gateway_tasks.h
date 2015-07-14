/* encoding: UTF-8 */

#ifndef GATEWAY_TASKS_H
#define GATEWAY_TASKS_H

#include "gateway_clients.h"

typedef enum task_types_e {
/* TASK = 0 */    PUT_MTRX_A_VALUE,
/* TASK = 1 */    PUT_MTRX_C_VALUE,
/* TASK = 2 */    PUT_VECTOR_b_VALUE,
/* TASK = 3 */    PUT_VECTOR_d_VALUE,
/* TASK = 4 */    MUL_MTRX_A_VALUE_ON_VEC_b_VALUE,
/* TASK = 5 */    MUL_MTRX_C_VALUE_ON_VEC_d_VALUE,
/* TASK = 6 */    MUL_VECTOR_b_VALUE_ON_MTRX_A_VALUE,
/* TASK = 7 */    MUL_VECTOR_d_VALUE_ON_MTRX_C_VALUE,
/* TASK = 8 */    ADD_RES_Ab_VECTOR_VALUE_TO_RES_VECTOR_VALUE,
/* TASK = 9 */    ADD_RES_Cd_VECTOR_VALUE_TO_RES_VECTOR_VALUE,
/* TASK = 10 */   SEND_VELUE_TO_DESTSTATION
} task_types_t;

typedef struct task_data_s {
    task_types_t task_type;
    union {
        mtrx_fmt_t mtrx;
        vector_fmt_t vector;
        int mtrx_col_val_pos;
        int vector_val_pos;
    } data;
} task_data_t;

void gateway_tasks_put_mtrx_A_value(mtrx_fmt_t mtrx);
void gateway_tasks_put_mtrx_C_value(mtrx_fmt_t mtrx);
void gateway_tasks_put_vector_b_value(vector_fmt_t vector);
void gateway_tasks_put_vector_d_value(vector_fmt_t vector);
void gateway_tasks_mul_mtrx_A_val_on_vector_b_val(int mtrx_col_val_pos);
void gateway_tasks_mul_mtrx_C_val_on_vector_d_val(int mtrx_col_val_pos);
void gateway_tasks_mul_vector_b_val_on_mtrx_A_val(int vector_val_pos);
void gateway_tasks_mul_vector_d_val_on_mtrx_C_val(int vector_val_pos);
void gateway_tasks_add_Ab_vector_res_val_to_res_vector_val(void);
void gateway_tasks_add_Cd_vector_res_val_to_res_vector_val(void);
void gateway_tasks_send_value_to_destination_station(void);

#endif /* GATEWAY_TASKS_H */