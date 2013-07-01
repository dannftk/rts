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
}

task_data_t const* gateway_scheduler_get_ptask_from_queue(void)
{
    task_data_t *p_task_data;

    if (g_head_queue_ind != g_tail_queue_ind)
    {
        p_task_data = &g_task_queue[g_head_queue_ind].task_data;
        ++g_head_queue_ind;
        g_head_queue_ind %= TASK_QUEUE_SIZE;
    }
    else
    {
        p_task_data = NULL;
    }

    return p_task_data;
}

void gateway_scheduler_process_task(task_data_t task_data)
{
    switch(task_data.task_type)
    {
        case PUT_MTRX_A_VALUE:
            break;
        case PUT_MTRX_C_VALUE:
            break;
        case PUT_VECTOR_b_VALUE:
            break;
        case PUT_VECTOR_d_VALUE:
            break;
        case MUL_MTRX_A_VEC_b_VALUE:
            break;
        case MUL_MTRX_C_VEC_d_VALUE:
            break;
        case ADD_RES_Ab_RES_Cd_VALUE:
            break;
        case SEND_VELUE_TO_DESTSTATION:
            break;
        case END_DESTSTATION:
            break;
        case END_MTRX_A:
            break;
        case END_MTRX_C:
            break;
        case END_VECTOR_b:
            break;
        case END_VECTOR_d:
            break;
        default:
            GATEWAY_COMMON_ASSERT(0);
    }
}