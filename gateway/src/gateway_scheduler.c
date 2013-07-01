/* encoding: UTF-8 */

#include <stdio.h>

#include "gateway_common.h"
#include "gateway_scheduler.h"
#include "gateway_tasks.h"

task_queue_t g_task_queue[TASK_QUEUE_SIZE];
unsigned int g_head_queue_ind, g_tail_queue_ind;

void gateway_scheduler_add_task_to_queue(task_data_t task_data)
{
    g_task_queue[g_tail_queue_ind].task_data = task_data;
    g_task_queue[g_tail_queue_ind].priority = 0;

    ++g_tail_queue_ind;
    g_tail_queue_ind %= TASK_QUEUE_SIZE;

    if (g_head_queue_ind == g_tail_queue_ind)
    {
        GATEWAY_COMMON_ASSERT(0);
    }
}

int gateway_scheduler_get_ptask_from_queue(task_data_t *p_task_data)
{
    int result = 1;

    if (g_head_queue_ind != g_tail_queue_ind)
    {
        *p_task_data = g_task_queue[g_head_queue_ind].task_data;
        ++g_head_queue_ind;
        g_head_queue_ind %= TASK_QUEUE_SIZE;
    }
    else
    {
        result = 0;
    }

    return result;
}

void gateway_scheduler_process_task(task_data_t task_data)
{
    switch(task_data.task_type)
    {
        case PUT_MTRX_A_VALUE:
            gateway_tasks_put_mtrx_A_value(task_data.data.mtrx);
            break;
        case PUT_MTRX_C_VALUE:
            gateway_tasks_put_mtrx_C_value(task_data.data.mtrx);
            break;
        case PUT_VECTOR_b_VALUE:
            gateway_tasks_put_vector_b_value(task_data.data.vector);
            break;
        case PUT_VECTOR_d_VALUE:
            gateway_tasks_put_vector_d_value(task_data.data.vector);
            break;
        case MUL_MTRX_A_VEC_b_VALUE:
            gateway_tasks_mul_mtrx_A_val_on_vector_b_val(task_data.data.mtrx_val_pos);
            break;
        case MUL_MTRX_C_VEC_d_VALUE:
            gateway_tasks_mul_mtrx_C_val_on_vector_d_val(task_data.data.mtrx_val_pos);
            break;
        case ADD_RES_Ab_RES_Cd_VALUE:
            gateway_tasks_add_Ab_res_val_Cd_res_val(task_data.data.value_position);
            break;
        case SEND_VELUE_TO_DESTSTATION:
            gateway_tasks_send_value_to_destination_station(task_data.data.value_position);
            break;
        case END_DESTSTATION:
            gateway_tasks_send_end_signal_to_destination_station();
            break;
        case END_MTRX_A:
            gateway_tasks_handle_end_mtrx_A_signal();
            break;
        case END_MTRX_C:
            gateway_tasks_handle_end_mtrx_C_signal();
            break;
        case END_VECTOR_b:
            gateway_tasks_handle_end_vector_b_signal();
            break;
        case END_VECTOR_d:
            gateway_tasks_handle_end_vector_d_signal();
            break;
        default:
            GATEWAY_COMMON_ASSERT(0);
    }
}