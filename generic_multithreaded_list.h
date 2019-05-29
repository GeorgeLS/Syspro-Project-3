#ifndef EXERCISE_III_GENERIC_MULTITHREADED_LIST_H
#define EXERCISE_III_GENERIC_MULTITHREADED_LIST_H

#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>
#include "common/attributes.h"
#include "common/common_types.h"

#define LIST_MULTITHREADED (true)

typedef struct list_node {
    struct list_node *next;
    struct list_node *previous;
    void *data;
} list_node;

typedef struct list {
    pthread_mutex_t mutex;
    equality_comparer comparer;
    list_node *head;
    size_t size;
    bool multithreaded;
} list;

list list_create(equality_comparer comparer, bool multithreaded);

__NON_NULL__(1)
void list_destroy(list *list);

__NON_NULL__(1, 2)
void list_rpush(list *list, void *data);

__NON_NULL__(1, 2)
void list_lpush(list *list, void *data);

__NON_NULL__(1, 2)
void list_remove(list *list, void *data);

__NON_NULL__(1)
void *list_first_entry(list *list);

__NON_NULL__(1)
void *list_last_entry(list *list);

__NON_NULL__(1, 2)
bool list_element_exists(list *list, void *element);

__NON_NULL__(1, 2, 3)
bool list_element_exists_custom(list *list, void *element, equality_comparer comparer);

#endif //EXERCISE_III_GENERIC_MULTITHREADED_LIST_H
