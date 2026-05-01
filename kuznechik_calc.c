#include "kuznechik_calc.h"

#ifdef DEBUG_MODE
static void kuznechik_print_debug(const uint8_t* state) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        printf("%02x", state[i]);
    }
    printf("\n");
}
#endif

static inline void kuznechik_s_transformation(const uint8_t* in_data, uint8_t* out_data) {
    for (int i = BLOCK_SIZE - 1; i >= 0; i--) {
        out_data[i] = pi[in_data[i]];
    }
}

static inline void kuznechik_reverse_s_transformation(const uint8_t* in_data, uint8_t* out_data) {
    for (int i = BLOCK_SIZE - 1; i >= 0; i--) {
        out_data[i] = reverse_pi[in_data[i]];
    }
}

static inline void kuznechik_xor(const uint8_t* a, const uint8_t* b, uint8_t* c) {
#ifdef __SIZEOF_INT128__
    __uint128_t a_128, b_128;
    memcpy(&a_128, a, sizeof (a_128));
    memcpy(&b_128, b, sizeof (b_128));
    a_128 ^= b_128;
    memcpy(c, &a_128, sizeof(a_128));
#else
    for(int i = BLOCK_SIZE - 1; i >= 0; i--) {
        c[i] = a[i] ^ b[i];
    }
#endif
}

static inline uint8_t kuznechik_gf_mul(uint8_t a, uint8_t b) {
    uint8_t c = 0;
    uint8_t hi_bit;
    for (int i = 0; i < 8; i++)
    {
        if (b & 1)
            c ^= a;
        hi_bit = a & 0x80;
        a <<= 1;
        if (hi_bit)
            a ^= 0xc3; //polinom x^8+x^7+x^6+x+1
        b >>= 1;
    }
    return c;
}

static inline void kuznechik_r_transformation(uint8_t* state) {
    uint8_t a_0 = state[15];
    for (int i = 14; i >= 0; i--) {
        a_0 ^= kuznechik_gf_mul(state[i], l_vec[i]);
        state[i + 1] = state[i];
    }
    state[0] = a_0;
}

static inline void kuznechik_reverse_r_transformation(uint8_t* state) {
    uint8_t a_15 = state[0];
    uint8_t sum = 0;
    for (int i = 0; i < 15; i++) {
        uint8_t byte = state[i + 1];
        sum ^= kuznechik_gf_mul(byte, l_vec[i]);
        state[i] = byte;
    }
    state[15] = a_15 ^ sum;
}

static void kuznechik_l_transformation(const uint8_t* in_data, uint8_t* out_data) {
    vect internal;
    memcpy(internal, in_data, BLOCK_SIZE);
    for (int i = 0; i < 16; i++) {
        kuznechik_r_transformation(internal);
    }
    memcpy(out_data, internal, BLOCK_SIZE);
}

static void kuznechik_reverse_l_transformation(const uint8_t* in_data, uint8_t* out_data) {
    vect internal;
    memcpy(internal, in_data, BLOCK_SIZE);
    for (int i = 0; i < 16; i++) {
        kuznechik_reverse_r_transformation(internal);
    }
    memcpy(out_data, internal, BLOCK_SIZE);
}

static void kuznechik_get_iter_const(kuznechik_ctx* ctx) {
#ifdef DEBUG_MODE
    printf("Iteration constants:\n");
#endif
    vect iter_num[32];
    for (int i = 0; i < 32; i++) {
        memset(iter_num[i], 0, BLOCK_SIZE);
        iter_num[i][15] = i+1;
    }
    for (int i = 0; i < 32; i++) {
         kuznechik_l_transformation(iter_num[i], ctx->iter_const[i]);
#ifdef DEBUG_MODE
         kuznechik_print_debug(ctx->iter_const[i]);
#endif
    }
}

static void kuznechik_f_transformation(const uint8_t* in_key_1, const uint8_t* in_key_2,
                           uint8_t* out_key_1, uint8_t* out_key_2,
                           uint8_t* iter_const) {
    vect internal;
    memcpy(out_key_2, in_key_1, BLOCK_SIZE);
    kuznechik_xor(in_key_1, iter_const, internal);
    kuznechik_s_transformation(internal, internal);
    kuznechik_l_transformation(internal, internal);
    kuznechik_xor(internal, in_key_2, out_key_1);
}

