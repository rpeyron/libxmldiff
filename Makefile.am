SUBDIRS=src

ACLOCAL_AMFLAGS=-I m4

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
		build/vc2017/xmldiff.vcxproj.filters \
		build/vc2017/libxmldiff.vcxproj \
		build/vc2017/xmldiff.sln \
		build/vc2017/xmldiff.vcxproj \
		build/vc2017/libxmldiff.sln \
		build/vc2019/xmldiff.vcxproj.filters \
		build/vc2019/libxmldiff.vcxproj \
		build/vc2019/xmldiff.sln \
		build/vc2019/xmldiff.vcxproj \
		build/vc2019/libxmldiff.sln \
		build/vc2019/clean.bat \
        build/vscode/c_cpp_properties.json \
        build/vscode/launch.json \
        build/vscode/tasks.json \
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
        tests/bugs/command.lst \
        src/libxmldiff.pc \
        src/libxmldiff.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libxmldiff.pc

old-deb:
	ln -sf build/debian .
	chmod +x debian/rules
	dpkg-buildpackage -rfakeroot
	fakeroot debian/rules clean

    
DEBSOURCEPKG=$(PACKAGE)_$(VERSION).orig.tar.gz
DEBSOURCEDIR=$(PACKAGE)-$(VERSION)
deb: dist
	ln -sf build/debian .
	scite debian/control
	scite debian/changelog
	@echo "Building Debian package..."; \
	if test ! -d debian; then \
		echo "The files needed for building a Debian package are not" \
		"included by default in the distribution. To build a package, check" \
		"out the project from source control."; \
		exit 1; \
	fi; \
	mv $(PACKAGE)-$(VERSION).tar.gz $(DEBSOURCEPKG) && \
	tar --extract --gunzip --file $(DEBSOURCEPKG) && \
	mkdir -p $(DEBSOURCEDIR)/debian && \
	cp \
		debian/* \
		$(DEBSOURCEDIR)/debian && \
	cd $(DEBSOURCEDIR) && \
	debuild --no-tgz-check -rfakeroot -G -D ; \
	cd .. ; \
	#rm --force $(DEBSOURCEPKG); \
	rm --recursive --force $(DEBSOURCEDIR)
	aptly -force-replace=true repo add local $(PACKAGE)_$(VERSION)*.dsc $(PACKAGE)_$(VERSION)*.deb
	@echo "To publish :"
	@echo aptly -gpg-key="remi+debian@via.ecp.fr" -force-overwrite publish update stable filesystem:web:debian


dox:
	doxygen doc/libxmldiff.dox

test: tests-run

tests-clean:
	cd tests/ && bash tests.sh clean

tests-exp:
	cd tests/ && bash tests.sh exp

tests-run:
	cd tests/ && bash tests.sh diff dev

tests-prev:
	cd tests/ && bash tests.sh diff prev

changelog:
	mv -f ChangeLog ChangeLog.bak
	cvs2cl
	mv -f ChangeLog ChangeLog.cvs
	mv -f ChangeLog.bak ChangeLog

clean-local: tests-clean code-coverage-clean

ppa:
	scite debian/control
	scite debian/changelog
	debuild -S -sa
	dput ppa ../*.changes

code-coverage-clean:
	-$(LCOV) --directory $(top_builddir) -z
	-find . \( -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" \) -delete
	-rm -rf "gcov-rapport" "gcov-rapport.info"

gcov-view:
	lcov --directory src -c -o gcov-rapport.info
	genhtml -o gcov-rapport -t "couverture de code des tests" gcov-rapport.info
	sensible-browser gcov-rapport/index.html