AUTOMAKE_OPTIONS = dist-bzip2 dist-zip
SUBDIRS = m4 intl po doc src tests
docdir = $(prefix)/share/doc/$(PACKAGE)
EXTRA_DIST = config.rpath mkinstalldirs  ABOUT-NLS BUGS CREDITS HISTORY LEAME depcomp
doc_DATA = ABOUT-NLS BUGS COPYING CREDITS HISTORY INSTALL LEAME README TODO
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
config.sub configure depcomp install-sh missing mkinstalldirs steghide.doxygen steghide.spec

ACLOCAL_AMFLAGS = -I m4

uninstall-local:
	rmdir $(docdir)
