#include "kuznechik_calc.h"
#include "kuznechik_test.h"

int main(int argc, char* argv[])
{
    vect block;
    kuznechik_ctx ctx;
    kuznechik_init(&ctx, test_key);
    kuznechik_encrypt(&ctx, encrypt_test_string, block);
    kuznechik_decrypt(&ctx, decrypt_test_string, block);
    return 0;
}
