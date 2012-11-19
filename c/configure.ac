AC_PREREQ([2.59])
AC_INIT([voltaire],
        [0.1.0],
        [coder@kyleisom.net], [],
        [https://github.com/kisom/precog])
AM_INIT_AUTOMAKE([1.12 foreign no-define color-tests parallel-tests])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([src/vindex.h])
AC_CHECK_HEADERS
AC_CHECK_HEADER([sys/queue.h], [], 
    [AC_MSG_ERROR([could not find queue library])])
AC_CONFIG_HEADERS([src/config.h])
AC_CANONICAL_HOST
AC_MSG_WARN([need to write: tests, docs])
#AC_CONFIG_FILES(test/Makefile doc/Makefile src/Makefile Makefile)
AC_CONFIG_FILES([tests/Makefile src/Makefile Makefile])
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_RANLIB

dnl set up some additional useful macros
AC_DEFINE([PACKAGE_LICENSE], ["License ISC:"],
          [license used by the package])
AC_DEFINE([PACKAGE_LICENSE_URL], 
          ["http://tyrfingr.is/licenses/LICENSE.ISC"],
          [where the user can find a copy of the license])
AC_DEFINE([PACKAGE_COPYRIGHT], 
          ["Copyright (c) 2012 Kyle Isom <kyle@tyrfingr.is>"],
          [copyright tagline])

dnl

dnl check for libgcrypt
AC_SEARCH_LIBS(gcry_check_version, [gcrypt], 
               [have_gcrypt="yes"], [have_gcrypt="no"])
NO_GCRYPT="
        Could not find required library libgcrypt.
"
 
if test "x${have_gcrypt}" = xno; then
    AC_MSG_ERROR([${NO_GCRYPT}])
fi


dnl check for libcunit
AC_SEARCH_LIBS([CU_initialize_registry], [cunit], 
               [have_cunit="yes"], [have_cunit="no"])
CANNOT_TEST_WARN="
    CUnit not found; this is required for the unit tests. You will not 
    be able to run the test suite!
"
 
if test "x${have_cunit}" = xno; then
   AC_MSG_WARN([${CANNOT_TEST_WARN}])
fi

AC_OUTPUT

