AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(libxmldiff,0.2.4)
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
AC_CHECK_LIB(xslt, main, , AC_MSG_ERROR(libxslt must be installed.))
AC_CHECK_LIB(exslt, main, , AC_MSG_ERROR(libexslt must be installed.))

dnl Exports
DEBUG="-g"
LIBS="$DEBUG $LIBS $XML_LIBS"
CXXFLAGS="$DEBUG $CXXFLAGS $XML_CXXFLAGS"
CPPFLAGS="$DEBUG $CPPFLAGS $XML_CPPFLAGS"
CFLAGS="$DEBUG $CFLAGS $XML_CFLAGS"

AC_OUTPUT(  \
    Makefile \
    src/Makefile    \
)
