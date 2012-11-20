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

#ifndef __VOLTAIRE_VINDEX_H
#define __VOLTAIRE_VINDEX_H

#include <sys/queue.h>
#include <stdint.h>
#include <stdlib.h>

struct key {
        char                    *name;
        uint8_t                  name_len;
        TAILQ_ENTRY(key)         keys;
};
TAILQ_HEAD(tq_key, key);

struct vbucket {
        uint8_t         *hash;
        uint8_t         *value;
        uint64_t         value_len;
        uint64_t         nkeys;
        struct tq_key   *keys;
};


char            *voltaire_bucket_filename(char *);
size_t           voltaire_bucket_filename_len(void);
struct vbucket  *voltaire_bucket_create(uint8_t *, uint64_t, char *, uint64_t);
int              voltaire_bucket_add(struct vbucket *, char *, uint64_t);
int              voltaire_bucket_has(struct vbucket *, char *, uint64_t);
int              voltaire_bucket_del(struct vbucket *, char *, uint64_t);
int              voltaire_bucket_destroy(struct vbucket **);

#endif
