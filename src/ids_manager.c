#include <ids_manager.h>

#include <list.h>
#include <stdlib.h>
#include <assert.h>


struct id_node {
    struct list_head list;
    id_t id;
};

struct ids_manager {
    id_t max_id;
    id_t next_id;
    struct list_head available_ids;
    unsigned int available_ids_num;
};

struct ids_manager *ids_manager_new(unsigned int max_id)
{
    struct ids_manager *manager;

    manager = malloc(sizeof(*manager));
    if (!manager)
        return NULL;
   
    assert(max_id);
    
    manager->max_id = max_id;
    manager->next_id = MIN_ID;
    manager->available_ids_num = 0;
    list_init(&manager->available_ids);

    return manager;
}

void ids_manager_delete(struct ids_manager *manager)
{
    struct list_head *itr, *item;

    list_for_each_safe(&manager->available_ids, itr, item) {
        free(item);
        list_del(item);
    }

    free(manager);
}

static struct id_node *id_node_new(id_t id)
{
    struct id_node *id_node = malloc(sizeof(*id_node));
    if (!id)
        return NULL;

    id_node->id = id;
    
    return id_node;
}

static void id_node_register(struct ids_manager *manager,
                             struct id_node *id_node)
{
    list_add(&manager->available_ids, &id_node->list);
    manager->available_ids_num++;
}

static int id_node_allocate(struct ids_manager *manager,
                            id_t id)
{
    struct id_node *id_node = id_node_new(id); 
    if (!id_node)
        return -1;

    id_node_register(manager, id_node);

    return 0;
}

static struct id_node *id_node_get_next(struct ids_manager *manager)
{
    return (struct id_node *)manager->available_ids.next;
}

static void id_node_remove(struct ids_manager *manager,
                           struct id_node *id_node)
{
    list_del(&id_node->list);
    free(id_node);
    manager->available_ids_num--;
}

static id_t get_preallocated_id(struct ids_manager *manager)
{
    struct id_node *id_node;
    id_t id;

    if (!manager->available_ids_num)
        return INVALID_ID;

    id_node = id_node_get_next(manager);
    id = id_node->id;
    id_node_remove(manager, id_node);

    return id;
}

static id_t get_new_id(struct ids_manager *manager)
{
    id_t id = manager->next_id;

    if (manager->next_id == manager->max_id)
        return INVALID_ID;

    manager->next_id++;
    
    return id;
}

id_t ids_manager_get_id(struct ids_manager *manager)
{
    id_t id = get_preallocated_id(manager);
    if (id != INVALID_ID)
        return id;

    return get_new_id(manager);
}

void ids_manager_put_id(struct ids_manager *manager, id_t id)
{
    id_node_allocate(manager, id);
}
