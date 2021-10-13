#include "bv.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct BitVector {
    uint32_t length; // Length in bits
    uint8_t *vector; // Array of bytes
} BitVector;

BitVector *bv_create(uint32_t length) {
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector));
    if (bv) {
        bv->length = length;
        if (length % 8 == 0) {
            bv->vector = (uint8_t *) calloc(length / 8, sizeof(uint8_t));
        } else {
            bv->vector = (uint8_t *) calloc(((length / 8) + 1), sizeof(uint8_t));
        }
        if (!(bv->vector)) {
            free(bv);
            bv = NULL;
        }
    }

    return bv;
}

void bv_delete(BitVector **v) {
    if (*v && (*v)->vector) {
        free((*v)->vector);
        free(*v);
        *v = NULL;
    }
    return;
}

uint32_t bv_length(BitVector *v) {
    return v->length;
}

void bv_set_bit(BitVector *v, uint32_t i) {
    v->vector[i / 8] |= (1 << (i % 8));
    return;
}

void bv_clr_bit(BitVector *v, uint32_t i) {
    v->vector[i / 8] &= ~(1 << (i % 8));
    return;
}

uint8_t bv_get_bit(BitVector *v, uint32_t i) {
    /*uint8_t result = (1 << (i % 8));
    result &= v->vector[i / 8];
    result = (result >> (i % 8));*/
    return (1 & (v->vector[i / 8] >> (i % 8)));
}

void bv_print(BitVector *v) {
    uint32_t bytes;
    if (v->length % 8 == 0) {
        bytes = v->length / 8;
    } else {
        bytes = v->length / 8 + 1;
    }
    for (uint32_t i = 0; i < bytes; i++) {
        printf("[ ");
        for (uint32_t j = 0; j < 8; j++) {
            printf("%" PRIu8 " ", bv_get_bit(v, 8 * i + j));
        }
        printf("]\n");
    }
    return;
}

/*
int main(void) {
	BitVector *v = bv_create(32);
	bv_set_bit(v, 0);
	bv_set_bit(v, 8);
	bv_set_bit(v, 16);
	bv_set_bit(v, 24);
	bv_set_bit(v, 2);
	bv_set_bit(v, 12);
	bv_set_bit(v, 22);
	bv_print(v);
	printf("\n");
	bv_clr_bit(v, 2);
	bv_xor_bit(v, 12, 1);
	bv_xor_bit(v, 22, 0);
	bv_print(v);
	return 0;
}
*/
