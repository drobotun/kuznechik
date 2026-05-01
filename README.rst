=============================
GOST R 34.12 - 2015 "Kuznechik"
=============================

.. image:: https://img.shields.io/github/license/drobotun/virustotalapi3?style=flat
    :target: http://doge.mit-license.org
.. image:: https://img.shields.io/badge/HACKMAG-blue?color=purple
    :target: https://hackmag.com/stuff/working-with-grasshopper
.. image:: https://img.shields.io/badge/-%5D%5Bakep-blue?color=blue
    :target: https://xakep.ru/2017/02/02/working-with-grasshopper/

Implementation of the Kuznyechik block cipher (GOST R 34.12-2015) in C.
This repository contains an implementation of the Russian national standard
for block cipher Kuznyechik (also known as `GOST R 34.12-2015`_).
The algorithm uses 128‑bit blocks and 256‑bit keys.

.. _GOST R 34.12-2015: https://tc26.ru/standard/gost/GOST_R_3412-2015.pdf

Features
--------

- Pure C (C99) implementation, no external crypto libraries.
- Supports encryption and decryption.
- Full key expansion with iterative constants.
- Optional debug output when compiled with ``DEBUG_MODE``.
- Includes test vectors in a separate header.

Project Structure
-----------------

::

   ├── kuznechik_calc.h    // public API declarations
   ├── kuznechik_calc.c    // algorithm implementation
   ├── kuznechik_const.h   // constants (pi, reverse_pi, l_vec)
   ├── kuznechik_test.h    // test vectors (key, plaintext, expected ciphertext)
   └── README.rst

Requirements
------------

- C99 compatible compiler (GCC, Clang, MSVC).
- Standard headers: ``stdint.h``, ``string.h``, ``stdio.h``, ``stdlib.h``.

Building
--------

To use in your project:

- Copy the files ``kuznechik_calc.h``, ``kuznechik_const.h``,
  ``kuznechik_calc.c`` into your project directory.
- Include the header ``kuznechik_calc.h``.
- Compile your project together with ``kuznechik_calc.c``::

   gcc -o kuznechik main.c kuznechik_calc.c

To enable debug output (prints round keys, intermediate states)::

   gcc -DDEBUG_MODE -o kuznechik main.c kuznechik_calc.c

Usage
-----

Key Setup
~~~~~~~~~

Before encryption or decryption, you must initialise a ``kuznechik_ctx``
structure with the 256‑bit key:

.. code-block:: c

   #include "kuznechik_calc.h"

   uint8_t key[KEY_SIZE] = { ... };   // 32 bytes
   kuznechik_ctx ctx;
   kuznechik_init(&ctx, key);

Encryption
~~~~~~~~~~

``kuznechik_encrypt()`` takes a plaintext block (128 bits) and writes the
ciphertext block to the output buffer:

.. code-block:: c

   uint8_t plaintext[BLOCK_SIZE] = { ... };
   uint8_t ciphertext[BLOCK_SIZE];
   kuznechik_encrypt(&ctx, plaintext, ciphertext);

Decryption
~~~~~~~~~~

Use ``kuznechik_decrypt()`` to reverse the process:

.. code-block:: c

   uint8_t decrypted[BLOCK_SIZE];
   kuznechik_decrypt(&ctx, ciphertext, decrypted);

Full Example (with test vectors)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The header ``kuznechik_test.h`` provides a known key, plaintext, and the
expected ciphertext (from the standard). You can use it to verify the
implementation:

.. code-block:: c

   #include <stdio.h>
   #include <string.h>
   #include "kuznechik_calc.h"
   #include "kuznechik_test.h"

   int main() {
       uint8_t ciphertext[BLOCK_SIZE];
       uint8_t decrypted[BLOCK_SIZE];

       kuznechik_ctx ctx;
       kuznechik_init(&ctx, test_key);
       kuznechik_encrypt(&ctx, encrypt_test_string, ciphertext);
       kuznechik_decrypt(&ctx, decrypt_test_string, decrypted);

       // Check against expected ciphertext (decrypt_test_string)
       int encrypt_ok = memcmp(ciphertext, decrypt_test_string, BLOCK_SIZE) == 0;
       int decrypt_ok = memcmp(decrypted, encrypt_test_string, BLOCK_SIZE) == 0;

       printf("Encryption %s\n", encrypt_ok ? "correct" : "failed");
       printf("Decryption %s\n", decrypt_ok ? "correct" : "failed");
       return 0;
   }

**Note:** The test vectors are:

- Key: ``test_key`` (32 bytes)
- Plaintext: ``encrypt_test_string`` (16 bytes)
- Expected ciphertext: ``decrypt_test_string`` (16 bytes)

API Reference
-------------

- ``vect`` - 128‑bit block (aligned to 16 bytes). Defined as ``uint8_t[BLOCK_SIZE]``.
- ``kuznechik_ctx`` - context structure that stores iterative constants and round keys.
- ``void kuznechik_init(kuznechik_ctx* ctx, const uint8_t* key)`` - expands the 256‑bit key and fills the context.

    Parameters:

    - ``ctx`` - pointer to context structure.
    - ``key`` - pointer to 32‑byte key array.

- ``void kuznechik_encrypt(kuznechik_ctx *ctx, const uint8_t *plain_block, uint8_t *cipher_block)`` - encrypts a single 128‑bit block.

    Parameters:

    - ``ctx`` - initialised context.
    - ``plain_block`` - input plaintext block (16 bytes).
    - ``cipher_block`` - output ciphertext block (16 bytes).

- ``void kuznechik_decrypt(kuznechik_ctx *ctx, const uint8_t *cipher_block, uint8_t *plain_block)`` - decrypts a single 128‑bit block.

    Parameters:

    - ``ctx`` - initialised context.
    - ``cipher_block`` - input ciphertext block (16 bytes).
    - ``plain_block`` - output plaintext block (16 bytes).

Internal Transformations
------------------------

The implementation uses the following operations (defined in
``kuznechik_const.h``):

- **S‑box** (non‑linear substitution) – tables ``pi`` and its inverse
  ``reverse_pi``.
- **R‑transformation** – multiplication in GF(2⁸) with polynomial
  :math:`x^8 + x^7 + x^6 + x + 1` (0xC3).
- **L‑transformation** – 16 consecutive R‑transformations.
- **F‑transformation** – used during key expansion.

Debug Mode
----------

When ``DEBUG_MODE`` is defined, the following data is printed during execution:

- Round keys after expansion.
- Plaintext before encryption.
- Ciphertext after encryption.
- Ciphertext before decryption.
- Decrypted plaintext after decryption.

To disable debug output, remove the ``-DDEBUG_MODE`` compiler flag.

Notes
-----

- The implementation does not use dynamic memory allocation (apart from
  constant global arrays).
- Block size (``BLOCK_SIZE``) and key size (``KEY_SIZE``) are fixed as per
  the standard.
- The test header ``kuznechik_test.h`` is **not required** for the library to
  work; it is provided for verification purposes only.

License
-------

MIT
Copyright (c) 2019 Evgeny Drobotun

-----
