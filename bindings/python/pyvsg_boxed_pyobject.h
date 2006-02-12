/* LIBVSG - Visaurin Geometric Library
 * Copyright (C) 2006 Pierre Gay
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <Python.h>
#include <glib-object.h>

#ifndef __PYVSG_BOXED_PYOBJECT_H__
#define __PYVSG_BOXED_PYOBJECT_H__

/*
 * This is a utility enabling to treat Pyobjects as GBoxed. The only requirment
 * on the considered object is to have a "clone" method returning a copy.
 */

#define PYVSG_TYPE_BOXED_PYOBJECT (pyvsg_boxed_pyobject_get_type ())

GType pyvsg_boxed_pyobject_get_type (void);

void pyvsg_boxed_pyobject_free (PyObject *obj);

PyObject *pyvsg_boxed_pyobject_clone (PyObject *obj);

gboolean pyvsg_boxed_pyobject_check_method (PyObject *obj);

#endif /* __PYVSG_BOXED_PYOBJECT_H__ */
