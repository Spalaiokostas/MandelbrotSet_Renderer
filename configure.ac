#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([MandelbrotSet_renderer], [0.2], [pkspyros@gmail.com])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([X11], [XOpenDisplay],[],[AC_MSG_ERROR([X11 library missing, please install it libx11-dev..])])
AC_CHECK_LIB([gmp], [__gmpz_init],[], [AC_MSG_ERROR([GNU MP not found, se https://gmplib.org/])])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([stdlib.h])


AC_OPENMP

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
