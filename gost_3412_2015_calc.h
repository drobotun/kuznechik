#ifndef GOST_3412_2015_CALC_H
#define GOST_3412_2015_CALC_H

#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "gost_3412_2015_const.h"
#include "test.h"

#define BLOCK_SIZE 16
#define KEY_SIZE 32

typedef uint8_t vect[BLOCK_SIZE]; // 128-bit block

vect iter_C[32]; // iteration constants

vect iter_key[10]; // iteration cipher keys

void
kuznechik_expand_key(const uint8_t* key);

void
kuznechik_encrypt(const uint8_t* blk, uint8_t* out_blk);

void
kuznechik_decrypt(const uint8_t* blk, uint8_t* out_blk);

#endif // GOST_3412_2015_CALC_H
