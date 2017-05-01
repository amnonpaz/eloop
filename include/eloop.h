#ifndef __ELOOP_H__
#define __ELOOP_H__

#define INVALID_TASK_ID 0

typedef unsigned int task_id_t;

typedef void (*task_cb_t)(void *ctx);

#endif /* __ELOOP_H__ */

