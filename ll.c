#include "ll.h"

#include "node.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct LinkedList {
    uint32_t length;
    Node *head; // Head sentinel node.
    Node *tail; // Tail sentinel node.
    bool mtf;
};

uint64_t seeks = 0;
uint64_t links = 0;

LinkedList *ll_create(bool mtf) {
    LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));
    if (ll) {
        ll->length = 0;
        ll->head = node_create("head", NULL);
        ll->tail = node_create("tail", NULL);
        // set head and tail to point to each other
        ll->head->next = ll->tail;
        ll->tail->prev = ll->head;
        ll->mtf = mtf;
        if (!(ll->head) || !(ll->tail)) {
            node_delete(&(ll->head));
            node_delete(&(ll->tail));
            free(ll);
        }
    }
    return ll;
}

void ll_delete(LinkedList **ll) {
    if (*ll) {
        Node *current = (*ll)->head;
        while (current != NULL) {
            Node *next = NULL;
            next = current->next;
            node_delete(current);
            current = next;
        }
        free(*ll);
        *ll = NULL;
    }
}

uint32_t ll_length(LinkedList *ll) {
    return ll->length;
}

Node *ll_lookup(LinkedList *ll, char *oldspeak) {
    seeks += 1;
    Node *current = ll->head;
    while (current->next != NULL) {
        links += 1;
        if (strcmp(oldspeak, current->oldspeak) == 0) {
            if (ll->mtf) { // move to front
                current->prev->next = current->next;
                current->next->prev = current->prev;

                current->prev = ll->head;
                current->next = ll->head->next;

                ll->head->next->prev = current;
                ll->head->next = current;
            }
            return current;
        }
        current = current->next;
    }
    current = NULL;
    return current;
}

void ll_insert(LinkedList *ll, char *oldspeak, char *newspeak) {
    if (!ll_lookup(ll, oldspeak)) {
        Node *g = node_create(oldspeak, newspeak);

        g->prev = ll->head;
        g->next = ll->head->next;
        ll->head->next->prev = g;
        ll->head->next = g;
        ll->length += 1;
    }
    return;
}

void ll_print(LinkedList *ll) {
    if (ll) {
        Node *current = ll->head->next;
        while (current->next != NULL) {
            node_print(current);
            current = current->next;
        }
    }
    return;
}
