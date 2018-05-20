#include "gost_3412_2015_calc.h"
#include "test.h"

int main(int argc, char *argv[])
{
    vect gipher_blk;
    GOST_Kuz_Expand_Key(test_key[0], test_key[1]);

    GOST_Kuz_Encrypt(encrypt_test_string, gipher_blk);

    GOST_Kuz_Decrypt(decrypt_test_string, gipher_blk);
    return 0;
}
