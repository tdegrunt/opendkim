# $Id: Makefile.am,v 1.42.2.1 2009/10/28 05:35:41 cm-msk Exp $

# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -pthread --pedantic -Wall -O2
AM_LDFLAGS = -pthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
sbin_PROGRAMS = opendkim-testkey opendkim-testadsp
sbin_SCRIPTS = opendkim-genkey.sh

exampledir = $(datarootdir)/doc/@PACKAGE@
example_DATA = opendkim.conf.sample

if BUILD_FILTER
sbin_PROGRAMS += opendkim
opendkim_SOURCES = opendkim.c opendkim.h opendkim-ar.c opendkim-ar.h opendkim-arf.c opendkim-arf.h opendkim-config.h opendkim-crypto.c opendkim-crypto.h opendkim-db.c opendkim-db.h config.c config.h stats.c stats.h test.c test.h util.c util.h
opendkim_CFLAGS = 
opendkim_CPPFLAGS = -I../libopendkim $(LIBCRYPTO_INCDIRS)
opendkim_LDFLAGS = -L../libopendkim $(LIBCRYPTO_LIBDIRS) $(LIBMILTER_LIBDIRS) $(LIBUNBOUND_LIBDIRS) $(LIBTRE_LIBDIRS)
opendkim_LDADD = ../libopendkim/libopendkim.la -lmilter $(LIBUNBOUND_LIBS) $(LIBTRE_LIBS) $(LIBCRYPTO_LIBS)
if BODYLENGTH_DB
opendkim_CFLAGS += -D_FFR_BODYLENGTH_DB
endif
if CAPTURE_UNKNOWN_ERRORS
opendkim_CFLAGS += -D_FFR_CAPTURE_UNKNOWN_ERRORS
endif
if DEBUG
opendkim_CFLAGS += -g
endif
if DIFFHEADERS
opendkim_CFLAGS += -D_FFR_DIFFHEADERS
opendkim_LDADD += $(LIBTRE_LIBS)
endif
if DKIM_REPUTATION
opendkim_CFLAGS += -D_FFR_DKIM_REPUTATION
endif
if DNS_UPGRADE
opendkim_CFLAGS += -D_FFR_DNS_UPGRADE
endif
if IDENTITY_HEADER
opendkim_CFLAGS += -D_FFR_IDENTITY_HEADER
endif
if MULTIPLE_SIGNATURES
opendkim_CFLAGS += -D_FFR_MULTIPLE_SIGNATURES
endif
if PARSE_TIME
opendkim_CFLAGS += -D_FFR_PARSE_TIME
endif
if POPAUTH
opendkim_CFLAGS += -DPOPAUTH
endif
if QUERY_CACHE
opendkim_CFLAGS += -DQUERY_CACHE
endif
if REDIRECT
opendkim_CFLAGS += -D_FFR_REDIRECT
endif
if REPLACE_RULES
opendkim_CFLAGS += -D_FFR_REPLACE_RULES
endif
if REPORT_INTERVALS
opendkim_CFLAGS += -D_FFR_REPORT_INTERVALS
endif
if SELECTOR_HEADER
opendkim_CFLAGS += -D_FFR_SELECTOR_HEADER
endif
if SENDER_MACRO
opendkim_CFLAGS += -D_FFR_SENDER_MACRO
endif
if STATS
opendkim_CFLAGS += -D_FFR_STATS
endif
if USE_ARLIB
opendkim_CFLAGS += -DUSE_ARLIB
opendkim_LDFLAGS += -L../libar
opendkim_LDADD += ../libar/libar.la
endif
if USE_DB_OPENDKIM
opendkim_CFLAGS += -DUSE_DB
opendkim_CPPFLAGS += $(LIBDB_INCDIRS)
opendkim_LDFLAGS += $(LIBDB_LIBDIRS)
opendkim_LDADD += $(LIBDB_LIBS)
endif
if USE_UNBOUND
opendkim_CFLAGS += -DUSE_UNBOUND
endif
if VBR
opendkim_CFLAGS += -D_FFR_VBR
endif
if VERIFY_DOMAINKEYS
opendkim_CFLAGS += -DVERIFY_DOMAINKEYS
opendkim_CPPFLAGS += $(LIBDK_INCDIRS)
opendkim_LDFLAGS += $(LIBDK_LIBDIRS)
opendkim_LDADD += $(LIBDK_LIBS)
endif
if ZTAGS
opendkim_CFLAGS += -D_FFR_ZTAGS
endif

