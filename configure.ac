AC_INIT([Wicher Server], [0.1.1], [maju@majudev.net])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_MACRO_DIR([m4])
AX_BOOST_BASE([1.58])
AX_BOOST_THREAD
AX_BOOST_SYSTEM
AC_PROG_CXX
AC_CONFIG_HEADERS([buildconfig.h])
PKG_CHECK_MODULES([jansson], [jansson])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT
