#include "bf.h"

#include "bv.h"
#include "speck.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct BloomFilter {
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter;
};

BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));
    if (bf) {
        // Grimm's Fairy Tales
        bf->primary[0] = 0x5adf08ae86d36f21;
        bf->primary[1] = 0x267bbd3116f3957;
        // The Adventures of Sherlock Holmes
        bf->secondary[0] = 0x419d292ea2ffd49e;
        bf->secondary[1] = 0x09601433057d5786;
        // The Strange Case of Dr. Jekyll and Mr. Hyde
        bf->tertiary[0] = 0x50d8bb08de3818df;
        bf->tertiary[1] = 0x4deaae187c16ae1d;
        bf->filter = bv_create(size);
        if (!bf->filter) {
            free(bf);
            bf = NULL;
        }
    }
    return bf;
}

void bf_delete(BloomFilter **bf) {
    if (*bf && (*bf)->filter) {
        bv_delete(&((*bf)->filter));
        free(*bf);
        *bf = NULL;
    }
    return;
}

uint32_t bf_size(BloomFilter *bf) {
    return bv_length(bf->filter);
}

void bf_insert(BloomFilter *bf, char *oldspeak) {
    // primary salt
    uint32_t index = hash(bf->primary, oldspeak) % bf_size(bf);
    bv_set_bit(bf->filter, index);
    // secondary salt
    index = hash(bf->secondary, oldspeak) % bf_size(bf);
    bv_set_bit(bf->filter, index);
    // tertiary salt
    index = hash(bf->tertiary, oldspeak) % bf_size(bf);
    bv_set_bit(bf->filter, index);
    return;
}

bool bf_probe(BloomFilter *bf, char *oldspeak) {
    uint8_t primary = 0, secondary = 0, tertiary = 0;
    // primary salt
    uint32_t index = hash(bf->primary, oldspeak) % bf_size(bf);
    primary = bv_get_bit(bf->filter, index);
    // secondary salt
    index = hash(bf->secondary, oldspeak) % bf_size(bf);
    secondary = bv_get_bit(bf->filter, index);
    // tertiary salt
    index = hash(bf->tertiary, oldspeak) % bf_size(bf);
    tertiary = bv_get_bit(bf->filter, index);

    if (primary && secondary && tertiary) {
        return true;
    } else {
        return false;
    }
}

uint32_t bf_count(BloomFilter *bf) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < bf_size(bf); i += 1) {
        if (bv_get_bit(bf->filter, i)) {
            count += 1;
        }
    }
    return count;
}

void bf_print(BloomFilter *bf) {
    bv_print(bf->filter);
    return;
}
