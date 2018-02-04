AUTOMAKE_OPTIONS = dist-zip subdir-objects no-dependencies

SUBDIRS = src data m4

EXTRA_DIST = ltmain.sh README.md

ACLOCAL_AMFLAGS = -I m4

@MAINT@doxygen:
@MAINT@	cd $(top_srcdir)/src ;\
@MAINT@	${MAKE} doxygen

@MAINT@alldist: doxygen
@MAINT@	${MAKE} distcheck
@MAINT@	${MAKE} dist-zip

@MAINT@cvsclean: maintainer-clean
@MAINT@	@-rm -f `find . -name Makefile.in`
@MAINT@	@-rm -f configure aclocal.m4 config.h.in stamp-h.in depcomp ylwrap
@MAINT@	@-rm -f config.guess config.sub config.cache config.log config.status
@MAINT@	@-rm -f mkinstalldirs missing install-sh ltmain.sh html
