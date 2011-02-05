SUBDIRS = . docs examples

SUFFIXES = .defs .override

pkgpythondir = $(pythondir)/vsg
pkgpyexecdir = $(pyexecdir)/vsg

vsg_la_overrides = vsgf.override vsgd.override

EXTRA_DIST = $(vsg_la_overrides) vsg@t@.override vsg@t@.defs \
vsg-types@t@.defs vsgvector2@t@.override vsgvector3@t@.override \
vsgquaternion@t@.override vsgmatrix3@t@.override \
vsgmatrix4@t@.override vsgprtree2@t@.override vsgprtree3@t@.override \
vsgconstants@t@.h vsgconstants@t@.c files_list \
h2def.py __init__.py vsgtypedefs.py vsg.defs

if ENABLE_PYGTK

defsdir = $(pkgdatadir)/$(VSG_MAJOR_VERSION).$(VSG_MINOR_VERSION)/defs
defs_DATA = vsgtypedefs.py

VSG_CFLAGS = -I$(top_srcdir)/build $(MPI_CFLAGS)
VSG_LIBS = $(top_builddir)/build/vsg/libvsg.la $(MPI_LIBS)

INCLUDES = $(PYTHON_INCLUDES) $(GLIB_CFLAGS) @PYGTK_CFLAGS@ $(VSG_CFLAGS)

pkgpyexec_SCRIPTS = __init__.py
pkgpyexec_LTLIBRARIES = vsg.la

common_ldflags = -module -avoid-version


# vsg module
vsg_la_LDFLAGS = $(common_ldflags) -export-symbols-regex initvsg
vsg_la_LIBADD = $(VSG_LIBS)
vsg_la_SOURCES = vsgmodule.c $(BUILT_SOURCES) vsgconstants.h vsgconstants.c \
pyvsg_boxed_pyobject.c pyvsg_boxed_pyobject.h

noinst_HEADERS = pyvsg_boxed_pyobject.h vsgconstants.h

BUILT_SOURCES = vsg.c vsgconstantsf.h vsgconstantsf.c \
vsgconstantsd.h vsgconstantsd.c

types_overrides = vsgvector2d.override vsgvector2f.override \
vsgvector3d.override vsgvector3f.override vsgquaterniond.override \
vsgquaternionf.override vsgmatrix3d.override vsgmatrix3f.override \
vsgmatrix4d.override vsgmatrix4f.override vsgprtree2d.override \
vsgprtree2f.override vsgprtree3d.override vsgprtree3f.override 

types_defs = vsgf.defs vsgd.defs vsg-typesf.defs vsg-typesd.defs

EXTRA_DIST += $(types_overrides) $(types_defs)

vsg.c : vsg.defs vsg.override $(types_overrides) \
vsgf.defs vsgd.defs vsg-typesf.defs vsg-typesd.defs 
	pygtk-codegen-2.0 \
	--load-types vsgtypedefs.py \
	--override vsg.override \
	--prefix pyvsg vsg.defs > gen-vsg.c \
	&& cp gen-vsg.c vsg.c \
	&& rm -f gen-vsg.c

BUILTCLEANFILES = $(types_overrides) \
vsg.override vsgf.override vsgd.override vsgf.defs vsgd.defs \
vsg-typesf.defs vsg-typesd.defs vsg.c vsgconstantsf.h vsgconstantsf.c \
vsgconstantsd.h vsgconstantsd.c

defs_DATA += vsg.defs  $(types_defs)

built-clean :
	\rm $(BUILTCLEANFILES)

MAINTAINERCLEANFILES = $(BUILTCLEANFILES)

vsg.override : $(vsg_la_overrides)
	\rm -f $@ \
	&& cat $(vsg_la_overrides) >> $@


types_list = $(top_srcdir)/types.list

%d.defs %f.defs : %@t@.defs $(types_list)
	$(PYTHON) $(top_srcdir)/gencode/typegens.py -f=$(types_list) $<

%-typesd.defs %-typesf.defs : %-types@t@.defs $(types_list)
	$(PYTHON) $(top_srcdir)/gencode/typegens.py -f=$(types_list) $<

%d.override %f.override : %@t@.override $(types_list)
	$(PYTHON) $(top_srcdir)/gencode/typegens.py -f=$(types_list) $<

%d.c %f.c : %@t@.c $(types_list)
	$(PYTHON) $(top_srcdir)/gencode/typegens.py -f=$(types_list) $<

%d.h %f.h : %@t@.h $(types_list)
	$(PYTHON) $(top_srcdir)/gencode/typegens.py -f=$(types_list) $<

vsgconstantsd.c : | vsgconstantsd.h
vsgconstantsf.c : | vsgconstantsf.h


endif # ENABLE_PYGTK

