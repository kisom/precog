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


#include <sys/types.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "hash.h"
#include "vindex.h"

void    test_simple_bucket(void);


/*
 * Suite set up and tear down functions.
 */
int
initialise_vindex_test()
{
        return 0;
}

int
cleanup_vindex_test()
{
        return 0;
}

void destroy_test_registry()
{
        CU_cleanup_registry();
        exit(CU_get_error());
}


/*
 * Ensure we can properly generate file names.
 */
void
test_filename_simple()
{
        size_t   fn_len = voltaire_bucket_filename_len();
        char     hw_hash[] = "b7e23ec29af22b0b4e41da31e868d57226121c84";
        char     expected[] = "b7/e23ec29af22b0b4e41da31e868d57226121c84";
        char     filename[42];      /* SHA-1 is 40b + path separator + NULL */
        char    *tmp_filename;

        tmp_filename = voltaire_bucket_filename(hw_hash);
        CU_ASSERT(NULL != tmp_filename);

        memcpy(filename, tmp_filename, fn_len);
        free(tmp_filename);
        CU_ASSERT(0 == strncmp(expected, filename, fn_len));
}


/*
 * Ensure that the simplest possible bucket is created as it should be.
 */
void
test_simple_bucket()
{
        char            testval[] = "hello, world";
        char            testhash[] = "b7e23ec29af22b0b4e41da31e868d57226121c84";
        char            testkey[] = "foo";
        uint64_t        tv_len;
        uint64_t        th_len;
        uint64_t        tk_len;
        int             rv;
        struct vbucket *bucket;

        tv_len = strlen(testval);
        th_len = strlen(testhash);
        tk_len = strlen(testkey);
        bucket = voltaire_bucket_create((uint8_t *)testval, tv_len, testkey,
            tk_len);

        CU_ASSERT(NULL != bucket);
        CU_ASSERT(NULL != bucket->hash);
        CU_ASSERT(0 == strncmp(testhash, (char *)bucket->hash, th_len));
        CU_ASSERT(0 == strncmp(testval, (char *)bucket->value, tv_len));
        CU_ASSERT(bucket->value_len == tv_len);
        CU_ASSERT(1 == bucket->nkeys);
        CU_ASSERT(voltaire_bucket_has(bucket, "foo", (uint64_t)3));
        CU_ASSERT(!voltaire_bucket_has(bucket, "bar", (uint64_t)3));
        rv = voltaire_bucket_del(bucket, "foo", (uint64_t)3);
        CU_ASSERT(1 == rv);
        if (1 == rv)
                bucket = NULL;
        CU_ASSERT(NULL == bucket);
}


/*
 * vindex_test ensures that the value bucket-store / inverted index
 * functions properly.
 */
int
main(void)
{
        CU_pSuite        vindex_suite = NULL;
        unsigned int     fails = 0;

        printf("[+] running tests for value bucket functions.\n");

        if (!(CUE_SUCCESS == CU_initialize_registry())) {
                errx(EX_CONFIG, "failed to initialise CUnit test registry");
                return EXIT_FAILURE;
        }

        vindex_suite = CU_add_suite("vindex_tests", initialise_vindex_test,
            cleanup_vindex_test);
        if (NULL == vindex_suite)
                destroy_test_registry();

        if (NULL == CU_add_test(vindex_suite, "simple filename test",
                    test_filename_simple))
                destroy_test_registry();

        if (NULL == CU_add_test(vindex_suite, "simple bucket test",
                    test_simple_bucket))
                destroy_test_registry();

        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
        fails = CU_get_number_of_tests_failed();
        printf("[+] %u tests failed\n", fails);

        CU_cleanup_registry();
        return fails;
}
