#include "gost_3412_2015_calc.h"

#define DEBUG_MODE

#ifdef DEBUG_MODE
static void
GOST_Kuz_PrintDebug(uint8_t *state)
{
    int i;
    for (i = 0; i < BLOCK_SIZE; i++)
        printf("%02x", state[i]);
    printf("\n");
}
#endif

static void
GOST_Kuz_S(const uint8_t *in_data, uint8_t *out_data)
{
    int i;
    for (i = 0; i < BLOCK_SIZE; i++)
        out_data[i] = Pi[in_data[i]];
}

static void
GOST_Kuz_reverse_S(const uint8_t *in_data, uint8_t *out_data)
{
    int i;
    for (i = 0; i < BLOCK_SIZE; i++)
        out_data[i] = reverse_Pi[in_data[i]];
}

static void
GOST_Kuz_X(const uint8_t *a, const uint8_t *b, uint8_t *c)
{
    int i;
    for (i = 0; i < BLOCK_SIZE; i++)
        c[i] = a[i] ^ b[i];
}

static uint8_t
GOST_Kuz_GF_mul(uint8_t a, uint8_t b)
{
    uint8_t c = 0;
    uint8_t hi_bit;
    int i;
    for (i = 0; i < 8; i++)
    {
        if (b & 1)
            c ^= a;
        hi_bit = a & 0x80;
        a <<= 1;
        if (hi_bit)
            a ^= 0xc3; //полином x^8+x^7+x^6+x+1
        b >>= 1;
    }
    return c;
}

static void
GOST_Kuz_R(uint8_t *state)
{
    int i;
    uint8_t a_15 = 0;
    vect internal;
    for (i = 15; i >= 0; i--)
    {
        internal[i - 1] = state[i];
        a_15 ^= GOST_Kuz_GF_mul(state[i], l_vec[i]);
    }
    internal[15] = a_15;
    memcpy(state, internal, BLOCK_SIZE);
}

static void
GOST_Kuz_reverse_R(uint8_t *state)
{
    int i;
    uint8_t a_0;
    a_0 = state[15];
    vect internal;
    for (i = 0; i < 16; i++)
    {
        internal[i] = state[i - 1];
        a_0 ^= GOST_Kuz_GF_mul(internal[i], l_vec[i]);
    }
    internal[0] = a_0;
    memcpy(state, internal, BLOCK_SIZE);
}

static void
GOST_Kuz_L(const uint8_t *in_data, uint8_t *out_data)
{
    int i;
    vect internal;
    memcpy(internal, in_data, BLOCK_SIZE);
    for (i = 0; i < 16; i++)
        GOST_Kuz_R(internal);
    memcpy(out_data, internal, BLOCK_SIZE);
}

static void
GOST_Kuz_reverse_L(const uint8_t *in_data, uint8_t *out_data)
{
    int i;
    vect internal;
    memcpy(internal, in_data, BLOCK_SIZE);
    for (i = 0; i < 16; i++)
        GOST_Kuz_reverse_R(internal);
    memcpy(out_data, internal, BLOCK_SIZE);
}

static void
GOST_Kuz_Get_C()
{
    int i;
    vect iter_num[32];
    for (i = 0; i < 32; i++)
    {
        memset(iter_num[i], 0, BLOCK_SIZE);
        iter_num[i][0] = i+1;
    }
    for (i = 0; i < 32; i++)
         GOST_Kuz_L(iter_num[i], iter_C[i]);
}

static void
GOST_Kuz_F(const uint8_t *in_key_1, const uint8_t *in_key_2,
           uint8_t *out_key_1, uint8_t *out_key_2,
           uint8_t *iter_const)
{
    vect internal;
    memcpy(out_key_2, in_key_1, BLOCK_SIZE);
    GOST_Kuz_X(in_key_1, iter_const, internal);
    GOST_Kuz_S(internal, internal);
    GOST_Kuz_L(internal, internal);
    GOST_Kuz_X(internal, in_key_2, out_key_1);
}

void
GOST_Kuz_Expand_Key(const uint8_t *key_1, const uint8_t *key_2)
{
    int i;
    uint8_t iter_1[64];
    uint8_t iter_2[64];
    uint8_t iter_3[64];
    uint8_t iter_4[64];
    GOST_Kuz_Get_C();
    memcpy(iter_key[0], key_1, 64);
    memcpy(iter_key[1], key_2, 64);
    memcpy(iter_1, key_1, 64);
    memcpy(iter_2, key_2, 64);
    for (i = 0; i < 4; i++)
    {
        GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[0 + 8 * i]);
        GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[1 + 8 * i]);
        GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[2 + 8 * i]);
        GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[3 + 8 * i]);
        GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[4 + 8 * i]);
        GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[5 + 8 * i]);
        GOST_Kuz_F(iter_1, iter_2, iter_3, iter_4, iter_C[6 + 8 * i]);
        GOST_Kuz_F(iter_3, iter_4, iter_1, iter_2, iter_C[7 + 8 * i]);
        memcpy(iter_key[2 * i + 2], iter_1, 64);
        memcpy(iter_key[2 * i + 3], iter_2, 64);
    }

#ifdef DEBUG_MODE
    printf("Iteration cipher keys:\n");
    for (i = 0; i < 10; i++)
        GOST_Kuz_PrintDebug(iter_key[i]);
#endif
}

void
GOST_Kuz_Encrypt(const uint8_t *blk, uint8_t *out_blk)
{
    int i;
    memcpy(out_blk, blk, BLOCK_SIZE);

#ifdef DEBUG_MODE
    printf("Text:\n");
    GOST_Kuz_PrintDebug(out_blk);
#endif

    for(i = 0; i < 9; i++)
    {
        GOST_Kuz_X(iter_key[i], out_blk, out_blk);

        GOST_Kuz_S(out_blk, out_blk);

        GOST_Kuz_L(out_blk, out_blk);
    }
    GOST_Kuz_X(out_blk, iter_key[9], out_blk);

#ifdef DEBUG_MODE
    printf("Encrypted text:\n");
    GOST_Kuz_PrintDebug(out_blk);
#endif
}

void
GOST_Kuz_Decrypt(const uint8_t *blk, uint8_t *out_blk)
{
    int i;
    memcpy(out_blk, blk, BLOCK_SIZE);

#ifdef DEBUG_MODE
    printf("Gipher text:\n");
    GOST_Kuz_PrintDebug(out_blk);
#endif

    GOST_Kuz_X(out_blk, iter_key[9], out_blk);
    for(i = 8; i >= 0; i--)
    {
        GOST_Kuz_reverse_L(out_blk, out_blk);

        GOST_Kuz_reverse_S(out_blk, out_blk);

        GOST_Kuz_X(iter_key[i], out_blk, out_blk);
    }

#ifdef DEBUG_MODE
    printf("Decrypted text:\n");
    GOST_Kuz_PrintDebug(out_blk);
#endif
}
