dnl Process this file with autoconf to produce a configure script.

# glib style versioning
#
# Making releases:
#   vsg_micro_version += 1;
#   vsg_interface_age += 1;
#   vsg_binary_age += 1;
# if any functions have been added, set vsg_interface_age to 0.
# if backwards compatibility has been broken,
# set vsg_binary_age _and_ vsg_interface_age to 0.
#
m4_define([vsg_major_version], [0])
m4_define([vsg_minor_version], [1])
m4_define([vsg_micro_version], [0])
m4_define([vsg_interface_age], [0])
m4_define([vsg_binary_age],
          [m4_eval(100 * vsg_minor_version + vsg_micro_version)])
m4_define([vsg_version],
          [vsg_major_version.vsg_minor_version.vsg_micro_version])

# libtool version related macros
m4_define([vsg_lt_release], [vsg_major_version.vsg_minor_version])
m4_define([vsg_lt_current],
          [m4_eval(100 * vsg_minor_version + vsg_micro_version - vsg_interface_age)])
m4_define([vsg_lt_revision], [vsg_interface_age])
m4_define([vsg_lt_age], [m4_eval(vsg_binary_age - vsg_interface_age)])
m4_define([vsg_lt_current_minus_age],
          [m4_eval(vsg_lt_current - vsg_lt_age)])


AC_INIT(vsg, [vsg_version])

AC_CONFIG_SRCDIR([src/vsg.c])

VSG_MAJOR_VERSION=vsg_major_version
VSG_MINOR_VERSION=vsg_minor_version
VSG_MICRO_VERSION=vsg_micro_version
VSG_INTERFACE_AGE=vsg_interface_age
VSG_BINARY_AGE=vsg_binary_age
VSG_VERSION=vsg_version

AC_SUBST(VSG_MAJOR_VERSION)
AC_SUBST(VSG_MINOR_VERSION)
AC_SUBST(VSG_MICRO_VERSION)
AC_SUBST(VSG_VERSION)
AC_SUBST(VSG_INTERFACE_AGE)
AC_SUBST(VSG_BINARY_AGE)

AC_DEFINE(VSG_MAJOR_VERSION, [vsg_major_version],
	  [Define to the VSG major version])
AC_DEFINE(VSG_MINOR_VERSION, [vsg_minor_version],
	  [Define to the VSG minor version])
AC_DEFINE(VSG_MICRO_VERSION, [vsg_micro_version],
	  [Define to the VSG micro version])
AC_DEFINE(VSG_INTERFACE_AGE, [vsg_interface_age],
	  [Define to the VSG interface age])
AC_DEFINE(VSG_BINARY_AGE, [vsg_binary_age],
	  [Define to the VSG binary age])

# libtool versioning
LT_RELEASE=vsg_lt_release
LT_CURRENT=vsg_lt_current
LT_REVISION=vsg_lt_revision
LT_AGE=vsg_lt_age
LT_CURRENT_MINUS_AGE=vsg_lt_current_minus_age
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)
AC_SUBST(LT_CURRENT_MINUS_AGE)

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(vsg, vsg_version, [no-define])

AM_CONFIG_HEADER(vsg-config.h)

AC_ARG_ENABLE(debug,
              [AC_HELP_STRING([--enable-debug], [turns on debugging])],
              [enable_debug=yes])

if test "x$enable_debug" = "xyes"; then
  case " $CFLAGS " in
    *[[\ \	]]-g[[\ \	]]*) ;;
    *) CFLAGS="$CFLAGS -g" ;;
  esac
fi

AC_ARG_ENABLE(check-params,
              [AC_HELP_STRING([--enable-check-params],
              [turns on function parameters checking])],
              [AC_DEFINE(VSG_CHECK_PARAMS, 1, [function parameters checking])])


dnl Checks for programs.
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_ISC_POSIX

AM_PROG_LIBTOOL

AM_MISSING_PROG([AUTOM4TE], [autom4te])

AM_PATH_PYTHON(2.2)

VL_PROG_CC_WARNINGS

