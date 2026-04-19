#include "gost_3412_2015_calc.h"
#include "test.h"

int main(int argc, char *argv[])
{
    vect gipher_blk;
    kuznechik_expand_key(test_key);

    kuznechik_encrypt(encrypt_test_string, gipher_blk);

    kuznechik_decrypt(decrypt_test_string, gipher_blk);
    return 0;
}
