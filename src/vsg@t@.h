/* LIBVSG - Visaurin Geometric Library
 * Copyright (C) 2006-2008 Pierre Gay
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


#ifndef __VSG@T@_H__ /* __VSG@T@_H__ */
#define __VSG@T@_H__

#include <glib.h>

#include <vsg/vsgmpi.h>

#include <vsg/vsgvector2@t@.h>
#include <vsg/vsgvector3@t@.h>

#include <vsg/vsgmatrix3@t@.h>
#include <vsg/vsgmatrix4@t@.h>

#include <vsg/vsgquaternion@t@.h>

#include <vsg/vsgprtree2-common.h>
#include <vsg/vsgprtree2@t@.h>
#include <vsg/vsgprtree2@t@-extras.h>

#include <vsg/vsgprtree3-common.h>
#include <vsg/vsgprtree3@t@.h>
#include <vsg/vsgprtree3@t@-extras.h>

#ifdef VSG_HAVE_MPI
#include <vsg/vsgprtree2@t@-parallel.h>
#include <vsg/vsgprtree3@t@-parallel.h>
#endif

G_BEGIN_DECLS;

void vsg_init_@type@ ();

G_END_DECLS;

#endif /* __VSG@T@_H__ */
