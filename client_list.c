#include "client_list.h"
#include "common/macros.h"

static list_node *create_list_node(client_tuple *tuple) {
    list_node *node = __MALLOC__(1, list_node);
    node->tuple = *tuple;
    node->previous = node;
    node->next = node;
    return node;
}

static void __client_list_insert(list_node *left_side, list_node *right_side, list_node *node) {
    left_side->next = node;
    right_side->previous = node;
    node->previous = left_side;
    node->next = right_side;
}

static void __client_list_remove(list_node *left_side, list_node *righ_side, list_node *node) {
    left_side->next = node->next;
    righ_side->previous = node->previous;
    free(node);
}

client_list client_list_create(bool multithreaded) {
    return (client_list) {
            .size = 0U,
            .head = NULL,
            .mutex = PTHREAD_MUTEX_INITIALIZER,
            .multithreaded = multithreaded
    };
}

void client_list_rpush(client_list *list, client_tuple *tuple) {
    if (list->multithreaded) pthread_mutex_lock(&list->mutex);

    list_node *node = create_list_node(tuple);

    if (list->size != 0U) {
        __client_list_insert(list->head->previous, list->head, node);
    } else {
        list->head = node;
    }
    ++list->size;

    if (list->multithreaded) pthread_mutex_unlock(&list->mutex);
}

void client_list_lpush(client_list *list, client_tuple *tuple) {
    if (list->multithreaded) pthread_mutex_lock(&list->mutex);

    list_node *node = create_list_node(tuple);

    if (list->size != 0U) {
        __client_list_insert(list->head->previous, list->head, node);
    } else {
        list->head = node;
    }

    ++list->size;

    if (list->multithreaded) pthread_mutex_unlock(&list->mutex);
}