void kuznechik_init(kuznechik_ctx* ctx, const uint8_t* key) {
    uint8_t iter_1[KEY_SIZE >> 1];
    uint8_t iter_2[KEY_SIZE >> 1];
    uint8_t iter_3[KEY_SIZE >> 1];
    uint8_t iter_4[KEY_SIZE >> 1];

    memcpy(ctx->iter_key[0], key, KEY_SIZE >> 1);
    memcpy(ctx->iter_key[1], key + (KEY_SIZE >> 1), KEY_SIZE >> 1);
    kuznechik_get_iter_const(ctx);

    memcpy(iter_1, ctx->iter_key[0], KEY_SIZE >> 1);
    memcpy(iter_2, ctx->iter_key[1], KEY_SIZE >> 1);
    for (int i = 0; i < 4; i++) {
        kuznechik_f_transformation(iter_1, iter_2, iter_3, iter_4, ctx->iter_const[0 + 8 * i]);
        kuznechik_f_transformation(iter_3, iter_4, iter_1, iter_2, ctx->iter_const[1 + 8 * i]);
        kuznechik_f_transformation(iter_1, iter_2, iter_3, iter_4, ctx->iter_const[2 + 8 * i]);
        kuznechik_f_transformation(iter_3, iter_4, iter_1, iter_2, ctx->iter_const[3 + 8 * i]);
        kuznechik_f_transformation(iter_1, iter_2, iter_3, iter_4, ctx->iter_const[4 + 8 * i]);
        kuznechik_f_transformation(iter_3, iter_4, iter_1, iter_2, ctx->iter_const[5 + 8 * i]);
        kuznechik_f_transformation(iter_1, iter_2, iter_3, iter_4, ctx->iter_const[6 + 8 * i]);
        kuznechik_f_transformation(iter_3, iter_4, iter_1, iter_2, ctx->iter_const[7 + 8 * i]);
        memcpy(ctx->iter_key[2 * i + 2], iter_1, KEY_SIZE >> 1);
        memcpy(ctx->iter_key[2 * i + 3], iter_2, KEY_SIZE >> 1);
    }
#ifdef DEBUG_MODE
    printf("Iteration cipher keys:\n");
    for (int i = 0; i < 10; i++)
        kuznechik_print_debug(ctx->iter_key[i]);
#endif
}

void kuznechik_encrypt(kuznechik_ctx* ctx, const uint8_t* plain_block, uint8_t* cipher_block) {
    memcpy(cipher_block, plain_block, BLOCK_SIZE);
#ifdef DEBUG_MODE
    printf("Text:\n");
    kuznechik_print_debug(cipher_block);
#endif
    for(int i = 0; i < 9; i++) {
        kuznechik_xor(ctx->iter_key[i], cipher_block, cipher_block);
        kuznechik_s_transformation(cipher_block, cipher_block);
        kuznechik_l_transformation(cipher_block, cipher_block);
    }
    kuznechik_xor(cipher_block, ctx->iter_key[9], cipher_block);
#ifdef DEBUG_MODE
    printf("Encrypted text:\n");
    kuznechik_print_debug(cipher_block);
#endif
}

void kuznechik_decrypt(kuznechik_ctx* ctx, const uint8_t* cipher_bock, uint8_t* plain_block) {
    memcpy(plain_block, cipher_bock, BLOCK_SIZE);
#ifdef DEBUG_MODE
    printf("Gipher text:\n");
    kuznechik_print_debug(plain_block);
#endif
    kuznechik_xor(plain_block, ctx->iter_key[9], plain_block);
    for(int i = 8; i >= 0; i--)
    {
        kuznechik_reverse_l_transformation(plain_block, plain_block);
        kuznechik_reverse_s_transformation(plain_block, plain_block);
        kuznechik_xor(ctx->iter_key[i], plain_block, plain_block);
    }
#ifdef DEBUG_MODE
    printf("Decrypted text:\n");
    kuznechik_print_debug(plain_block);
#endif
}

