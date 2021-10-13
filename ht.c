#include "ht.h"

#include "ll.h"
#include "speck.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct HashTable {
    uint64_t salt[2];
    uint32_t size;
    bool mtf;
    LinkedList **lists;
};

HashTable *ht_create(uint32_t size, bool mtf) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable));
    if (ht) {
        // Leviathan
        ht->salt[0] = 0x9846e4f157fe8840;
        ht->salt[1] = 0xc5f318d7e055afb8;
        ht->size = size;
        ht->mtf = mtf;
        ht->lists = (LinkedList **) calloc(size, sizeof(LinkedList *));
        if (!ht->lists) {
            free(ht);
            ht = NULL;
        }
    }
    return ht;
}

void ht_delete(HashTable **ht) {
    if (*ht && (*ht)->lists) {
        for (uint32_t i = 0; i < ht_size(ht); i += 1) {
            if ((*ht)->lists[i]) {
                ll_delete((*ht)->lists[i]);
            }
        }
        free(*ht);
        *ht = NULL;
    }
}

uint32_t ht_size(HashTable *ht) {
    return ht->size;
}

Node *ht_lookup(HashTable *ht, char *oldspeak) {
    uint32_t index = hash(ht->salt, oldspeak) % ht_size(ht);
    if (!ht->lists[index]) {
        return NULL;
    }
    Node *n = ll_lookup(ht->lists[index], oldspeak);
    return n;
}

void ht_insert(HashTable *ht, char *oldspeak, char *newspeak) {
    uint32_t index = hash(ht->salt, oldspeak) % ht_size(ht);
    if (!ht->lists[index]) {
        ht->lists[index] = ll_create(ht->mtf);
    }
    ll_insert(ht->lists[index], oldspeak, newspeak);
    return;
}

uint32_t ht_count(HashTable *ht) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < ht_size(ht); i += 1) {
        if (ht->lists[i]) {
            count += 1;
        }
    }
    return count;
}

void ht_print(HashTable *ht) {
    for (uint32_t i = 0; i < ht_size(ht); i += 1) {
        if (ht->lists[i]) {
            ll_print(ht->lists[i]);
        }
    }
    return;
}
