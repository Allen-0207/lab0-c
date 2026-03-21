#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *h =
        (struct list_head *) calloc(1, sizeof(struct list_head));
    if (!h) {
        return NULL;
    }

    INIT_LIST_HEAD(h);
    return h;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }

    struct list_head *node, *safe;
    list_for_each_safe(node, safe, head) {
        element_t *elem = list_entry(node, element_t, list);
        q_release_element(elem);
    }

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }

    element_t *new_e = (element_t *) calloc(1, sizeof(element_t));
    if (!new_e) {
        return false;
    }

    INIT_LIST_HEAD(&new_e->list);

    new_e->value = strdup(s);
    if (!new_e->value) {
        free(new_e);
        return false;
    }
    list_add(&new_e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }

    element_t *new_e = (element_t *) calloc(1, sizeof(element_t));
    if (!new_e) {
        return false;
    }

    INIT_LIST_HEAD(&new_e->list);

    new_e->value = strdup(s);
    if (!new_e->value) {
        free(new_e);
        return false;
    }
    list_add_tail(&new_e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *remove_e = list_first_entry(head, element_t, list);
    list_del(&remove_e->list);
    if (sp && remove_e->value) {
        snprintf(sp, bufsize, "%s", remove_e->value);
    }
    return remove_e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *remove_e = list_last_entry(head, element_t, list);
    list_del(&remove_e->list);
    if (sp && remove_e->value) {
        snprintf(sp, bufsize, "%s", remove_e->value);
    }
    return remove_e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }

    int size = 0;
    struct list_head *node;
    list_for_each(node, head) {
        size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *slow;
    struct list_head *fast;

    for (slow = head->next, fast = slow; fast != head && fast->next != head;
         slow = slow->next, fast = fast->next->next) {
    }

    element_t *mid = list_entry(slow, element_t, list);
    list_del(slow);
    if (mid->value) {
        free(mid->value);
    }
    free(mid);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *curr = head->next;
    struct list_head *next, *node;
    while (curr != head) {
        bool dup = false;
        node = curr->next;
        while (node != head) {
            struct list_head *temp = node->next;
            if (strcmp(list_entry(curr, element_t, list)->value,
                       list_entry(node, element_t, list)->value) == 0) {
                dup = true;
                list_del(node);
                q_release_element(list_entry(node, element_t, list));
            }
            node = temp;
        }

        next = curr->next;
        if (dup) {
            list_del(curr);
            q_release_element(list_entry(curr, element_t, list));
        }
        curr = next;
    }

    return true;
}

static struct list_head *swap_two_node(struct list_head *node)
{
    struct list_head *tmp = node->next;
    tmp->next->prev = node;
    node->prev->next = tmp;
    node->next = tmp->next;
    tmp->prev = node->prev;
    node->prev = tmp;
    tmp->next = node;
    return tmp;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }

    struct list_head *n1 = head;
    while (n1->next != head && n1->next->next != head) {
        n1->next = swap_two_node(n1->next);
        n1 = n1->next->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }

    struct list_head *node, *safe, *temp;
    list_for_each_safe(node, safe, head) {
        temp = node->next;
        node->next = node->prev;
        node->prev = temp;
    }

    temp = head->next;
    head->next = head->prev;
    head->prev = temp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head)) {
        return;
    }

    struct list_head *node = head->next, *safe, *tmp = head;
    int size = q_size(head);
    int group_size = size / k;
    for (int i = 0; i < group_size; i++) {
        for (int j = 0; j < k; j++) {
            safe = node->next;
            list_move(node, tmp);
            node = safe;
        }

        tmp = node->prev;
    }
}

static struct list_head *merge_list(struct list_head *l1,
                                    struct list_head *l2,
                                    bool descend)
{
    struct list_head head;
    struct list_head *tmp = &head;

    while (l1 && l2) {
        const char *val1 = list_entry(l1, element_t, list)->value;
        const char *val2 = list_entry(l2, element_t, list)->value;

        if (descend ? strcmp(val1, val2) >= 0 : strcmp(val1, val2) <= 0) {
            tmp->next = l1;
            l1 = l1->next;
        } else {
            tmp->next = l2;
            l2 = l2->next;
        }

        tmp = tmp->next;
    }

    tmp->next = l1 ? l1 : l2;
    return head.next;
}

static struct list_head *merge(struct list_head *head, bool descend)
{
    if (!head || !head->next) {
        return head;
    }

    struct list_head *slow = head;
    struct list_head *fast = slow->next;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    struct list_head *mid = slow->next;
    slow->next = NULL;

    struct list_head *left = merge(head, descend);
    struct list_head *right = merge(mid, descend);
    return merge_list(left, right, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head)) {
        return;
    }

    head->prev->next = NULL;
    head->next = merge(head->next, descend);

    struct list_head *cur;
    struct list_head *prev_node = head;
    for (cur = head->next; cur->next != NULL; cur = cur->next) {
        cur->prev = prev_node;
        prev_node = cur;
    }

    cur->next = head;
    cur->prev = prev_node;
    head->prev = cur;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    struct list_head *node, *safe;
    const char *min_value = NULL;

    struct list_head *remove_head = q_new();

    for (node = head->prev, safe = node->prev; node != head;
         node = safe, safe = node->prev) {
        char *str = list_entry(node, element_t, list)->value;
        if (!min_value) {
            min_value = str;
            continue;
        }

        if (strcmp(min_value, str) < 0) {
            list_move(node, remove_head);
        } else {
            min_value = str;
        }
    }

    q_free(remove_head);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    struct list_head *node, *safe;
    const char *max_value = NULL;

    struct list_head *remove_head = q_new();

    for (node = head->prev, safe = node->prev; node != head;
         node = safe, safe = node->prev) {
        char *str = list_entry(node, element_t, list)->value;
        if (!max_value) {
            max_value = str;
            continue;
        }

        if (strcmp(max_value, str) > 0) {
            list_move(node, remove_head);
        } else {
            max_value = str;
        }
    }

    q_free(remove_head);
    return q_size(head);
}


static void merge_two_list(struct list_head *l1,
                           struct list_head *l2,
                           int descend)
{
    struct list_head merged;
    INIT_LIST_HEAD(&merged);

    while (!list_empty(l1) && !list_empty(l2)) {
        const char *val1 = list_entry(l1->next, element_t, list)->value;
        const char *val2 = list_entry(l2->next, element_t, list)->value;
        if (descend ? strcmp(val1, val2) >= 0 : strcmp(val1, val2) <= 0) {
            list_move_tail(l1->next, &merged);
        } else {
            list_move_tail(l2->next, &merged);
        }
    }

    if (!list_empty(l1)) {
        list_splice_tail_init(l1, &merged);
    }

    if (!list_empty(l2)) {
        list_splice_tail_init(l2, &merged);
    }

    list_splice_init(&merged, l1);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head)) {
        return 0;
    }

    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);

    for (struct list_head *cur = head->next->next; cur != head;
         cur = cur->next) {
        queue_contex_t *tmp = list_entry(cur, queue_contex_t, chain);
        first->size += tmp->size;
        tmp->size = 0;
        merge_two_list(first->q, tmp->q, descend);
    }

    return first->size;
}
