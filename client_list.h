#ifndef EXERCISE_III_CLIENT_LIST_H
#define EXERCISE_III_CLIENT_LIST_H

#include <stddef.h>
#include <pthread.h>
#include <stdbool.h>
#include "common/common_types.h"
#include "common/attributes.h"

#define CLIENT_LIST_MULTITHREADED true

typedef struct client_tuple {
    char *ip;
    u16 port_number;
} client_tuple;

typedef struct list_node {
    client_tuple tuple;
    struct list_node *next;
    struct list_node *previous;
} list_node;

typedef struct client_list {
    pthread_mutex_t mutex;
    list_node *head;
    size_t size;
    bool multithreaded;
} client_list;

client_list client_list_create(bool multithreaded);

__NON_NULL__(1, 2)
void client_list_rpush(client_list *list, client_tuple *tuple);

__NON_NULL__(1, 2)
void client_list_lpush(client_list *list, client_tuple *tuple);


#endif //EXERCISE_III_CLIENT_LIST_H
