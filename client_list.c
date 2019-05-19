#include "client_list.h"
#include "common/macros.h"
#include "common/string_utils.h"
#include "ipv4.h"

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
    list_node *node = create_list_node(tuple);

    if (list->multithreaded) pthread_mutex_lock(&list->mutex);

    if (list->size != 0U) {
        __client_list_insert(list->head->previous, list->head, node);
    } else {
        list->head = node;
    }
    ++list->size;

    if (list->multithreaded) pthread_mutex_unlock(&list->mutex);
}

void client_list_lpush(client_list *list, client_tuple *tuple) {
    list_node *node = create_list_node(tuple);

    if (list->multithreaded) pthread_mutex_lock(&list->mutex);

    if (list->size != 0U) {
        __client_list_insert(list->head->previous, list->head, node);
    } else {
        list->head = node;
    }

    ++list->size;

    if (list->multithreaded) pthread_mutex_unlock(&list->mutex);
}

bool client_tuple_equals(client_tuple *tuple1, client_tuple *tuple2) {
    bool result = str_n_equals(tuple1->ip, tuple2->ip, MAX_IPV4_LENGTH) &&
                  tuple1->port_number == tuple2->port_number;
    return result;
}

bool client_list_exists(client_list *list, client_tuple *tuple) {
    if (list->multithreaded) pthread_mutex_lock(&list->mutex);

    if (list->head == NULL) goto __EXIT__;

    list_node *curr = list->head;

    do {
        if (client_tuple_equals(&curr->tuple, tuple)) return true;
        curr = curr->next;
    } while (curr != list->head);

    __EXIT__:
    if (list->multithreaded) pthread_mutex_unlock(&list->mutex);

    return false;
}