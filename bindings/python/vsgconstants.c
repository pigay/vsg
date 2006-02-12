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

#include "vsgconstants.h"

#include "vsg/vsgprtree2-common.h"
#include "vsg/vsgprtree3-common.h"

void vsg_constants_register (PyObject *m)
{
  /* prtree2 */

  PyModule_AddObject (m, "LOC2_SW", Py_BuildValue ("i", VSG_LOC2_SW));
  PyModule_AddObject (m, "LOC2_SE", Py_BuildValue ("i", VSG_LOC2_SE));
  PyModule_AddObject (m, "LOC2_NW", Py_BuildValue ("i", VSG_LOC2_NW));
  PyModule_AddObject (m, "LOC2_NE", Py_BuildValue ("i", VSG_LOC2_NE));

  PyModule_AddObject (m, "LOC2_MASK", Py_BuildValue ("i", VSG_LOC2_MASK));
  PyModule_AddObject (m, "LOC2_X", Py_BuildValue ("i", VSG_LOC2_X));
  PyModule_AddObject (m, "LOC2_Y", Py_BuildValue ("i", VSG_LOC2_Y));

  PyModule_AddObject (m, "RLOC2_MASK", Py_BuildValue ("i", VSG_RLOC2_MASK));

  /* prtree3 */

  PyModule_AddObject (m, "LOC3_BSW", Py_BuildValue ("i", VSG_LOC3_BSW));
  PyModule_AddObject (m, "LOC3_BSE", Py_BuildValue ("i", VSG_LOC3_BSE));
  PyModule_AddObject (m, "LOC3_BNW", Py_BuildValue ("i", VSG_LOC3_BNW));
  PyModule_AddObject (m, "LOC3_BNE", Py_BuildValue ("i", VSG_LOC3_BNE));
  PyModule_AddObject (m, "LOC3_USW", Py_BuildValue ("i", VSG_LOC3_USW));
  PyModule_AddObject (m, "LOC3_USE", Py_BuildValue ("i", VSG_LOC3_USE));
  PyModule_AddObject (m, "LOC3_UNW", Py_BuildValue ("i", VSG_LOC3_UNW));
  PyModule_AddObject (m, "LOC3_UNE", Py_BuildValue ("i", VSG_LOC3_UNE));
  PyModule_AddObject (m, "LOC3_MASK", Py_BuildValue ("i", VSG_LOC3_MASK));
  PyModule_AddObject (m, "LOC3_X", Py_BuildValue ("i", VSG_LOC3_X));
  PyModule_AddObject (m, "LOC3_Y", Py_BuildValue ("i", VSG_LOC3_Y));
  PyModule_AddObject (m, "LOC3_Z", Py_BuildValue ("i", VSG_LOC3_Z));
  PyModule_AddObject (m, "RLOC3_MASK", Py_BuildValue ("i", VSG_RLOC3_MASK));

  /* GLib missing enum */
  PyModule_AddObject (m, "G_IN_ORDER", Py_BuildValue ("i", G_IN_ORDER));
  PyModule_AddObject (m, "G_PRE_ORDER", Py_BuildValue ("i", G_PRE_ORDER));
  PyModule_AddObject (m, "G_POST_ORDER", Py_BuildValue ("i", G_POST_ORDER));
  PyModule_AddObject (m, "G_LEVEL_ORDER", Py_BuildValue ("i", G_LEVEL_ORDER));

}
