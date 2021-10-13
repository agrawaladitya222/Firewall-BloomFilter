#include "bf.h"
#include "bv.h"
#include "ht.h"
#include "ll.h"
#include "messages.h"
#include "node.h"
#include "parser.h"
#include "speck.h"

#include <ctype.h>
#include <inttypes.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define OPTIONS "ht:f:ms"
#define WORD    "[a-zA-Z0-9_]+([- | '][a-zA-Z0-9_]+)*"

int main(int argc, char **argv) {
    int opt = 0;
    uint32_t hash_size = 10000;
    uint32_t bloom_size = 1048576; // 2^20
    bool mtf = false;
    bool stats = false;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h':
            // print help message
            printf("SYNOPSIS: A word filtering program for the GPRSC. "
                   "Filters out and reports bad words parsed from stdin.\n"
                   "USAGE: ./banhammer [-hsm] [-t size] [-f size]\n"
                   "OPTIONS:\n\t-h           Program usage and help."
                   "\n\t-s           Print program statistics."
                   "\n\t-m           Enable move-to-front rule."
                   "\n\t-t size      Specify hash table size (default: 10000)."
                   "\n\t-f size      Specify Bloom filter size (default: 2^20).\n");
            return 0;
        case 't': hash_size = strtoul(optarg, NULL, 10); break;
        case 'f': bloom_size = strtoul(optarg, NULL, 10); break;
        case 'm': mtf = true; break;
        case 's': stats = true; break;
        default: printf("Invalid command line option.\n"); return 1;
        }
    }

    // initialize bf and ht
    BloomFilter *bf = bf_create(bloom_size);
    HashTable *ht = ht_create(hash_size, mtf);

    // read in badspeak
    FILE *badspeak = fopen("./badspeak.txt", "r");
    int c;
    char badwords[100];
    while ((c = fscanf(badspeak, "%s", badwords)) != EOF) {
        if (c != 1) {
            printf("Error reading badspeak.txt\n");
            return 0;
        }
        bf_insert(bf, badwords);
        ht_insert(ht, badwords, NULL);
    }

    // read in old/new speak pairs
    FILE *newspeak = fopen("newspeak.txt", "r");
    char oldwords[100];
    char newwords[100];
    while ((c = fscanf(newspeak, "%s %s", oldwords, newwords)) != EOF) {
        if (c != 2) {
            printf("Error reading newspeak.txt\n");
            return 0;
        }
        bf_insert(bf, oldwords);
        ht_insert(ht, oldwords, newwords);
    }

    // read words in from stdin using parsing module
    regex_t re;
    if (regcomp(&re, WORD, REG_EXTENDED)) {
        fprintf(stderr, "Failed to compile regex.\n");
        return 1;
    }
    char *word = NULL;

    LinkedList *badpeople = ll_create(false);
    LinkedList *oldpeople = ll_create(false);
    while ((word = next_word(stdin, &re)) != NULL) {
        for (uint32_t i = 0; i < strlen(word); i++) {
            word[i] = tolower(word[i]);
        }
        if (bf_probe(bf, word)) { // check if word is in bloom filter
            Node *n = ht_lookup(ht, word);
            if (n != NULL) { // check if word is in hash table
                if (n->newspeak != NULL) {
                    // word has newspeak translation
                    ll_insert(oldpeople, word, n->newspeak);
                } else {
                    // word has no newspeak translation
                    ll_insert(badpeople, word, NULL);
                }
            }
        }
    }
    clear_words();
    regfree(&re);

    // Judge and punish crimes
    if (!stats) {
        if (ll_length(badpeople) != 0 && ll_length(oldpeople) != 0) {
            printf("%s", mixspeak_message);
            ll_print(badpeople);
            ll_print(oldpeople);
        } else if (ll_length(badpeople) != 0) {
            printf("%s", badspeak_message);
            ll_print(badpeople);
        } else if (ll_length(oldpeople) != 0) {
            printf("%s", goodspeak_message);
            ll_print(oldpeople);
        }
    } else {
        printf("Seeks: %" PRIu64 "\n", seeks);
        double average = (double) links / (double) seeks;
        printf("Average seek length: %.6f\n", average);
        double htsize = ht_size(ht);
        double htload = ((double) ht_count(ht) / htsize) * 100.0;
        printf("Hash table load: %.6f%%\n", htload);
        double bfsize = bf_size(bf);
        double bfload = ((double) bf_count(bf) / bfsize) * 100.0;
        printf("Bloom filter load: %.6f%%\n", bfload);
    }
    // clean up
    ht_delete(&ht);
    bf_delete(&bf);
    ll_delete(&oldpeople);
    ll_delete(&badpeople);
    fclose(badspeak);
    fclose(newspeak);
    return 0;
}
