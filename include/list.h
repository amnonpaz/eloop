#ifndef __LIST_H__
#define __LIST_H__

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

#define list_new(list) \
    struct list_head list = { .next = &(list), .prev = &(list) }

#define list_is_empty(list) \
    ((list)->next == (list)->prev)

#define list_for_each(head, itr) \
    for (itr = (head)->next; itr != (head); itr = itr->next)

#define list_for_each_rev(head, itr) \
    for (itr = (head)->prev; itr != (head); itr = itr->prev)

#define list_for_each_safe(head, itr, item) \
    for (item = (head)->next, itr = item->next; \
            item != (head); item = itr, itr = itr->next)

void list_add(struct list_head *head, struct list_head *item);
void list_del(struct list_head *item);

#endif /* __LIST_H__ */
