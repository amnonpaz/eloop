#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "tasks_queue.h"

struct syncer;

struct syncer *syncer_new(void);

void syncer_delete(struct syncer *syncer);

void syncer_run(struct syncer *syncer);

void syncer_stop(struct syncer *syncer);

int syncer_task_add(struct syncer *syncer,
                    task_cb_t cb,
                    void *ctx,
                    task_id_t id);

int syncer_task_cancel(struct syncer *syncer,
                       task_id_t task_id);

#endif /* __SCHEDULER_H__ */
