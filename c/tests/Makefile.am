AM_CFLAGS = -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations   \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long    \
            -Wconversion -Wstrict-prototypes -g -O0 -I/usr/local/include  \
            -DGCRYPT_NO_DEPRECATED -I../src
AM_CFLAGS = -Wall -g -I/usr/local/include -I../src -DGCRYPT_NO_DEPRECATED \
            -O0
AM_LDFLAGS = -L/usr/local/lib -lcunit

include_HEADERS = *.h
check_PROGRAMS = hash_test vindex_test
hash_test_SOURCES = ../src/hash.c hash_test.c
vindex_test_SOURCES = ../src/vindex.c vindex_test.c
