/*
 * Copyright (c) 2012 Kyle Isom <kyle@tyrfingr.is>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * ---------------------------------------------------------------------
 */

#include <gcrypt.h>
#include <stdio.h>


#define HASH_ALGO       GCRY_MD_SHA1


/*
 * Returns the size of the algorithm's data length. This is provided
 * so users do not have to know the internal algorithm being used.
 * hashlen returns the binary size of the hash, while hexhashlen
 * returns the length of the hexdigest.
 */
int
voltaire_get_hashlen()
{
        return gcry_md_get_algo_dlen(HASH_ALGO);
}

int
voltaire_get_hexhashlen()
{
       return 2 * gcry_md_get_algo_dlen(HASH_ALGO);
}


/*
 * generate a binary hash for the given data. if hash is NULL,
 * memory will be allocated for it. otherwise, it is up to the
 * caller to ensure that hash is of the appropriate size.
 */
int
voltaire_hash_data(uint8_t *data, size_t data_len, uint8_t **hash)
{
        int hashlen, i;

        hashlen = voltaire_get_hashlen();
        if (*hash == NULL) {
                *hash = (uint8_t *)malloc(sizeof(uint8_t) * hashlen);
                if (NULL == *hash)
                        return EXIT_FAILURE;
        }
        gcry_md_hash_buffer(HASH_ALGO, *hash, data, data_len);
        return EXIT_SUCCESS;
}


/*
 * generate a hexadecimal hash for the given data. if hash is NULL,
 * memory will be allocated for it. otherwise, it is up to the
 * caller to ensure that hash is of the appropriate size.
 */
int
voltaire_hexhash_data(uint8_t *data, size_t data_len, uint8_t **hash)
{
        int binhashlen, hashlen, i;
        uint8_t *binhash = NULL;
        uint8_t *hashp = NULL;

        binhashlen = voltaire_get_hashlen();
        hashlen = voltaire_get_hexhashlen();
        if (*hash == NULL) {
                *hash = (uint8_t *)malloc(sizeof(uint8_t) * hashlen);
                if (NULL == *hash)
                        return EXIT_FAILURE;
                hashp = *hash;
        }

        if (EXIT_SUCCESS != voltaire_hash_data(data, data_len, &binhash)) {
                free(hash);
                return EXIT_FAILURE;
        }

        for (i = 0; i < binhashlen; i++, hashp += 2 ) {
                char hex[3];
                snprintf(hex, 3, "%02x", binhash[i]);
                strncat(hashp, hex, 2);
        }
        return EXIT_SUCCESS;
}
