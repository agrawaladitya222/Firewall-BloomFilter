#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*struct Node {
    char *oldspeak;
    char *newspeak;
    Node *next;
    Node *prev;
}
*/

#define strdupe(s) strcpy(malloc(strlen(s) + 1), s)

Node *node_create(char *oldspeak, char *newspeak) { // UNFINISHED
    Node *n = (Node *) malloc(sizeof(Node));
    if (n) {
        // copy and set oldspeak and newspeak with string copy function
        n->oldspeak = oldspeak != NULL ? strdupe(oldspeak) : NULL; // cpy of oldspeak
        n->newspeak = newspeak != NULL ? strdupe(newspeak) : NULL; // cpy of newspeak
        n->next = NULL;
        n->prev = NULL;
        /*if (!(n->oldspeak)) { // only check oldspeak bc newspeak can be NULL
            free(n->oldspeak);
            free(n->newspeak);
            free(n);
        }*/
    }
    return n;
}

void node_delete(Node **n) {
    if (*n) {
        if ((*n)->oldspeak) {
            free((*n)->oldspeak);
        }
        if ((*n)->newspeak) {
            free((*n)->newspeak);
        }
        free(*n);
        *n = NULL;
    }
    return;
}

void node_print(Node *n) {
    if (n) {
        if (n->oldspeak != NULL && n->newspeak != NULL) {
            printf("%s -> %s\n", n->oldspeak, n->newspeak);
        } else {
            printf("%s\n", n->oldspeak);
        }
    }
    return;
}
