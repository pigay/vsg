SUBDIRS = images
EXTRA_DIST = 

if ENABLE_PYGTK

STAMP_FILES = xml_files.stamp html_files.stamp

codegen_dir = `pkg-config pygtk-2.0 --variable=codegendir`

supdefs = vsgsupvector2d.defs vsgsupvector2f.defs vsgsupvector3d.defs \
vsgsupvector3f.defs vsgsupquaterniond.defs vsgsupquaternionf.defs \
vsgsupmatrix3d.defs vsgsupmatrix3f.defs vsgsupmatrix4d.defs vsgsupmatrix4f.defs

EXTRA_DIST += $(supdefs)

if ENABLE_GTK_DOC

xml_files.stamp : $(supdefs)
	test -d $(srcdir)/xml || mkdir $(srcdir)/xml
	env PYTHONPATH=$(codegen_dir) $(PYTHON) vsgdocgen.py 
	touch xml_files.stamp

html_files.stamp : xml_files.stamp pyvsgdocs.xml
	test -d $(srcdir)/html || mkdir $(srcdir)/html
	(cd html && xsltproc --xinclude ../pyvsgdocs.xsl ../pyvsgdocs.xml)
	cp images/*.png html
	touch html_files.stamp

html : html_files.stamp

all : html

pyhtmldir = $(datadir)/doc/pyvsg

install-data-local : html
	$(mkinstalldirs) $(pyhtmldir)
	(installfiles=`echo $(srcdir)/html/*.html`; \
	if test "$$installfiles" = '$(srcdir)/html/*.html'; \
	then echo '-- Nothing to install' ; \
	else \
	  for i in $$installfiles; do \
	    echo '-- Installing '$$i ; \
	    $(INSTALL_DATA) $$i $(pyhtmldir); \
	  done; \
	  navfiles='$(srcdir)/html/*.png' ; \
	  for i in $$navfiles; do \
           echo '-- Installing '$$i ; \
	    $(INSTALL_DATA) $$i $(pyhtmldir); \
	  done; \
	fi)


types_list = $(top_srcdir)/types.list

%d.defs %f.defs : %@t@.defs $(types_list)
	$(PYTHON) $(top_srcdir)/gencode/typegens.py -f=$(types_list) $<

endif # ENABLE_GTK_DOC

endif # ENABLE_PYGTK

noinst_SCRIPTS = vsgdocgen.py pyvsgdocs.xsl devhelp.xsl pyvsgdocs.xml \
vsgsupvector2@t@.defs vsgsupvector3@t@.defs vsgsupquaternion@t@.defs \
vsgsupmatrix3@t@.defs vsgsupmatrix4@t@.defs


EXTRA_DIST += $(noinst_SCRIPTS)

CLEANFILES = $(STAMP_FILES)

clean-local:
	rm -rf html xml

.PHONY = html
