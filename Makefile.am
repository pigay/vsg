# require automake-1.7
AUTOMAKE_OPTIONS = 1.7

SUBDIRS = gencode src build docs tests bindings m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = vsg.pc vsgd.pc vsgf.pc

$(pkgconfig_DATA): config.status

EXTRA_DIST = types.list autogen.sh

ACLOCAL_AMFLAGS = -I m4
