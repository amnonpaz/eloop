#ifndef __FD_H__
#define __FD_H__

#include <eloop.h>

#include "syncer.h"

struct fd_handler;

#define FD_EVENT_ERROR 0x01
#define FD_EVENT_HUP   0x02
#define FD_EVENT_READ  0x04
#define FD_EVENT_WRITE 0x08

struct fd_handler *fd_handler_new(struct syncer *syncer,
                                  uint32_t max_events);

void fd_handler_delete(struct fd_handler *handler);

eloop_id_t fd_handler_add_fd(struct fd_handler *handler,
                             int fd,
                             fd_cb_t cb,
                             void *ctx);

void fd_handler_remove_fd(struct fd_handler *handler,
                          eloop_id_t fd_id);

void fd_handler_handle_events(struct fd_handler *handler);

#endif /* __FD_H__ */
