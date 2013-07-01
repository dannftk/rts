#ifndef GATEWAY_SCHEDULER_H
#define GATEWAY_SCHEDULER_H

#include "gateway_tasks.h"

#define TASK_QUEUE_SIZE 50

typedef struct task_queue_s {
	task_data_t task_data;
	unsigned int priority;
} task_queue_t;

void gateway_scheduler_add_task_to_queue(task_data_t task_data);
task_data_t const* gateway_scheduler_get_ptask_from_queue(void);
void gateway_scheduler_process_task(task_data_t task_data);

#endif /* GATEWAY_SCHEDULER_H */