opendkim_CPPFLAGS += $(LIBMILTER_INCDIRS)
endif

if STATS
sbin_PROGRAMS += opendkim-stats
opendkim_stats_SOURCES = opendkim-db.c opendkim-stats.c
opendkim_stats_CPPFLAGS = -DUSE_DB -I../libopendkim $(LIBCRYPTO_INCDIRS) $(LIBDB_INCDIRS)
opendkim_stats_LDFLAGS = -L../libopendkim $(LIBCRYPTO_LIBDIRS) $(LIBUNBOUND_LIBDIRS) $(LIBDB_LIBDIRS) $(LIBTRE_LIBDIRS)
opendkim_stats_LDADD = ../libopendkim/libopendkim.la $(LIBUNBOUND_LIBS) $(LIBDB_LIBS) $(LIBTRE_LIBS) $(LIBCRYPTO_LIBS)
if USE_ARLIB
opendkim_stats_LDFLAGS += -L../libar
opendkim_stats_LDADD += ../libar/libar.la
endif
if DEBUG
opendkim_stats_CFLAGS = -g
endif
endif

opendkim_testkey_SOURCES = opendkim-testkey.c
opendkim_testkey_CPPFLAGS = -I../libopendkim $(LIBCRYPTO_INCDIRS)
opendkim_testkey_LDFLAGS = -L../libopendkim $(LIBCRYPTO_LIBDIRS)
opendkim_testkey_LDADD = ../libopendkim/libopendkim.la $(LIBCRYPTO_LIBS)
opendkim_testkey_CFLAGS = 
if USE_ARLIB
opendkim_testkey_LDFLAGS += -L../libar
opendkim_testkey_LDADD += ../libar/libar.la
endif
if USE_DB_OPENDKIM
opendkim_testkey_LDFLAGS += $(LIBDB_LIBDIRS)
opendkim_testkey_LDADD += $(LIBDB_LIBS)
endif
if USE_TRE
opendkim_testkey_LDFLAGS += $(LIBTRE_LIBDIRS)
opendkim_testkey_LDADD += $(LIBTRE_LIBS)
endif
if USE_UNBOUND
opendkim_testkey_LDFLAGS += $(LIBUNBOUND_LIBDIRS)
opendkim_testkey_LDADD += $(LIBUNBOUND_LIBS)
endif
if DEBUG
opendkim_testkey_CFLAGS += -g
endif

opendkim_testadsp_SOURCES = opendkim-testadsp.c
opendkim_testadsp_CPPFLAGS = -I../libopendkim $(LIBCRYPTO_INCDIRS)
opendkim_testadsp_LDFLAGS = -L../libopendkim $(LIBCRYPTO_LIBDIRS)
opendkim_testadsp_LDADD = ../libopendkim/libopendkim.la $(LIBCRYPTO_LIBS)
opendkim_testadsp_CFLAGS = 
if USE_ARLIB
opendkim_testadsp_LDFLAGS += -L../libar
opendkim_testadsp_LDADD += ../libar/libar.la
endif
if USE_DB_OPENDKIM
opendkim_testadsp_LDFLAGS += $(LIBDB_LIBDIRS)
opendkim_testadsp_LDADD += $(LIBDB_LIBS)
endif
if USE_TRE
opendkim_testadsp_LDFLAGS += $(LIBTRE_LIBDIRS)
opendkim_testadsp_LDADD += $(LIBTRE_LIBS)
endif
if USE_UNBOUND
opendkim_testadsp_LDFLAGS += $(LIBUNBOUND_LIBDIRS)
opendkim_testadsp_LDADD += $(LIBUNBOUND_LIBS)
endif
if DEBUG
opendkim_testadsp_CFLAGS += -g
endif

man_MANS = opendkim-genkey.8 opendkim-testadsp.8 opendkim-testkey.8
if BUILD_FILTER
man_MANS += opendkim.conf.5 opendkim.8
endif
if STATS
man_MANS += opendkim-stats.8
endif

EXTRA_DIST = $(example_DATA) $(man_MANS) opendkim-stats.8 $(sbin_SCRIPTS)
