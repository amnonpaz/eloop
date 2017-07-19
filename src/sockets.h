#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#include <eloop.h>

struct sockets_handler;

#define SOCKET_EVENT_ERROR 0x01
#define SOCKET_EVENT_HUP   0x02
#define SOCKET_EVENT_READ  0x04
#define SOCKET_EVENT_WRITE 0x08

typedef void (*socket_cb_t)(void *ctx, uint32_t event_mask);

struct sockets_handler *socket_handler_new(uint32_t max_events);

void sockets_handler_delete(struct sockets_handler *handler);

eloop_id_t sockets_handler_add_socket(struct sockets_handler *handler,
                                      int fd,
                                      socket_cb_t cb,
                                      void *ctx);

void sockets_handler_remove_socket(struct sockets_handler *handler,
                                   eloop_id_t socket_id);

void sockets_handler_handle_events(struct sockets_handler *handler);

#endif /* __SOCKETS_H__ */
