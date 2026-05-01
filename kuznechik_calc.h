#ifndef KUZNECHIK_CALC_H
#define KUZNECHIK_CALC_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kuznechik_const.h"

#define BLOCK_SIZE 16
#define KEY_SIZE 32

typedef uint8_t vect[BLOCK_SIZE] __attribute__((aligned(16))); // 128-bit block

typedef struct {
    vect iter_const[32]; // iteration constants
    vect iter_key[10]; // iteration cipher keys
} kuznechik_ctx;

void kuznechik_init(kuznechik_ctx* ctx, const uint8_t* key);
void kuznechik_encrypt(kuznechik_ctx* ctx, const uint8_t* plain_block, uint8_t* cipher_block);
void kuznechik_decrypt(kuznechik_ctx* ctx, const uint8_t* cipher_block, uint8_t* plain_block);

#endif // KUZNECHIK_CALC_H
