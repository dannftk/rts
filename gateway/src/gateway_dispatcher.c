/* encoding: UTF-8 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "gateway_common.h"
#include "gateway_dispatcher.h"
#include "gateway_scheduler.h"
#include "gateway_tasks.h"

#define US_IN_MS 1000
#define DISPATCHER_SLEEP_MS 500

pthread_t disp_thread_id;
FILE *disp_f;

extern task_queue_t g_task_queue[TASK_QUEUE_SIZE];
extern unsigned int g_head_queue_ind, g_tail_queue_ind;

static void process_task(task_data_t task_data)
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
        case MUL_MTRX_A_VALUE_ON_VEC_b_VALUE:
            gateway_tasks_mul_mtrx_A_val_on_vector_b_val(task_data.data.mtrx_col_val_pos);
            break;
        case MUL_MTRX_C_VALUE_ON_VEC_d_VALUE:
            gateway_tasks_mul_mtrx_C_val_on_vector_d_val(task_data.data.mtrx_col_val_pos);
            break;
        case MUL_VECTOR_b_VALUE_ON_MTRX_A_VALUE:
            gateway_tasks_mul_vector_b_val_on_mtrx_A_val(task_data.data.vector_val_pos);
            break;
        case MUL_VECTOR_d_VALUE_ON_MTRX_C_VALUE:
            gateway_tasks_mul_vector_d_val_on_mtrx_C_val(task_data.data.vector_val_pos);
            break;
        case ADD_RES_Ab_VECTOR_VALUE_TO_RES_VECTOR_VALUE:
            gateway_tasks_add_Ab_vector_res_val_to_res_vector_val();
            break;
        case ADD_RES_Cd_VECTOR_VALUE_TO_RES_VECTOR_VALUE:
            gateway_tasks_add_Cd_vector_res_val_to_res_vector_val();
            break;
        case SEND_VELUE_TO_DESTSTATION:
            gateway_tasks_send_value_to_destination_station();
            break;
        default:
            GATEWAY_COMMON_ASSERT(0);
    }
}

void* process_messages(void *arg)
{
    clock_t time_begin_process, time_end_process;

    fprintf(disp_f, "Begin processing queue messages\n");

    time_begin_process = clock();
    while (1)
    {
        task_data_t task_data;

        if (gateway_scheduler_get_ptask_from_queue(&task_data))
        {
            int br_fl = SEND_VELUE_TO_DESTSTATION == task_data.task_type;

            fprintf(disp_f, "Proceed to processing task %d\n", task_data.task_type);
            process_task(task_data);
            if (br_fl)
            {
                fprintf(disp_f, "Finish running dispatcher\n");
                break;
            }
        }
        else
        {
            fprintf(disp_f, "Queue is empty. Sleeping for ms %d...\n", DISPATCHER_SLEEP_MS);
            usleep(DISPATCHER_SLEEP_MS * US_IN_MS);
        }

        fflush(disp_f);
    }

    time_end_process = clock();

    fprintf(disp_f,
            "RTS has completed the task for %3.3f second(s)\n",
            (float)(time_end_process - time_begin_process)/CLOCKS_PER_SEC * 1000);
    fprintf(disp_f, "Finish dispatcher\n\n");

    fclose(disp_f);

    return NULL;
}

void gateway_dispatcher_run_dispatcher(void)
{
    disp_f = fopen("dispatcher.log", "a");
    if (NULL != disp_f)
    {
        if (pthread_create(&disp_thread_id, NULL, &process_messages, NULL))
        {
            GATEWAY_COMMON_ASSERT(0);
        }
        else
        {
            fprintf(disp_f,
                    "Successfully created thread of dispatcher. "
                    "Process messages from queue...\n");
        }
    }
    else
    {
        GATEWAY_COMMON_ASSERT(0);
    }

    return;
}
