#include <syncer.h>
#include <tasks_queue.h>
#include <ids_manager.h>

#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#define MAX_READ_LEN 4096

#define COMM_READ   0
#define COMM_WRITE  1

#define TASK_ID_MAX (1<<16)

struct syncer {
    struct ids_manager *ids_manager;
    struct tasks_queue *tasks;
    int comm[2];
    bool stop;
};

#define REQUEST_TYPE_ADD_TASK       1
#define REQUEST_TYPE_CANCEL_TASK    2

#define MAX_TASK_ID (1<<16)

struct task {
    task_cb_t cb;
    void *ctx;
    id_t id;
};

struct request {
    unsigned int type;
    union {
        struct task task;
        id_t id;
    } data;
};

static char read_buffer[MAX_READ_LEN];

struct syncer *syncer_new()
{
    struct syncer *syncer = calloc(1, sizeof(*syncer));
    if (!syncer)
        goto err_alloc;

    syncer->ids_manager = ids_manager_new(TASK_ID_MAX);
    if (!syncer->ids_manager)
        goto err_ids_mgr;

    syncer->tasks = tasks_queue_new();
    if (!syncer->tasks)
        goto err_queue;

    if (pipe2(syncer->comm, O_NONBLOCK))
        goto err_pipe;

    return syncer;

err_pipe:
    tasks_queue_delete(syncer->tasks, false);
err_queue:
    ids_manager_delete(syncer->ids_manager);
err_ids_mgr:
    free(syncer);
err_alloc:
    return NULL;
}

void syncer_delete(struct syncer *syncer)
{
    if (!syncer)
        return;

    if (syncer->comm[COMM_READ] && syncer->comm[COMM_WRITE]) {
        close(syncer->comm[COMM_READ]);
        close(syncer->comm[COMM_WRITE]);
    }

    if (syncer->tasks)
        tasks_queue_delete(syncer->tasks, false);

    free(syncer);
}

static int request_add(struct syncer *syncer,
                       struct request *req)
{
    return write(syncer->comm[COMM_WRITE], req, sizeof(*req));
}

static void syncer_handle_add_task(struct syncer *syncer,
                                   struct task *task)
{
    tasks_queue_add(syncer->tasks, task->cb, task->ctx, task->id);
}

static void syncer_handle_cancel_task(struct syncer *syncer,
                                      id_t task_id)
{
    tasks_queue_remove(syncer->tasks, task_id);
    ids_manager_put_id(syncer->ids_manager, task_id);
}

static void syncer_handle_request(struct syncer *syncer,
                                  struct request *req)
{
    switch (req->type) {
        case REQUEST_TYPE_ADD_TASK:
            syncer_handle_add_task(syncer, &req->data.task);
            break;
        case REQUEST_TYPE_CANCEL_TASK:
            syncer_handle_cancel_task(syncer, req->data.id);
            break;
        default:
            assert(0);
            break;
    }
}

static int syncer_read(struct syncer *syncer, void *ptr, unsigned int size)
{
    return (read(syncer->comm[COMM_READ], ptr, size) <= 0) ?  -1 : 0;
}

static int syncer_read_request(struct syncer *syncer,
                               struct request *req)
{
    return syncer_read(syncer, req, sizeof(*req));
}

static int syncer_process_request(struct syncer *syncer)
{
    struct request *req = (struct request *)read_buffer;

    if (syncer_read_request(syncer, req) < 0)
        return -1;

    syncer_handle_request(syncer, req);

    return 0;
}

void syncer_run(struct syncer *syncer)
{
    syncer->stop = false;

    syncer_process_request(syncer);

    while (!syncer->stop && tasks_queue_pending_tasks(syncer->tasks)) {
        id_t id = tasks_queue_execute_next(syncer->tasks);
        ids_manager_put_id(syncer->ids_manager, id);
        while (!syncer_process_request(syncer));
    }
}

void syncer_stop(struct syncer *syncer)
{
    syncer->stop = true;
}

static void set_add_task_request(struct request *req,
                                 task_cb_t cb,
                                 void *ctx,
                                 id_t id)
{
    req->type = REQUEST_TYPE_ADD_TASK,
    req->data.task.cb = cb;
    req->data.task.ctx = ctx;
    req->data.task.id = id;
}

id_t syncer_task_add(struct syncer *syncer,
                     task_cb_t cb,
                     void *ctx)
{
    id_t id;
    struct request add_req;

    id = ids_manager_get_id(syncer->ids_manager);
    if (id == INVALID_ID)
        return INVALID_ID;

    set_add_task_request(&add_req, cb, ctx, id);
    if (request_add(syncer, &add_req) <= 0) {
        ids_manager_put_id(syncer->ids_manager, id);
        id = INVALID_ID;
    }

    return id;
}

int syncer_task_cancel(struct syncer *syncer,
                       id_t task_id)
{
    struct request cancel_req = {
        .type = REQUEST_TYPE_CANCEL_TASK,
        .data = {
            .id = task_id,
        },
    };

    return request_add(syncer, &cancel_req);
}
