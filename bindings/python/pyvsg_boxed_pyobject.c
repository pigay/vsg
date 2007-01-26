/* LIBVSG - Visaurin Geometric Library
 * Copyright (C) 2006-2007 Pierre Gay
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

#include "pyvsg_boxed_pyobject.h"

GType pyvsg_boxed_pyobject_get_type (void)
{
  static GType pyvsg_boxed_pyobject_type = 0;

  if (!pyvsg_boxed_pyobject_type)
    {
      pyvsg_boxed_pyobject_type =
        g_boxed_type_register_static ("PyVsgBoxedPyObject",
                                      (GBoxedCopyFunc) pyvsg_boxed_pyobject_clone,
                                      (GBoxedFreeFunc) pyvsg_boxed_pyobject_free);

    }

  return pyvsg_boxed_pyobject_type;

}

void pyvsg_boxed_pyobject_free (PyObject *obj)
{
  Py_DECREF (obj);
}

PyObject *pyvsg_boxed_pyobject_clone (PyObject *obj)
{
  PyObject *result = PyObject_CallMethod (obj, "copy", NULL);
  return result;
}

gboolean pyvsg_boxed_pyobject_check_method (PyObject *obj)
{
  PyObject *clone = PyObject_GetAttrString (obj, "copy");

  if (clone == NULL) return FALSE;

  return (gboolean) PyMethod_Check (clone);
}

