AC_INIT([libxmldiff],[0.2.10pre])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE
LT_INIT
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL

AC_C_CONST
AC_CHECK_FUNCS([memmove])
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_FUNC_VPRINTF

dnl libxml
dnl AM_PATH_XML2( 2.0.0, , AC_MSG_ERROR(libxml2 must be installed.) )
dnl AM_PATH_XSLT(,, AC_MSG_ERROR(libxslt must be installed.) )
dnl AC_CHECK_LIB(xslt, main, , AC_MSG_ERROR(libxslt must be installed.))
dnl AC_CHECK_LIB(exslt, main, , AC_MSG_ERROR(libexslt must be installed.))
PKG_CHECK_MODULES(XML, libxml-2.0)
PKG_CHECK_MODULES(XSLT, libxslt)
PKG_CHECK_MODULES(EXSLT, libexslt)

dnl GCOV

AC_ARG_ENABLE(gcov,
AS_HELP_STRING([--enable-gcov],[turn on test coverage @<:@default=no@:>@]),
[case "${enableval}" in
  yes) enable_gcov=true ;;
  no)  enable_gcov=false ;;
  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-gcov) ;;
esac], [enable_gcov=false ])

if test x$enable_gcov = xtrue ; then
  if test x"$GCC" != xyes; then
    AC_MSG_ERROR([gcov only works if gcc is used])
  fi

  GCOV_CFLAGS="-fprofile-arcs -ftest-coverage"
  AC_SUBST(GCOV_CFLAGS)

  dnl libtool 1.5.22 and lower strip -fprofile-arcs from the flags
  dnl passed to the linker, which is a bug; -fprofile-arcs implicitly
  dnl links in -lgcov, so we do it explicitly here for the same effect
  GCOV_LIBS=-lgcov
  AC_SUBST(GCOV_LIBS)
fi

AM_CONDITIONAL(ENABLE_GCOV, test x"$enable_gcov" = "xtrue")

AC_CHECK_PROGS(GCOV, gcov, false)
AC_CHECK_PROGS(LCOV, lcov, false)

dnl Exports
dnl a bug in libxml-2.0 pkg-config needs to include cflags in LIBS...
dnl and pkg-config does not seem to declare CXXFLAGS or CPPFLAGS, so it may be redundant, but more will not harm
DEBUG="-g"
LIBS="$DEBUG $LIBS $XML_LIBS $XSLT_LIBS $EXSLT_LIBS $GCOV_LIBS -static -static-libgcc -static-libstdc++  "
CXXFLAGS="$DEBUG $CXXFLAGS $XML_CXXFLAGS $XSLT_CXXFLAGS  $EXSLT_CXXFLAGS $XML_CFLAGS $XSLT_CFLAGS $EXSLT_CFLAGS  -Wno-write-strings $GCOV_CFLAGS"
CPPFLAGS="$DEBUG $CPPFLAGS $XML_CPPFLAGS $XSLT_CPPFLAGS  $EXSLT_CPPFLAGS $XML_CFLAGS $XSLT_CFLAGS $EXSLT_CFLAGS  -Wno-write-strings $GCOV_CFLAGS"
CFLAGS="$DEBUG $CFLAGS $XML_CFLAGS $XSLT_CFLAGS $EXSLT_CFLAGS -Wno-write-strings $GCOV_LIBS"


AC_CONFIG_FILES([\
    Makefile \
    src/Makefile    \
    libxmldiff.pc \
	doc/libxmldiff.dox \
])
AC_OUTPUT
