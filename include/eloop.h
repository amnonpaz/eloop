#ifndef __ELOOP_H__
#define __ELOOP_H__

#define _GNU_SOURCE

#include <inttypes.h>
#include <stdbool.h>

#define INVALID_ID 0
#define PRIid PRIu64
typedef uint64_t eloop_id_t;

typedef void (*task_cb_t)(void *ctx);

#endif /* __ELOOP_H__ */

