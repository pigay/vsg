
include $(top_srcdir)/src/sources.list

doc:

AM_CFLAGS = @CFLAGS@ -DG_LOG_DOMAIN=\"Vsg\" -I$(top_srcdir)/build

lib_LTLIBRARIES = libvsgd.la libvsgf.la libvsg.la

# gdouble library

libvsgd_la_SOURCES = $(libvsgd_la_built_headers) $(libvsgd_la_built_sources) \
$(libvsgd_built_noinst_headers) $(libvsg_common_headers)

libvsgd_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-export-dynamic

# gfloat library

libvsgf_la_SOURCES = $(libvsgf_la_built_headers) $(libvsgf_la_built_sources) \
$(libvsgf_built_noinst_headers) $(libvsg_common_headers)

libvsgf_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-export-dynamic


# general library

libvsg_la_SOURCES =  $(libvsg_common_headers) $(libvsg_la_sources) \
$(libvsg_la_headers)

libvsg_la_LIBADD = libvsgd.la libvsgf.la

libvsg_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	-export-dynamic

# headers

vsgincludedir = $(includedir)/vsg
vsginclude_HEADERS = $(libvsgd_la_built_headers) $(libvsgf_la_built_headers) \
$(libvsg_la_headers) $(libvsg_common_headers)

noinst_HEADERS = $(libvsgd_built_noinst_headers) \
$(libvsgf_built_noinst_headers)

BUILT_SOURCES = $(libvsgd_la_SOURCES) $(libvsgf_la_SOURCES) \
$(libvsg_la_SOURCES) 

MAINTAINERCLEANFILES = $(BUILT_SOURCES)

types_list = $(top_srcdir)/types.list
typegen = $(top_srcdir)/gencode/typegens.py
tmpldir = $(top_srcdir)/src

%d.h %f.h : $(tmpldir)/%@t@.h $(typegen) $(types_list)
	$(PYTHON) $(typegen) -f=$(types_list) $<

%d.c %f.c : $(tmpldir)/%@t@.c $(typegen) $(types_list)
	$(PYTHON) $(typegen) -f=$(types_list) $<

%d-private.h %f-private.h : $(tmpldir)/%@t@-private.h $(typegen) $(types_list)
	$(PYTHON) $(typegen) -f=$(types_list) $<

%d-extras.h %f-extras.h : $(tmpldir)/%@t@-extras.h $(typegen) $(types_list)
	$(PYTHON) $(typegen) -f=$(types_list) $<

%d-extras.c %f-extras.c : $(tmpldir)/%@t@-extras.c $(typegen) $(types_list)
	$(PYTHON) $(typegen) -f=$(types_list) $<

%.c : $(top_srcdir)/src/%.c
	cp $< $@

%.h : $(top_srcdir)/src/%.h
	cp $< $@

built : $(BUILT_SOURCES)
