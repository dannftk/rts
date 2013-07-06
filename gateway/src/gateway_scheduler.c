/* encoding: UTF-8 */

#include <pthread.h>
#include <stdio.h>

#include "gateway_common.h"
#include "gateway_scheduler.h"
#include "gateway_tasks.h"

task_queue_t g_task_queue[TASK_QUEUE_SIZE];
unsigned int g_head_queue_ind, g_tail_queue_ind;

pthread_mutex_t queue_mutex;

void gateway_scheduler_init_sync(void)
{
    if (pthread_mutex_init(&queue_mutex, NULL))
    {
        GATEWAY_COMMON_ASSERT(0);
    }
}

void gateway_scheduler_deinit_sync(void)
{
    if (pthread_mutex_destroy(&queue_mutex))
    {
        GATEWAY_COMMON_ASSERT(0);
    }
}

void gateway_scheduler_add_task_to_queue(task_data_t task_data)
{
    /* Protected */
    pthread_mutex_lock(&queue_mutex);

        g_task_queue[g_tail_queue_ind].task_data = task_data;
        g_task_queue[g_tail_queue_ind].priority = 0;

        ++g_tail_queue_ind;
        g_tail_queue_ind %= TASK_QUEUE_SIZE;

        if (g_head_queue_ind == g_tail_queue_ind)
        {
            GATEWAY_COMMON_ASSERT(0);
        }

    pthread_mutex_unlock(&queue_mutex);
}

int gateway_scheduler_get_ptask_from_queue(task_data_t *p_task_data)
{
    int result = 1;

    /* Protected */
    pthread_mutex_lock(&queue_mutex);

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

    pthread_mutex_unlock(&queue_mutex);

    return result;
}