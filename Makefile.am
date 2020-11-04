AUTOMAKE_OPTIONS = dist-bzip2 dist-zip
SUBDIRS = m4 src tests/steghide
docdir = $(prefix)/share/doc/$(PACKAGE)
EXTRA_DIST = config.rpath mkinstalldirs BUGS CREDITS HISTORY depcomp
doc_DATA = BUGS COPYING CREDITS HISTORY README.md
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
config.sub configure depcomp install-sh missing mkinstalldirs steghide.doxygen steghide.spec

ACLOCAL_AMFLAGS = -I m4

uninstall-local:
	rmdir $(docdir)
