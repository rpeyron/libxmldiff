SUBDIRS=src

EXTRA_DIST = \
        README \
        COPYING \
        AUTHORS \
        ChangeLog \
        INSTALL \
        NEWS \
        TODO \
        build/dev-cpp \
        build/dev-cpp/License.txt \
        build/dev-cpp/Makefile.win \
        build/dev-cpp/Readme.txt \
        build/dev-cpp/libxmldiff-dev.DevPackage \
        build/dev-cpp/libxmldiff.dev \
        build/dev-cpp/xmldiff.dev \
        build/dev-cpp/xmldiff.layout \
        build/vc/libxmldiff.dsp \
        build/vc/libxmldiff.dsw \
        build/vc/xmldiff.dsp \
        build/win32/package-libxmldiff-bin-devcpp.bat \
        build/win32/package-libxmldiff-bin-msvc.bat \
        build/win32/package-libxmldiff-dev-devcpp.bat \
        build/win32/package-libxmldiff-dev-msvc.bat \
        build/win32/package-libxmldiff-src.bat \
        build/win32/package-win32.bat \
        build/win32/package-xmldiff-bin.bat \
        build/debian/control \
        build/debian/changelog \
        build/debian/conffiles \
        build/debian/copyright \
        build/debian/menu \
        build/debian/postinst \
        build/debian/postrm \
        build/debian/rules \
        build/debian/libxmldiff.1 \
		bin/xmldiff.gui \
		doc/libxmldiff.dox

deb:
	ln -sf build/debian .
	chmod +x debian/rules
	dpkg-buildpackage -rfakeroot
	fakeroot debian/rules clean

dox:
	doxygen doc/libxmldiff.dox
