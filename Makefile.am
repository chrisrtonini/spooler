## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

spoolerdocdir = ${prefix}/doc/spooler
spoolerdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(spoolerdoc_DATA) \
	spooler/spooler.cfg\
	ecf_spooler       \
	spooler_setup.sh       \
	setup_totcnf.pl       \
	setup_fpgto.pl

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(spoolerdocdir)
