AC_INIT([Wicher Server], [0.1.1], [maju@majudev.net])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_MACRO_DIR([m4])
AX_BOOST_BASE([1.58])
AX_BOOST_THREAD
AX_BOOST_SYSTEM
AC_PROG_CXX
AC_CONFIG_HEADERS([buildconfig.h])
PKG_CHECK_MODULES([jansson], [jansson])

AX_CXX_COMPILE_STDCXX([11], [ext], [mandatory])

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
])
AC_OUTPUT
