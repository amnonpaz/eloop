#ifndef __ELOOP_H__
#define __ELOOP_H__

#define _GNU_SOURCE

#include <inttypes.h>
#include <stdbool.h>

#define INVALID_ID 0
#define PRIid PRIuPTR
typedef uintptr_t eloop_id_t;

typedef void (*task_cb_t)(void *ctx);
typedef void (*fd_cb_t)(void *ctx, uint32_t event_mask);


#define ELOOP_FD_EVENT_ERROR 0x01
#define ELOOP_FD_EVENT_HUP   0x02
#define ELOOP_FD_EVENT_READ  0x04
#define ELOOP_FD_EVENT_WRITE 0x08

#endif /* __ELOOP_H__ */
