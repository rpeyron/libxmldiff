SUBDIRS=src

EXTRA_DIST = \
        README \
        COPYING \
        AUTHORS \
        ChangeLog \
        INSTALL \
        NEWS \
        TODO \
		bootstrap \
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
		doc/libxmldiff.dox \
		tests/tests.sh \
        tests/basics/after.xml \
        tests/basics/before.xml \
        tests/basics/command.lst \
        tests/basics/toto.xml \
        tests/bug-emptyprefix/after.xml \
        tests/bug-emptyprefix/before.xml \
        tests/bug-emptyprefix/command.lst \
        tests/similar/after.xml \
        tests/similar/before.xml \
        tests/similar/command.lst \
        tests/scripts/command.lst \
        tests/scripts/script.xds \
        tests/scripts/message.xsl \
        tests/scripts/message.xds \
        tests/xslt/omit-declaration.xsl \
        tests/xslt/command.lst \
        tests/xslt/file.xml \
        tests/bugs/product1.xml \
        tests/bugs/product2.xml \
        tests/bugs/command.lst 

deb:
	ln -sf build/debian .
	chmod +x debian/rules
	dpkg-buildpackage -rfakeroot
	fakeroot debian/rules clean

dox:
	doxygen doc/libxmldiff.dox

tests-exp:
	cd tests/ && sh tests.sh exp

tests-run:
	cd tests/ && sh tests.sh diff dev

tests-prev:
	cd tests/ && sh tests.sh diff prev

changelog:
	mv -f ChangeLog ChangeLog.bak
	cvs2cl
	mv -f ChangeLog ChangeLog.cvs
	mv -f ChangeLog.bak ChangeLog

