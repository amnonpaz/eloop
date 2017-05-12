#ifndef __ELOOP_H__
#define __ELOOP_H__

#define _GNU_SOURCE

#include <inttypes.h>
#include <stdbool.h>

#define INVALID_ID 0
#define PRIid PRIu64
typedef uint64_t eloop_id_t;

static inline int eloop_id_cmp(eloop_id_t id1, eloop_id_t id2)
{
    if (id1 > id2)
        return 1;

    if (id1 < id2)
        return -1;

    return 0;
}

typedef void (*task_cb_t)(void *ctx);

#endif /* __ELOOP_H__ */

