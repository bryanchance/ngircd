#
# ngIRCd -- The Next Generation IRC Daemon
# Copyright (c)2001-2012 Alexander Barton (alex@barton.de) and Contributors
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# Please read the file COPYING, README and AUTHORS for more information.
#

.tmpl:
	sed \
	    -e s@:ETCDIR:@${sysconfdir}@ \
	    <$< >$@

SUFFIXES = .tmpl

static_docs = \
	Bopm.txt \
	Capabilities.txt \
	FAQ.txt \
	GIT.txt \
	HowToRelease.txt \
	Modes.txt \
	PAM.txt \
	Platforms.txt \
	Protocol.txt \
	README-AUX.txt \
	README-BeOS.txt \
	README-Interix.txt \
	RFC.txt \
	Services.txt \
	SSL.txt

doc_templates = sample-ngircd.conf.tmpl

generated_docs = sample-ngircd.conf

toplevel_docs = ../AUTHORS ../COPYING ../ChangeLog ../INSTALL ../NEWS ../README

SUBDIRS = src

EXTRA_DIST = $(static_docs) $(doc_templates)

CLEANFILES = $(generated_docs)

maintainer-clean-local:
	rm -f Makefile Makefile.in

all: $(generated_docs)

install-data-hook: $(static_docs) $(toplevel_docs) $(generated_docs)
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)
	if [ ! -f $(DESTDIR)$(sysconfdir)/ngircd.conf ]; then \
	  $(INSTALL) -m 600 -c sample-ngircd.conf $(DESTDIR)$(sysconfdir)/ngircd.conf; \
	 fi
	$(mkinstalldirs) $(DESTDIR)$(docdir)
	for f in $(static_docs) $(toplevel_docs); do \
	  $(INSTALL) -m 644 -c $(srcdir)/$$f $(DESTDIR)$(docdir)/; \
	 done
	for f in $(generated_docs); do \
	  $(INSTALL) -m 644 -c $$f $(DESTDIR)$(docdir)/; \
	 done

uninstall-hook:
	rm -rf $(DESTDIR)$(docdir)

srcdoc:
	make -C src srcdoc

# -eof-
