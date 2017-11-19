AC_INIT([Wicher Server], [0.1.1], [maju@majudev.net])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC
AC_PROG_CXX
AC_CONFIG_HEADERS([buildconfig.h])
PKG_CHECK_MODULES([jansson], [jansson])

AX_CXX_COMPILE_STDCXX([11], [ext], [mandatory])

AX_PTHREAD([], AC_MSG_ERROR([pthreads are mandatory in order to build wicher-server])])
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

AC_CHECK_FUNCS([syslog])
     if test $ac_cv_func_syslog = no; then
       # syslog is not in the default libraries.  See if it's in some other.
       for lib in bsd socket inet; do
         AC_CHECK_LIB([$lib], [syslog], [AC_DEFINE([HAVE_SYSLOG])
           LIBS="-l$lib $LIBS"; break])
       done
     fi

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	cli/Makefile
])
AC_OUTPUT
