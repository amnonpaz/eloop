#include <list.h>
#include <stdio.h>
#include <stdlib.h>

struct int_node {
    struct list_head list;
    int data;
};

#define arr_len(arr) \
    (sizeof(arr)/sizeof((arr)[0]))


static int fill_list(struct list_head *list, int data[], int len)
{
    int n;

    for (n = 0; n < len; n++) {
        struct int_node *item = malloc(sizeof(*item));
        if (!item) {
            printf("Error allocating new node");
            break;
        }

        item->data = data[n];
        list_add(list, (struct list_head *)item);
    }

    return n;
}

static void print_list(struct list_head *list)
{
    struct list_head *itr;

    printf("* ");
    list_for_each(list, itr) {
        printf("%d -> ", ((struct int_node *)itr)->data);
    }
    printf("#\n");

    printf("# ");
    list_for_each_rev(list, itr) {
        printf("%d <- ", ((struct int_node *)itr)->data);
    }
    printf("*\n");
}

static void clean_list(struct list_head *list)
{
    struct list_head *itr, *item;

    printf("Removed: ");
    list_for_each_safe(list, itr, item) {
        struct int_node *node = (struct int_node *)item;
        int n = node->data;

        list_del(item);
        free(node);

        printf("%d ", n);
    }

    printf("#\n");
}

int main(int argc, char *argv[])
{
    int data[] = {4, 7, 18, 10, 12, 6, 11};
    list_new(numbers);

    fill_list(&numbers, data, arr_len(data));
    print_list(&numbers);
    clean_list(&numbers);

    return 0;
}
