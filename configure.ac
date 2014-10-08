AC_INIT([libbitcoin-protocol], [0.0.1], [mienkphi@gmail.com])
AC_USE_SYSTEM_EXTENSIONS
AC_LANG(C++)
AC_CONFIG_AUX_DIR([build-aux])

# The BOOST_ROOT environment variable is *not* prioritize by the m4 tests.
# As a result there may be dependency conflicts between installed versions
# and a version specified in BOOST_ROOT.
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror])
AM_PROG_AR
LT_INIT
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_GNU_SOURCE
AX_CXX_COMPILE_STDCXX_11(noext,mandatory)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Boost doesn't publish package configs but has m4 tests.
# The current versions produce incorrect error message text.
AX_BOOST_BASE([1.50],,
    [AC_MSG_ERROR([Boost 1.50 or later is required but was not found.])])
AX_BOOST_UNIT_TEST_FRAMEWORK

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([bitcoin], [libbitcoin >= 2.3.0])
PKG_CHECK_MODULES([protobuf], [protobuf >= 2.6.0])
PKG_CHECK_MODULES([czmqpp], [libczmq++ >= 0.4.1])

AC_ARG_WITH([pkgconfigdir], AS_HELP_STRING([--with-pkgconfigdir=PATH],
    [Path to the pkgconfig directory [[LIBDIR/pkgconfig]]]),
    [pkgconfigdir="$withval"], [pkgconfigdir='${libdir}/pkgconfig'])
AC_SUBST([pkgconfigdir])

AC_CONFIG_FILES([Makefile libbitcoin-protocol.pc])
AC_OUTPUT

