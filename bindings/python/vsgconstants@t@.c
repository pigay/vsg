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

#include "vsgconstants@t@.h"

#include <vsg/vsgvector2@t@.h>
#include <vsg/vsgvector3@t@.h>

#include <vsg/vsgmatrix3@t@.h>
#include <vsg/vsgmatrix4@t@.h>

#include <vsg/vsgquaternion@t@.h>

void vsg_constants@t@_register (PyObject *m)
{
  /*
   * here we use *_clone method and not pyg_boxed_new copy argument
   * to avoid warnings on const qualifier.
   */

  /* vector2@t@ */

  PyModule_AddObject (m, "V2@T@_ZERO",
                      pyg_boxed_new (VSG_TYPE_VECTOR2@T@,
                                     vsg_vector2@t@_clone (&VSG_V2@T@_ZERO),
                                     FALSE, TRUE));
  PyModule_AddObject (m, "V2@T@_I",
                      pyg_boxed_new (VSG_TYPE_VECTOR2@T@,
                                     vsg_vector2@t@_clone (&VSG_V2@T@_I),
                                     FALSE, TRUE));
  PyModule_AddObject (m, "V2@T@_J",
                      pyg_boxed_new (VSG_TYPE_VECTOR2@T@,
                                     vsg_vector2@t@_clone (&VSG_V2@T@_J),
                                     FALSE, TRUE));

  /* vector3@t@ */

  PyModule_AddObject (m, "V3@T@_ZERO",
                      pyg_boxed_new (VSG_TYPE_VECTOR3@T@,
                                     vsg_vector3@t@_clone (&VSG_V3@T@_ZERO),
                                     FALSE, TRUE));
  PyModule_AddObject (m, "V3@T@_I",
                      pyg_boxed_new (VSG_TYPE_VECTOR3@T@,
                                     vsg_vector3@t@_clone (&VSG_V3@T@_I),
                                     FALSE, TRUE));
  PyModule_AddObject (m, "V3@T@_J",
                      pyg_boxed_new (VSG_TYPE_VECTOR3@T@,
                                     vsg_vector3@t@_clone (&VSG_V3@T@_J),
                                     FALSE, TRUE));
  PyModule_AddObject (m, "V3@T@_K",
                      pyg_boxed_new (VSG_TYPE_VECTOR3@T@,
                                     vsg_vector3@t@_clone (&VSG_V3@T@_K),
                                     FALSE, TRUE));

  /* matrix3@t@ */

  PyModule_AddObject (m, "M3@T@_ZERO",
                      pyg_boxed_new (VSG_TYPE_MATRIX3@T@,
                                     vsg_matrix3@t@_clone (&VSG_M3@T@_ZERO),
                                     FALSE, TRUE));
  PyModule_AddObject (m, "M3@T@_ID",
                      pyg_boxed_new (VSG_TYPE_MATRIX3@T@,
                                     vsg_matrix3@t@_clone (&VSG_M3@T@_ID),
                                     FALSE, TRUE));

  /* matrix4@t@ */

  PyModule_AddObject (m, "M4@T@_ZERO",
                      pyg_boxed_new (VSG_TYPE_MATRIX4@T@,
                                     vsg_matrix4@t@_clone (&VSG_M4@T@_ZERO),
                                     FALSE, TRUE));
  PyModule_AddObject (m, "M4@T@_ID",
                      pyg_boxed_new (VSG_TYPE_MATRIX4@T@,
                                     vsg_matrix4@t@_clone (&VSG_M4@T@_ID),
                                     FALSE, TRUE));
  /* quaternion@t@ */

  PyModule_AddObject (m, "Q@T@_ID",
                      pyg_boxed_new (VSG_TYPE_QUATERNION@T@,
                                     vsg_quaternion@t@_clone (&VSG_Q@T@_ID),
                                     FALSE, TRUE));
}
