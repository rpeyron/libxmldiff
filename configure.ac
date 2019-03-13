AC_INIT(libxmldiff,0.2.8)
AC_CONFIG_SRCDIR([configure.ac])
AM_INIT_AUTOMAKE
AM_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL

dnl Stuff detected by autoscan
AC_HEADER_STDC
AC_C_CONST
AC_CHECK_FUNCS([memmove])
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_FUNC_VPRINTF

dnl libxml
AM_PATH_XML2( 2.0.0, , AC_MSG_ERROR(libxml2 must be installed.) )
AM_PATH_XSLT(,, AC_MSG_ERROR(libxslt must be installed.) )
dnl AC_CHECK_LIB(xslt, main, , AC_MSG_ERROR(libxslt must be installed.))
dnl AC_CHECK_LIB(exslt, main, , AC_MSG_ERROR(libexslt must be installed.))

dnl Exports
DEBUG="-g"
LIBS="$DEBUG $LIBS $XML_LIBS $XSLT_LIBS -lexslt"
CXXFLAGS="$DEBUG $CXXFLAGS $XML_CXXFLAGS $XSLT_CXXFLAGS"
CPPFLAGS="$DEBUG $CPPFLAGS $XML_CPPFLAGS $XSLT_CPPFLAGS -Wno-write-strings"
CFLAGS="$DEBUG $CFLAGS $XML_CFLAGS $XSLT_CFLAGS - -Wno-write-strings"

AC_OUTPUT(  \
    Makefile \
    src/Makefile    \
    libxmldiff.pc \
	doc/libxmldiff.dox \
)
