bin_PROGRAMS=xmldiff
lib_LIBRARIES=libxmldiff.a
libxmldiff_a_SOURCES = \
    errors.cpp \
    lx2_diff.cpp \
    lx2_ops.cpp \
    lx2_parse.cpp \
    lx2_util.cpp

libxmldiff_includedir=$(includedir)/libxmldiff
libxmldiff_include_HEADERS= \
    errors.h \
    lx2_diff.h \
    lx2_ops.h \
    lx2_parse.h \
    lx2_str.h \
    lx2_util.h \
    libxmldiff.h

xmldiff_SOURCES = \
	xmldiff.h \
	xmldiff.cpp

xmldiff_LDADD = libxmldiff.a
