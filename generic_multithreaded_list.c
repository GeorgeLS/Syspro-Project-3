#include <malloc.h>
#include "generic_multithreaded_list.h"
#include "common/macros.h"

static void
__list_insert(list_node *left_side, list_node *right_side, list_node *node) {
    left_side->next = node;
    right_side->previous = node;
    node->previous = left_side;
    node->next = right_side;
}

static void
__list_remove(list_node *left_side, list_node *right_side, list_node *node) {
    left_side->next = node->next;
    right_side->previous = node->previous;
    free(node);
}

__INLINE__ static list_node *
__create_list_node(void *data) {
    list_node *node = __MALLOC__(1, list_node);
    node->next = node->previous = node;
    node->data = data;
    return node;
}

list list_create(equality_comparer comparer, bool multithreaded) {
    return (list) {
            .mutex = PTHREAD_MUTEX_INITIALIZER,
            .head = NULL,
            .comparer = comparer,
            .size = 0U,
            .multithreaded = multithreaded
    };
}

void list_destroy(list *list) {
    list_node *curr = list->head;
    if (curr != NULL) {
        do {
            list_node *temp = curr;
            curr = curr->next;
            free(temp->data);
            free(temp);
        } while (curr != list->head);
    }
}

void list_rpush(list *list, void *data) {
    list_node *node = __create_list_node(data);

    if (list->multithreaded) pthread_mutex_lock(&list->mutex);

    if (list->size != 0U) {
        __list_insert(list->head->previous, list->head, node);
    } else {
        list->head = node;
    }
    ++list->size;

    if (list->multithreaded) pthread_mutex_unlock(&list->mutex);
}

void list_lpush(list *list, void *data) {
    list_node *node = __create_list_node(data);

    if (list->multithreaded) pthread_mutex_lock(&list->mutex);

    if (list->size != 0U) {
        __list_insert(list->head->previous, list->head, node);
        list->head = node;
    } else {
        list->head = node;
    }

    ++list->size;

    if (list->multithreaded) pthread_mutex_unlock(&list->mutex);
}

void list_remove(list *list, void *data) {
    if (list->multithreaded) {
        pthread_mutex_lock(&list->mutex);
    }

    list_node *curr = list->head;
    do {
        if (list->comparer(curr->data, data)) {
            __list_remove(curr->previous, curr->next, curr);
            break;
        }
        curr = curr->next;
    } while (curr != list->head);

    --list->size;

    if (list->size == 0) {
        list->head = NULL;
    } else if (list->head == curr) {
        list->head = list->head->next;
    }

    if (list->multithreaded) {
        pthread_mutex_unlock(&list->mutex);
    }
}

void *list_first_entry(list *list) {
    if (list->head == NULL) {
        return NULL;
    }
    void *result = list->head->data;
    return result;
}

void *list_last_entry(list *list) {
    if (list->head == NULL || list->head->previous == NULL) {
        return NULL;
    }
    void *result = list->head->previous->data;
    return result;
}

bool list_element_exists(list *list, void *element) {
    bool exists = false;
    if (list->multithreaded) pthread_mutex_lock(&list->mutex);

    if (list->head == NULL) {
        goto __EXIT__;
    }

    list_node *curr = list->head;
    do {
        if (list->comparer(curr->data, element)) {
            exists = true;
            goto __EXIT__;
        }
        curr = curr->next;
    } while (curr != list->head);

    __EXIT__:
    if (list->multithreaded) pthread_mutex_unlock(&list->mutex);

    return exists;
}

bool list_element_exists_custom(list *list, void *element, equality_comparer comparer) {
    equality_comparer list_comparer = list->comparer;
    list->comparer = comparer;
    bool result = list_element_exists(list, element);
    list->comparer = list_comparer;
    return result;
}

void *list_find_element(list *list, void *key) {
    void *result = NULL;
    if (list->multithreaded) {
        pthread_mutex_lock(&list->mutex);
    }

    list_node *curr = list->head;
    if (curr == NULL) return NULL;
    do {
        if (list->comparer(curr->data, key)) {
            result = curr->data;
            break;
        }
        curr = curr->next;
    } while (curr != list->head);

    if (list->multithreaded) {
        pthread_mutex_unlock(&list->mutex);
    }
    return result;
}