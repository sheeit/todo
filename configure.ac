AC_PREREQ([2.69])
AC_INIT([todo], [0.2], [strupo@autistici.org])
AM_INIT_AUTOMAKE([-Wall -Werror -Wportability gnu])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIZE_T

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
