#ifndef __ID_MANAGER_H__
#define __ID_MANAGER_H__

#include <eloop.h>

struct ids_manager;

struct ids_manager *ids_manager_new(id_t max_id);

void ids_manager_delete(struct ids_manager *ids_manager);

id_t ids_manager_get_id(struct ids_manager *manager);

void ids_manager_put_id(struct ids_manager *manager, id_t id);

#endif /* __ID_MANAGER_H__ */
