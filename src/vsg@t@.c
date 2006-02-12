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

#include "vsg@t@.h"

#include "glib-object.h"

/* declaration of shadow init functions */
void vsg_vector2@t@_init ();

void vsg_vector3@t@_init ();

void vsg_matrix3@t@_init ();

void vsg_quaternion@t@_init ();

void vsg_prtree2@t@_init();

void vsg_prtree3@t@_init();

/**
 * vsg_init_@type@:
 *
 * Call this before you use functions from #Vsg @type@ library.
 */
void vsg_init_@type@ ()
{
  static gboolean wasinit = FALSE;

  if (wasinit) return;

  wasinit = TRUE;

  g_type_init ();

  vsg_vector2@t@_init ();

  vsg_vector3@t@_init ();
 
  vsg_matrix3@t@_init ();

  vsg_quaternion@t@_init ();

  vsg_prtree2@t@_init();

  vsg_prtree3@t@_init();
}
