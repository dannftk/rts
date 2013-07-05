/* encoding: UTF-8 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "gateway_common.h"
#include "gateway_dispatcher.h"
#include "gateway_scheduler.h"
#include "gateway_tasks.h"

#define DISPATCHER_SLEEP_MS 100

pthread_t disp_thread_id;

extern task_queue_t g_task_queue[TASK_QUEUE_SIZE];
extern unsigned int g_head_queue_ind, g_tail_queue_ind;

static void* process_messages(void *arg)
{
    clock_t time_begin_process, time_end_process;

    time_begin_process = clock();
    while (1)
    {
        task_data_t task_data;
        
        if (gateway_scheduler_get_ptask_from_queue(&task_data))
        {
            int br_fl = SEND_VELUE_TO_DESTSTATION == task_data.task_type;
            gateway_scheduler_process_task(task_data);
            if (br_fl)
            {
                printf("Finish running dispatcher\n");
                break;
            }
        }
        else
        {
            sleep(DISPATCHER_SLEEP_MS);
        }
    }
    time_end_process = clock();

    printf("RTS has completed the task for %3.3f second(s)\n", ((float)(time_end_process - time_begin_process))/CLOCKS_PER_SEC);

    return NULL;
}

void gateway_dispatcher_run_dispatcher(void)
{

    if (!pthread_create(&disp_thread_id, NULL, &process_messages, NULL))
    {
        GATEWAY_COMMON_ASSERT(0);
    }
    else
    {
        printf("Successfully created thread of dispatcher. Process messages from queue...\n");
    }

    return;
}