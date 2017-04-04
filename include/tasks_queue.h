#ifndef __TASKS_QUEQUE_H__
#define __TASKS_QUEQUE_H__

#include "list.h"
#include <stdbool.h>

typedef void (*task_cb_t)(void *ctx);

typedef unsigned int task_id_t;

#define INVALID_TASK_ID 0

struct tasks_queue;

struct tasks_queue *tasks_queue_new();

void tasks_queue_delete(struct tasks_queue *queue, bool execute_all);

task_id_t tasks_queue_add(struct tasks_queue *queue, task_cb_t cb, void *ctx);

void tasks_queue_remove(struct tasks_queue *queue, task_id_t id);

int tasks_queue_execute_next(struct tasks_queue *queue);

#endif /* __TASKS_QUEQUE_H__ */
