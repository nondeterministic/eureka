AUTOMAKE_OPTIONS = dist-zip subdir-objects no-dependencies

# dist-tarZ dist-shar  

SUBDIRS = src data m4

EXTRA_DIST = ltmain.sh

ACLOCAL_AMFLAGS = -I m4

@MAINT@alldist: 
@MAINT@	${MAKE} distcheck
@MAINT@	${MAKE} dist-zip

# @MAINT@	${MAKE} dist-shar
# @MAINT@	${MAKE} dist-tarZ

@MAINT@cvsclean: maintainer-clean
@MAINT@	@-rm -f `find . -name Makefile.in`
@MAINT@	@-rm -f configure aclocal.m4 config.h.in stamp-h.in depcomp ylwrap
@MAINT@	@-rm -f config.guess config.sub config.cache config.log config.status
@MAINT@	@-rm -f mkinstalldirs missing install-sh ltmain.sh
