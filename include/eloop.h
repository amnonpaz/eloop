#ifndef __ELOOP_H__
#define __ELOOP_H__

#define _GNU_SOURCE

#include <inttypes.h>
#include <stdbool.h>

#define INVALID_ID 0
#define PRIid PRIuPTR
typedef uintptr_t eloop_id_t;

typedef void (*task_cb_t)(void *ctx);

#define arr_len(arr) \
    (sizeof(arr)/sizeof((arr)[0]))

#endif /* __ELOOP_H__ */

