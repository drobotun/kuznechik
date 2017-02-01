#ifndef GOST_3412_2015_CALC_H
#define GOST_3412_2015_CALC_H

#include <stdfix.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "gost_3412_2015_const.h"
#include "test.h"

#define BLOCK_SIZE 16

typedef uint8_t vect[BLOCK_SIZE]; //блок размером 128 бит

vect iter_C[32]; //итерационные константы C

vect iter_key[10]; //итерационные ключи шифрования

void
GOST_Kuz_Expand_Key(const uint8_t *key_1, const uint8_t *key_2);

void
GOST_Kuz_Encript(const uint8_t *blk, uint8_t *out_blk);

void
GOST_Kuz_Decript(const uint8_t *blk, uint8_t *out_blk);

#endif // GOST_3412_2015_CALC_H
