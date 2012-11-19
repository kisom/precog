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
#include <stdint.h>
#include <sysexits.h>

#include "hash.h"

static void     test_hello_world(void);


/*
 * Suite set up and tear down functions.
 */
int
initialise_hash_test()
{
        return 0;
}

int
cleanup_hash_test()
{
        return 0;
}

void destroy_test_registry()
{
        CU_ErrorCode cu_err;
        CU_cleanup_registry();
        cu_err = CU_get_error();
        exit((int)cu_err);
}


/*
 * Ensure that the hex digest creates the appropriate value; this
 * will test both the hex digest and binary digest (as the binary
 * digest is part of the hex digest code).
 */
void
test_hello_world()
{
        int      rv;
        char     hw[] = "hello, world";
        char     hw_hexhash[] = "b7e23ec29af22b0b4e41da31e868d57226121c84";
        char    *hash = NULL;

        rv = voltaire_hexhash_data((uint8_t *)hw, strlen(hw), (uint8_t **)(&hash));
        CU_ASSERT(rv == EXIT_SUCCESS);

        CU_ASSERT(0 == strncmp(hw_hexhash, hash, strlen(hw_hexhash)));
        free(hash);
}


/*
 * hash_test runs checks to ensure the hashing code works properly.
 */
int
main(void)
{
        CU_pSuite        hash_suite = NULL;
        unsigned int     fails = 0;

        printf("[+] running tests for hash functions.\n");

        if (!(CUE_SUCCESS == CU_initialize_registry())) {
                errx(EX_CONFIG, "failed to initialise CUnit test registry");
                return EXIT_FAILURE;
        }

        hash_suite = CU_add_suite("hash_tests", initialise_hash_test,
            cleanup_hash_test);
        if (NULL == hash_suite)
                destroy_test_registry();

        if (NULL == CU_add_test(hash_suite, "hello world test",
                    test_hello_world))
                destroy_test_registry();

        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
        fails = CU_get_number_of_tests_failed();
        printf("[+] %u tests failed\n", fails);

        CU_cleanup_registry();
        return fails;
}
