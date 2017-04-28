#include <syncer.h>

#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#define MAX_READ_LEN 4096

#define COMM_READ   0
#define COMM_WRITE  1

struct syncer {
    struct tasks_queue *tasks;
    int comm[2];
    bool stop;
};

#define REQUEST_TYPE_ADD_TASK       1
#define REQUEST_TYPE_CANCEL_TASK    2

#define MAX_TASK_ID (1<<16)

struct request {
    unsigned int type;
    char payload[0];
};

struct task {
    task_cb_t cb;
    void *ctx;
    task_id_t id;
};

static char read_buffer[MAX_READ_LEN];

static unsigned int request_calc_payload_size(unsigned int type)
{
    unsigned int size = 0;

    switch (type) {
        case REQUEST_TYPE_ADD_TASK:
            size += sizeof(struct task);
            break;
        case REQUEST_TYPE_CANCEL_TASK:
            size += sizeof(task_id_t);
            break;
        default:
            break;
    }

    return size;
}

static int request_add(struct syncer *syncer,
                       unsigned int type,
                       void *data,
                       unsigned int len)
{
    struct request *req;
    unsigned int payload_size = request_calc_payload_size(type),
                 req_size = sizeof(*req) + payload_size;

    assert(payload_size == len);
    /* "data" and "len" must both be valid or
     * both be invalid
     */
    assert((!data || len) && (data || !len));

    req = malloc(req_size);
    if (!req)
        return -1;

    req->type = type;
    if (data)
        memcpy(&req->payload[0], data, len);
    write(syncer->comm[COMM_WRITE], req, req_size);

    free(req);

    return 0;
}

struct syncer *syncer_new()
{
    struct syncer *syncer = calloc(1, sizeof(*syncer));
    if (!syncer)
        goto err_alloc;

    syncer->tasks = tasks_queue_new();
    if (!syncer->tasks)
        goto err_queue;

    if (pipe2(syncer->comm, O_NONBLOCK))
        goto err_pipe;

    return syncer;

err_pipe:
    tasks_queue_delete(syncer->tasks, false);
err_queue:
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

static void syncer_handle_add_task(struct syncer *syncer,
                                   struct task *task)
{
    tasks_queue_add(syncer->tasks, task->cb, task->ctx, task->id);
}

static void syncer_handle_cancel_task(struct syncer *syncer,
                                      unsigned int task_id)
{
    tasks_queue_remove(syncer->tasks, task_id);
}

static void syncer_handle_request(struct syncer *syncer,
                                  struct request *req)
{
    switch (req->type) {
        case REQUEST_TYPE_ADD_TASK:
            syncer_handle_add_task(syncer, (struct task *)&req->payload[0]);
            break;
        case REQUEST_TYPE_CANCEL_TASK:
            syncer_handle_cancel_task(syncer, *(unsigned int *)&req->payload[0]);
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

static int syncer_read_request_header(struct syncer *syncer,
                                      struct request *req)
{
    return syncer_read(syncer, req, sizeof(*req));
}

static int syncer_read_request_body(struct syncer *syncer,
                                    struct request *req)
{
    return syncer_read(syncer,
                       &req->payload[0],
                       request_calc_payload_size(req->type));
}

static int syncer_process_request(struct syncer *syncer)
{
    struct request *req = (struct request *)read_buffer;

    if (syncer_read_request_header(syncer, req) < 0)
        return -1;

    if (syncer_read_request_body(syncer, req) < 0)
        return -1;

    syncer_handle_request(syncer, req);

    return 0;
}

void syncer_run(struct syncer *syncer)
{
    syncer_process_request(syncer);

    while (!syncer->stop && tasks_queue_pending_tasks(syncer->tasks)) {
        tasks_queue_execute_next(syncer->tasks);
        while (!syncer_process_request(syncer));
    }
}

void syncer_stop(struct syncer *syncer)
{
    syncer->stop = true;
}

int syncer_task_add(struct syncer *syncer,
                    task_cb_t cb,
                    void *ctx,
                    task_id_t id)
{
    struct task new_task = {
        .cb = cb,
        .ctx = ctx,
        .id = id,
    };

    return request_add(syncer,
                       REQUEST_TYPE_ADD_TASK,
                       &new_task,
                       sizeof(new_task));
}

int syncer_task_cancel(struct syncer *syncer,
                       task_id_t task_id)
{
    return request_add(syncer,
                       REQUEST_TYPE_CANCEL_TASK,
                       &task_id,
                       sizeof(task_id));
}
