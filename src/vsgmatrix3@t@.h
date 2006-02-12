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

#ifndef __VSG_MATRIX3@T@_H__        /* __VSG_MATRIX3@T@_H__ */
#define __VSG_MATRIX3@T@_H__

#include <vsg/vsgvector2@t@.h>
#include <vsg/vsgvector3@t@.h>


#include <glib.h>
#include <glib-object.h>

#include <stdio.h>

G_BEGIN_DECLS;
/* macros */
#define VSG_TYPE_MATRIX3@T@ (vsg_matrix3@t@_get_type ())

/* forward declarations */
typedef struct _VsgMatrix3@t@ VsgMatrix3@t@;

/* structure definition */
struct _VsgMatrix3@t@
{

  @type@ components[9];

};

/* constants */

static const VsgMatrix3@t@ VSG_M3@T@_ZERO = {
  {
    0., 0., 0.,
    0., 0., 0.,
    0., 0., 0.,
  }
};

static const VsgMatrix3@t@ VSG_M3@T@_ID = {
  {
    1., 0., 0.,
    0., 1., 0.,
    0., 0., 1.,
  }
};

/* functions */
GType vsg_matrix3@t@_get_type () G_GNUC_CONST;

VsgMatrix3@t@ *vsg_matrix3@t@_new (@type@ a00, @type@ a01, @type@ a02,
                                   @type@ a10, @type@ a11, @type@ a12,
                                   @type@ a20, @type@ a21, @type@ a22);

void vsg_matrix3@t@_free (VsgMatrix3@t@ *mat);

VsgMatrix3@t@ *vsg_matrix3@t@_identity_new ();


VsgMatrix3@t@ *vsg_matrix3@t@_rotate_new (@type@ a);

VsgMatrix3@t@ *vsg_matrix3@t@_translate_new (@type@ x,@type@ y);

VsgMatrix3@t@ *vsg_matrix3@t@_scale_new (@type@ x,@type@ y);

void vsg_matrix3@t@_set (VsgMatrix3@t@ *mat,
                         @type@ a00,@type@ a01,@type@ a02,
                         @type@ a10,@type@ a11,@type@ a12,
                         @type@ a20,@type@ a21,@type@ a22);

void vsg_matrix3@t@_identity (VsgMatrix3@t@ *mat);

void vsg_matrix3@t@_copy (const VsgMatrix3@t@ *src,
                          VsgMatrix3@t@ *dst);

VsgMatrix3@t@ *vsg_matrix3@t@_clone (const VsgMatrix3@t@ *src);

void vsg_matrix3@t@_rotate (VsgMatrix3@t@ *mat,@type@ a);

void vsg_matrix3@t@_translate (VsgMatrix3@t@ *mat, @type@ x, @type@ y);

void vsg_matrix3@t@_scale (VsgMatrix3@t@ *mat, @type@ x, @type@ y);

void vsg_matrix3@t@_add (const VsgMatrix3@t@ *mat,
                         const VsgMatrix3@t@ *other,
                         VsgMatrix3@t@ *result);

void vsg_matrix3@t@_sub (const VsgMatrix3@t@ *mat,
                         const VsgMatrix3@t@ *other,
                         VsgMatrix3@t@ *result);

void vsg_matrix3@t@_matmult (const VsgMatrix3@t@ *mat,
                             const VsgMatrix3@t@ *other,
                             VsgMatrix3@t@ *result);

void vsg_matrix3@t@_vecmult (const VsgMatrix3@t@ *mat,
                             const VsgVector2@t@ *vec,
                             VsgVector2@t@ *result);

void vsg_matrix3@t@_vec3mult (const VsgMatrix3@t@ *mat,
                              const VsgVector3@t@ *vec,
                              VsgVector3@t@ *result);

void vsg_matrix3@t@_vec3mult_T (const VsgMatrix3@t@ *mat,
                                const VsgVector3@t@ *vec,
                                VsgVector3@t@ *result);

@type@ vsg_matrix3@t@_det (const VsgMatrix3@t@ *mat);

gboolean vsg_matrix3@t@_invert (const VsgMatrix3@t@ *mat,
                                VsgMatrix3@t@ *result);

@type@ vsg_matrix3@t@_component (const VsgMatrix3@t@ *mat,
                                 guint i, guint j);

void vsg_matrix3@t@_transpose (const VsgMatrix3@t@ *mat,
                               VsgMatrix3@t@ *result);

void vsg_matrix3@t@_write (const VsgMatrix3@t@ *mat,
                           FILE * file);

void vsg_matrix3@t@_print (const VsgMatrix3@t@ *mat);

G_END_DECLS;

#endif /* __VSG_MATRIX3@T@_H__ */
