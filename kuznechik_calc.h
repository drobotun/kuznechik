#ifndef KUZNECHIK_CALC_H
#define KUZNECHIK_CALC_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kuznechik_const.h"
#include "kuznechik_test.h"

#define BLOCK_SIZE 16
#define KEY_SIZE 32

typedef uint8_t vect[BLOCK_SIZE]; // 128-bit block

vect iter_const[32]; // iteration constants
vect iter_key[10]; // iteration cipher keys

void
kuznechik_expand_key(const uint8_t* key);

void
kuznechik_encrypt(const uint8_t* blk, uint8_t* out_blk);

void
kuznechik_decrypt(const uint8_t* blk, uint8_t* out_blk);

#endif // KUZNECHIK_CALC_H
