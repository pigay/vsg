#!/usr/bin/env python

# borrowed and adapted from pygtk docgen.py

import sys, getopt

import docgen



class VsgDocbookDocWriter(docgen.DocbookDocWriter):
    def write_class_header(self, obj_name):
        self._fp.write('<?xml version="1.0" standalone="no"?>\n')
        self._fp.write('<!DOCTYPE refentry PUBLIC "-//OASIS//DTD DocBook XML V4.1.2//EN"\n')
        self._fp.write('    "http://www.oasis-open.org/docbook/xml/4.1.2/docbookx.dtd">\n')
        self._fp.write('<refentry id="' + self.make_class_ref(obj_name) + '">\n')
        self._fp.write('  <refmeta>\n')
        self._fp.write('    <refentrytitle>%s</refentrytitle>\n'
                 % self.pyname(obj_name))
        self._fp.write('    <manvolnum>3</manvolnum>\n')
        self._fp.write('    <refmiscinfo>PyVsg Docs</refmiscinfo>\n')
        self._fp.write('  </refmeta>\n\n')
        self._fp.write('  <refnamediv>\n')
        self._fp.write('    <refname>%s</refname><refpurpose></refpurpose>\n'
                 % self.pyname(obj_name))
        self._fp.write('  </refnamediv>\n\n')

    def _format_param(self, match):
        return '<parameter>%s</parameter>' % (match.group(1), )

if __name__ == '__main__':
    d = VsgDocbookDocWriter()

    # we parse ".." directory in order to get overriden comments.
    sys.stderr.write ("parsing source docs.\n")
    d.add_sourcedirs(["../../../build/vsg", ".."])
    d.add_tmpldirs(["../../../docs/reference/tmpl"])

    sys.stderr.write ("parsing normal definition docs.\n")
    d.add_docs("../vsg.defs", "../vsg.override", 'vsg')

    sys.stderr.write ("parsing supplemental definition docs.\n")
    d.add_docs("vsgsupvector2d.defs", "", 'vsg')
    d.add_docs("vsgsupvector2f.defs", "", 'vsg')
    d.add_docs("vsgsupvector3d.defs", "", 'vsg')
    d.add_docs("vsgsupvector3f.defs", "", 'vsg')
    d.add_docs("vsgsupquaterniond.defs", "", 'vsg')
    d.add_docs("vsgsupquaternionf.defs", "", 'vsg')
    d.add_docs("vsgsupmatrix3d.defs", "", 'vsg')
    d.add_docs("vsgsupmatrix3f.defs", "", 'vsg')
    d.add_docs("vsgsupmatrix4d.defs", "", 'vsg')
    d.add_docs("vsgsupmatrix4f.defs", "", 'vsg')

    sys.stderr.write ("output xml files.\n")
    d.output_docs("xml/pyvsg")
