#include <tasks_queue.h>
#include <stdio.h>

#define arr_len(arr) \
    (sizeof(arr)/sizeof((arr)[0]))

static void work_func_1(void *ctx)
{
    printf("Function #1 with value %d\n", *(int *)ctx);
}

static void work_func_2(void *ctx)
{
    printf("Function #2 with value %d\n", *(int *)ctx);
}

struct {
    const char *name;
    task_cb_t cb;
    int ctx;
} tasks[] = {
    { "Work01", work_func_1, 0 },
    { "Work02", work_func_1, 0 },
    { "Work03", work_func_2, 0 },
    { "Work04", work_func_2, 0 },
    { "Work05", work_func_1, 0 },
    { "Work06", work_func_1, 0 },
    { "Work07", work_func_2, 0 },
    { "Work08", work_func_2, 0 },
    { "Work09", work_func_1, 0 },
    { "Work10", work_func_2, 0 },
};

static void fill_queue(struct tasks_queue *queue)
{
    int i;

    for (i = 0; i < arr_len(tasks); i++) {
        tasks[i].ctx = i + 1;
        tasks_queue_add(queue, tasks[i].name, tasks[i].cb, &tasks[i].ctx);
    }
}

static void execute_queue(struct tasks_queue *queue, int n)
{
    while (!tasks_queue_execute_next(queue) && !(--n));
}

int main()
{
    struct tasks_queue *queue = tasks_queue_new("My queue");
    if (!queue)
        return 1;

    fill_queue(queue);
    execute_queue(queue, -1);
    tasks_queue_delete(queue, true);

    return 0;
}
