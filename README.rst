=============================
GOST 34.12 - 2015 "Kuznechik"
=============================

.. image:: https://img.shields.io/github/license/drobotun/virustotalapi3?style=flat
    :target: http://doge.mit-license.org
.. image:: https://img.shields.io/badge/HACKMAG-blue?color=purple
    :target: https://hackmag.com/stuff/working-with-grasshopper
.. image:: https://img.shields.io/badge/-%5D%5Bakep-blue?color=blue
    :target: https://xakep.ru/2017/02/02/working-with-grasshopper/

Implementation of the Kuznyechik block cipher (`GOST 34.12-2015 <https://tc26.ru/standard/gost/GOST_R_3412-2015.pdf>`_) in C

This repository contains an implementation of the Russian national standard for block cipher **Kuznyechik** (also known as `GOST 34.12-2015 <https://tc26.ru/standard/gost/GOST_R_3412-2015.pdf>`_). The algorithm uses 128‑bit blocks and 256‑bit keys.

Features
++++++++

- Pure C (C99) implementation, no external crypto libraries.
- Supports encryption and decryption.
- Full key expansion with iterative constants.
- Optional debug output when compiled with ``DEBUG_MODE`` (plaintext, round keys, ciphertext, etc.).

Project Structure
+++++++++++++++++

::

    ├── kuznechik_calc.h       // header with function declarations
    ├── kuznechik_calc.c       // algorithm implementation
    ├── kuznechik_const.h      // constants (pi, reverse_pi, l_vec)
    └── kuznechik_test.h       // optional test helper

Requirements
++++++++++++

- C99 compatible compiler (GCC, Clang, MSVC).
- Standard headers: ``stdint.h``, ``string.h``, ``stdio.h``, ``stdlib.h``.

Building
++++++++

To use in your project:

1. Copy the files ``kuznechik_calc.h``, ``kuznechik_const.h``, ``kuznechik_calc.c`` and ``kuznechik_test.h`` (if necessary) into your project directory.
2. Include the header files ``kuznechik_calc.h``, ``kuznechik_const.h`` and ``kuznechik_test.h`` (if necessary).
3. Compile your project together with ``kuznechik_calc.c``.


.. code-block:: bash

    gcc -o kuznechik main.c kuznechik_calc.c 

To enable debug output, add ``-DDEBUG_MODE``:

.. code-block:: bash

    gcc -DDEBUG_MODE -o kuznechik main.c kuznechik_calc.c 

Usage
+++++

Key Setup
---------

Before encryption or decryption, expand the key with ``kuznechik_expand_key()``:

.. code-block:: c

    uint8_t key[KEY_SIZE] = { ... };  // 32 bytes (256 bits)
    kuznechik_expand_key(key);

Encryption
----------

``kuznechik_encrypt()`` takes a plaintext block (128 bits) and writes the ciphertext block to ``out_blk``:

.. code-block:: c

    uint8_t plaintext[BLOCK_SIZE] = { ... };
    uint8_t ciphertext[BLOCK_SIZE];
    kuznechik_encrypt(plaintext, ciphertext);

Decryption
----------

Use ``kuznechik_decrypt()`` to reverse the process:

.. code-block:: c

    uint8_t decrypted[BLOCK_SIZE];
    kuznechik_decrypt(ciphertext, decrypted);

Full Example
------------

.. code-block:: c

    #include <stdio.h>
    #include <string.h>
    #include "gost_3412_2015_calc.h"

    int main() {
        // 256-bit key (32 bytes)
        uint8_t key[KEY_SIZE] = {
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
            0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
        };

        // 128-bit data block
        uint8_t plaintext[BLOCK_SIZE] = {
            0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
            0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00
        };

        uint8_t ciphertext[BLOCK_SIZE];
        uint8_t decrypted[BLOCK_SIZE];

        kuznechik_expand_key(key);
        kuznechik_encrypt(plaintext, ciphertext);
        kuznechik_decrypt(ciphertext, decrypted);

        printf("Decryption %s\n", memcmp(plaintext, decrypted, BLOCK_SIZE) == 0 ? "successful" : "failed");

        return 0;
    }

API Reference
+++++++++++++

+--------------------------------+--------------------------------------------------------+
| Function                       | Description                                            |
+================================+========================================================+
| ``kuznechik_expand_key(const uint8_t* key)`` | Expands the original 256‑bit key into 10 round keys (stored in the global ``iter_key`` array). |
+--------------------------------+--------------------------------------------------------+
| ``kuznechik_encrypt(const uint8_t* blk, uint8_t* out_blk)`` | Encrypts a single 128‑bit block.          |
+--------------------------------+--------------------------------------------------------+
| ``kuznechik_decrypt(const uint8_t* blk, uint8_t* out_blk)`` | Decrypts a single 128‑bit block.          |
+--------------------------------+--------------------------------------------------------+

Internal Transformations
++++++++++++++++++++++++

The implementation uses the following operations (defined in ``gost_3412_2015_const.h``):

- **S‑box** (non‑linear substitution) – tables ``pi`` and its inverse ``reverse_pi``.
- **R‑transformation** – multiplication in GF(2\ :sup:`8`) with polynomial ``x^8 + x^7 + x^6 + x + 1`` (0xC3).
- **L‑transformation** – 16 consecutive R‑transformations.
- **F‑transformation** – used during key expansion.

Debug Mode
++++++++++

When ``DEBUG_MODE`` is defined, the following data is printed during execution:

- Round keys after expansion.
- Plaintext before encryption.
- Ciphertext after encryption.
- Ciphertext before decryption.
- Decrypted plaintext after decryption.

To disable debug output, remove the ``-DDEBUG_MODE`` compiler flag or comment out ``#define DEBUG_MODE`` in the source code.

Notes
+++++

- The implementation does **not** use dynamic memory allocation (apart from constant global arrays).
- Block size (``BLOCK_SIZE``) and key size (``KEY_SIZE``) are fixed as per the standard.
- The constants ``pi``, ``reverse_pi``, and ``l_vec`` must be defined in ``gost_3412_2015_const.h``. This file is assumed to be provided separately.

License
+++++++

MIT Copyright (c) 2019 Evgeny Drobotun

-----