dnl language choice
AC_LANG_C


dnl Checks for libraries.
AC_CHECK_LIB(m, cos)



PKG_CHECK_MODULES(BASE_DEPENDENCIES, [glib-2.0 >= 2.0.0 gobject-2.0 >= 2.0.0])

CFLAGS="$BASE_DEPENDENCIES_CFLAGS $CFLAGS"
LIBS="$BASE_DEPENDENCIES_LIBS $LIBS "
AC_SUBST(CFLAGS)
AC_SUBST(LIBS)

##################################################
# Check for gtk-doc.
##################################################

AC_ARG_WITH(html-dir, [  --with-html-dir=PATH path to installed docs ])

if test "x$with_html_dir" = "x" ; then
  HTML_DIR='${datadir}/gtk-doc/html'
else
  HTML_DIR=$with_html_dir
fi

AC_SUBST(HTML_DIR)

AC_CHECK_PROG(GTKDOC, gtkdoc-mkdb, true, false)

gtk_doc_min_version=0.6
if $GTKDOC ; then 
    gtk_doc_version=`gtkdoc-mkdb --version`
    AC_MSG_CHECKING([gtk-doc version ($gtk_doc_version) >= $gtk_doc_min_version])
    if perl <<EOF ; then
      exit (("$gtk_doc_version" =~ /^[[0-9]]+\.[[0-9]]+$/) &&
            ("$gtk_doc_version" >= "$gtk_doc_min_version") ? 0 : 1);
EOF
      AC_MSG_RESULT(yes)
   else
      AC_MSG_RESULT(no)
      GTKDOC=false
   fi
fi

dnl Let people disable the gtk-doc stuff.
AC_ARG_ENABLE(gtk-doc, [  --enable-gtk-doc  Use gtk-doc to build documentation [default=auto]], enable_gtk_doc="$enableval", enable_gtk_doc=auto)

if test x$enable_gtk_doc = xauto ; then
  if test x$GTKDOC = xtrue ; then
    enable_gtk_doc=yes
  else
    enable_gtk_doc=no 
  fi
fi

AM_CONDITIONAL(ENABLE_GTK_DOC, test x$enable_gtk_doc = xyes)

##################################################
# Check for pygtk.
##################################################

PKG_CHECK_MODULES(PYGTK, pygtk-2.0 ,
	 [have_pygtk="yes"],
	 [have_pygtk="no"])

if test x$have_pygtk = xyes ; then

  AC_CHECK_PROG(PYGTK, pygtk-codegen-2.0, true, false)

dnl Let people disable python bindings.
  AC_ARG_ENABLE(pygtk, [  --enable-pygtk  Use pygtk to build Python bindings [default=auto]], enable_pygtk="$enableval", enable_pygtk=auto)

  if test x$enable_pygtk = xauto ; then
    if test x$PYGTK = xtrue ; then
      enable_pygtk=yes
    else
      enable_pygtk=no 
    fi
  fi
else
  enable_pygtk=no
fi

if test x$enable_pygtk = xyes ; then
  AM_CHECK_PYTHON_HEADERS(,[AC_MSG_ERROR(could not find Python headers)])
fi

AM_CONDITIONAL(ENABLE_PYGTK, test x$enable_pygtk = xyes)
AC_SUBST(PYGTK_CFLAGS)
AC_SUBST(PYGTK_LIBS)

dnl Checks for header files.
AC_HEADER_STDC
dnl 


dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_INLINE

dnl Checks for library functions.


# Testsuite configuration.
AC_CONFIG_TESTDIR(tests)


# Generate files.
AC_OUTPUT([
vsgd.pc
vsgf.pc
vsg.pc
Makefile
src/Makefile
build/Makefile
build/vsg/Makefile
gencode/Makefile
docs/Makefile
docs/reference/Makefile
docs/reference/version.xml
tests/Makefile
tests/atlocal
bindings/Makefile
bindings/python/Makefile
bindings/python/examples/Makefile
bindings/python/docs/Makefile
bindings/python/docs/images/Makefile
m4/Makefile
] )
