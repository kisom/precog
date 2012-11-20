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


#include <sys/queue.h>
#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hash.h"
#include "vindex.h"

/*
 * return the filename for a hash object.
 */
char
*voltaire_bucket_filename(char *hash)
{
        size_t   fn_len;
        char    *filename;
        char     prefix[3];
        int      hashlen;

        hashlen = voltaire_get_hexhashlen();
        fn_len = voltaire_bucket_filename_len();
        /* add two for the path separator '/' and NULL byte */
        filename = (char *)malloc(sizeof(char) * fn_len);
        if (NULL == filename)
                return filename;
        strncpy(prefix, hash, 2);
        snprintf(filename, fn_len, "%s/%s", prefix, hash + 2);

        return filename;
}


/*
 * return the length of a filename so the user can properly allocate
 * memory.
 */
size_t
voltaire_bucket_filename_len()
{
        return voltaire_get_hexhashlen() + 2;
}


/*
 * Create and initialise a new bucket. Pass in the value and an initial
 * key, and the function will return a new value bucket.
 */
struct vbucket
*voltaire_bucket_create(uint8_t *value, uint64_t value_len, char *ikey,
    uint64_t ikey_len)
{
        struct vbucket     *bkt;
        uint8_t            *hash;

        bkt = (struct vbucket *)malloc(sizeof(struct vbucket));
        if (bkt == NULL)
                return bkt;
        if (EXIT_FAILURE == voltaire_hexhash_data(value, value_len, &hash)) {
                free(bkt);
                return NULL;
        }

        bkt->hash = hash;
        bkt->value_len = value_len;
        bkt->value = (uint8_t *)malloc(sizeof(uint8_t) * value_len + 1);
        if (NULL == bkt->value)
                /* destroy bucket here */
                warnx("please destroy this bucket");
        else
                memcpy(bkt->value, value, value_len);

        bkt->keys = (struct tq_key *)malloc(sizeof(struct tq_key));
        if (NULL != bkt->keys) {
                TAILQ_INIT(bkt->keys);
                if (EXIT_SUCCESS == voltaire_bucket_add(bkt, ikey, ikey_len))
                        bkt->nkeys = 1;
                else
                        voltaire_bucket_destroy(&bkt);
        }

        return bkt;
}


/*
 * Add a key to a bucket.
 */
int
voltaire_bucket_add(struct vbucket *bkt, char *ikey, uint64_t ikey_len)
{
        struct key *new_key;

        new_key = (struct key *)malloc(sizeof(struct key));
        if (NULL == new_key)
                return EXIT_FAILURE;
        new_key->name = strdup(ikey);
        new_key->name_len = ikey_len;

        if (NULL != new_key->name) {
                TAILQ_INSERT_HEAD(bkt->keys, new_key, keys);
                bkt->nkeys++;
                return EXIT_SUCCESS;
        }
        return EXIT_FAILURE;
}


/*
 * look up whether a bucket contains a given key.
 */
int
voltaire_bucket_has(struct vbucket *bkt, char *key, uint64_t keylen)
{
        struct key      *current_key;
        int              found = 0;

        if (0 == bkt->nkeys || TAILQ_EMPTY(bkt->keys))
                return 0;

        TAILQ_FOREACH(current_key, bkt->keys, keys) {
                if (keylen != current_key->name_len)
                        continue;

                if (0 == strncmp(key, current_key->name, keylen)) {
                        found = 1;
                        break;
                }
        }
        return found;
}


/*
 * Remove a key from the list of keys associated with a given value.
 */
int
voltaire_bucket_del(struct vbucket *bkt, char *key, uint64_t keylen)
{
        struct key      *current_key = NULL;
        int              found = 0;

        TAILQ_FOREACH(current_key, bkt->keys, keys) {
                if (keylen != current_key->name_len)
                        continue;
                if (0 == strncmp(key, current_key->name, keylen)) {
                        found = 1;
                        break;
                }
        }
        if (1 == found) {
                free(current_key->name);
                TAILQ_REMOVE(bkt->keys, current_key, keys);
                bkt->nkeys--;
        }
        if (0 == bkt->nkeys || TAILQ_EMPTY(bkt->keys))
                return (0 == voltaire_bucket_destroy(&bkt));

        return 1;
}


/*
 * Destroy the bucket.
 */
int
voltaire_bucket_destroy(struct vbucket **bktp) {
        struct key      *current_key;

        while ((current_key = TAILQ_FIRST((*bktp)->keys))) {
                printf("[-] removing key %s\n", current_key->name);
                free(current_key->name);
                TAILQ_REMOVE((*bktp)->keys, current_key, keys);
        }
        free((*bktp)->keys);
        free((*bktp)->hash);
        free((*bktp)->value);
        free(*bktp);
        *bktp = NULL;
        return 0;
}


/*
 * Write a bucket to disk.
 */
int
voltaire_bucket_dump(struct vbucket *bkt, const char *topdir)
{
        struct key      *current_key;
        ssize_t          wrsz;
        char             filename[FILENAME_MAX];
        char            *bucket_name;
        int              bucket_fd, rv;

        bucket_name = voltaire_bucket_filename(bkt->hash);
        if (NULL == bucket_name)
                return EXIT_FAILURE;
        snprintf(filename, FILENAME_MAX, "%s/%s", topdir, bucket_name);

        bucket_fd = open(filename, O_WRONLY|O_CREAT|O_TRUNCATE,
            S_IRUSR | S_IWUSR);
        if (bucket < 1)
                return EXIT_FAILURE;

        wrsz = write(bucket_fd, bkt->value_len, sizeof(bkt->value_len));
        if (wrsz != sizeof(bkt->value_len)) {
                close(bucket_fd);
                return EXIT_FAILURE;
        }
        wrsz = write(bucket_fd, bkt->value, sizeof(bkt->value) * bkt->value_len);
        if (wrsz != (sizeof(bkt->value) * bkt->value_len)) {
                close(bucket_fd);
                return EXIT_FAILURE;
        }
        TAILQ_FOREACH(current_key, bkt->keys, keys) {
                wrsz = write(bucket_fd, current_key->name_len, sizeof(name_len));

                wrsz = write(bucket_fd, current_key->name,
                    sizeof(current_key->name) * current_key->name_len);
        }
}
