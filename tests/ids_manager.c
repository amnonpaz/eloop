#include <ids_manager.h>

#include <stdio.h>

#define ID_MAX 30

static void allocate_ids(struct ids_manager *manager, int n)
{
    int i;

    printf("Allocating IDs: ");
    for (i = 0; i < n; i++) {
        id_t id = ids_manager_get_id(manager);
        if (id != INVALID_ID)
            printf("%u ", id);
        else {
            printf("[Failed]");
            break;
        }            
    }

    printf("\n");
}

static void release_ids(struct ids_manager *manager,
                        id_t start_id, id_t end_id)
{
    id_t id;

    printf("Releasing IDs: ");
    for (id = start_id; id <= end_id; id++) {
        ids_manager_put_id(manager, id);
        printf("%u ", id);
    }

    printf("\n");
}

int main()
{
    struct ids_manager *manager;
    
    manager = ids_manager_new(ID_MAX);
    if (!manager) {
        printf("Failed to create IDs manager\n");
        return 1;
    }

    allocate_ids(manager, ID_MAX/2);
    release_ids(manager, ID_MAX/8, ID_MAX/4);
    allocate_ids(manager, ID_MAX/2);
    release_ids(manager, ID_MAX/2, ID_MAX/2+ID_MAX/8);
    allocate_ids(manager, ID_MAX/2);

    ids_manager_delete(manager);

    return 0;
}
