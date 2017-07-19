#include "sockets.h"
#include "list.h"

#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

struct sockets_handler {
    struct list_head sockets_list;
    int poll_fd;
    uint32_t max_events;
    struct epoll_event* events_buffer;
};

struct socket_data {
    struct list_head list;
    int fd;
    socket_cb_t cb;
    void *ctx;
};

static struct socket_data *socket_data_new(struct sockets_handler *handler,
                                           int fd,
                                           socket_cb_t cb,
                                           void *ctx)
{
    struct socket_data *socket_data = malloc(sizeof(*socket_data));
    if (!socket_data)
        return NULL;

    list_add(&handler->sockets_list, &socket_data->list);
    socket_data->fd = fd;
    socket_data->cb = cb;
    socket_data->ctx = ctx;

    return socket_data;
}

static void socket_data_delete(struct socket_data *socket_data)
{
    list_del(&socket_data->list);
    free(socket_data);
}

struct sockets_handler *socket_handler_new(uint32_t max_events)
{
    struct sockets_handler *handler =
        (struct sockets_handler *)malloc(sizeof(*handler));
    if (!handler)
        return NULL;

    handler->max_events = max_events;
    handler->events_buffer = calloc(handler->max_events,
                                    sizeof(struct epoll_event));
    if (!handler->events_buffer)
    {
        free(handler);
        return NULL;
    }

    list_init(&handler->sockets_list);

    handler->poll_fd = epoll_create1(0);
    if (handler->poll_fd < 0)
    {
        free(handler->events_buffer);
        free(handler);
        return NULL;
    }

    return handler;
}

void sockets_handler_delete(struct sockets_handler *handler)
{
    struct list_head *itr, *item;

    list_for_each_safe(&handler->sockets_list, itr, item)
        sockets_handler_remove_socket(handler, (eloop_id_t)item);

    close(handler->poll_fd);
    free(handler->events_buffer);
    free(handler);
}

eloop_id_t sockets_handler_add_socket(struct sockets_handler *handler,
                                      int fd,
                                      socket_cb_t cb,
                                      void *ctx)
{
    struct epoll_event event_config;
    struct socket_data *socket_data = socket_data_new(handler, fd, cb, ctx);
    if (!socket_data)
        return INVALID_ID;

    event_config.data.ptr = socket_data;
    event_config.events = EPOLLIN | EPOLLRDHUP | EPOLLET;

    if (epoll_ctl(handler->poll_fd,
                  EPOLL_CTL_ADD,
                  socket_data->fd,
                  &event_config) < 0) {
        socket_data_delete(socket_data);
        return INVALID_ID;
    }

    return (eloop_id_t)socket_data;
}

void sockets_handler_remove_socket(struct sockets_handler *handler,
                                   eloop_id_t socket_id)
{
    struct socket_data *socket_data = (struct socket_data *)socket_id;

    epoll_ctl(handler->poll_fd, EPOLL_CTL_DEL, socket_data->fd, NULL);
    socket_data_delete(socket_data);
}

void sockets_handler_handle_events(struct sockets_handler *handler)
{
    uint32_t i;
    int num_events = epoll_wait(handler->poll_fd,
                                handler->events_buffer,
                                handler->max_events,
                                0);
    if (num_events <= 0)
        return;

    for (i = 0; i < num_events; i++) {
        struct socket_data* socket_data =
            (struct socket_data* )handler->events_buffer[i].data.ptr;
        uint32_t current_event = handler->events_buffer[i].events;

        uint32_t event_mask = 0;

        if (current_event & EPOLLERR) {
            event_mask = SOCKET_EVENT_ERROR;
        } else if ((current_event & EPOLLHUP) ||
                   (current_event & EPOLLRDHUP)) {
            event_mask = SOCKET_EVENT_HUP;
        } else {
            if (current_event & EPOLLIN)
                event_mask |= SOCKET_EVENT_READ;
            if (current_event & EPOLLOUT)
                event_mask |= SOCKET_EVENT_WRITE;
        }

        socket_data->cb(socket_data->ctx, event_mask);
    }
}
