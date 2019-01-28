bin_PROGRAMS = jfbterm-hue
jfbterm_hue_SOURCES = \
		main.c main.h \
		mytypes.h sequence.h \
		fbcommon.c fbcommon.h \
		fbdpsp.c fbdpsp.h \
		font.c font.h \
		picofont.c picofont.h \
		getcap.c getcap.h \
		message.c message.h \
		pcf.c pcf.h \
		pen.c pen.h \
		term.c term.h \
		util.c util.h \
		csv.c csv.h \
		vterm.c vtermlow.c vterm.h
jfbterm_hue_LDADD = -lutil

JFBTERM_CONFIG_FILE = jfbterm.conf.sample

sysconf_DATA = jfbterm.conf

EXTRA_DIST = $(JFBTERM_CONFIG_FILE) $(sysconf_DATA)

#install-exec-hook:
#	chown root:utmp $(DESTDIR)$(bindir)/jfbterm-hue
#	chmod ug+s $(DESTDIR)$(bindir)/jfbterm-hue

#install-data-local:
#	$(INSTALL_DATA) $(srcdir)/$(JFBTERM_CONFIG_FILE) $(DESTDIR)$(sysconfdir)/$(JFBTERM_CONFIG_FILE)
#	$(INSTALL_DATA) $(srcdir)/fonts/*.pcf.gz $(DESTDIR)$(datadir)/fonts/jfbterm-hue

