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
        exit(CU_get_error);
}


/*
 * Ensure that the hex digest creates the appropriate value; this
 * will test both the hex digest and binary digest (as the binary
 * digest is part of the hex digest code).
 */
void
test_simple_bucket()
{
        CU_ASSERT(1 == 0);
}


/*
 * vindex_test ensures that the value bucket-store / inverted index
 * functions properly.
 */
int
main(void)
{
        CU_pSuite vindex_suite = NULL;
        unsigned int fails = 0;

        printf("[+] running tests for value bucket functions.\n");

        if (!(CUE_SUCCESS == CU_initialize_registry())) {
                errx(EX_CONFIG, "failed to initialise CUnit test registry");
                return EXIT_FAILURE;
        }

        vindex_suite = CU_add_suite("vindex_tests", initialise_vindex_test,
            cleanup_vindex_test);
        if (NULL == vindex_suite)
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
