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

#ifndef __VSG_MATRIX4@T@_H__ /* __VSG_MATRIX4@T@_H__ */
#define __VSG_MATRIX4@T@_H__

#include <vsg/vsgquaternion@t@.h>

#include <vsg/vsgmpi.h>

#include <glib.h>
#include <glib-object.h>

#include <stdio.h>

G_BEGIN_DECLS;
/* macros */
#define VSG_TYPE_MATRIX4@T@ (vsg_matrix4@t@_get_type ())

#ifdef VSG_HAVE_MPI
#define VSG_MPI_TYPE_MATRIX4@T@ (vsg_matrix4@t@_get_mpi_type ())
#endif

/* forward declarations */
typedef struct _VsgMatrix4@t@ VsgMatrix4@t@;

/* structure definition */
struct _VsgMatrix4@t@ {

  @type@ components[16];

};

/* constants */

static const VsgMatrix4@t@ VSG_M4@T@_ZERO = {
  {
    0., 0., 0., 0.,
    0., 0., 0., 0.,
    0., 0., 0., 0.,
    0., 0., 0., 0.,
  }
};

static const VsgMatrix4@t@ VSG_M4@T@_ID = {
  {
    1., 0., 0., 0.,
    0., 1., 0., 0.,
    0., 0., 1., 0.,
    0., 0., 0., 1.,
  }
};

/* functions */
GType vsg_matrix4@t@_get_type () G_GNUC_CONST;

#ifdef VSG_HAVE_MPI
MPI_Datatype vsg_matrix4@t@_get_mpi_type (void) G_GNUC_CONST;
#endif

VsgMatrix4@t@ *
vsg_matrix4@t@_new (@type@ a00, @type@ a01, @type@ a02, @type@ a03,
                    @type@ a10, @type@ a11, @type@ a12, @type@ a13,
                    @type@ a20, @type@ a21, @type@ a22, @type@ a23,
                    @type@ a30, @type@ a31, @type@ a32, @type@ a33);

void vsg_matrix4@t@_free (VsgMatrix4@t@ *mat);

VsgMatrix4@t@ *vsg_matrix4@t@_identity_new ();


VsgMatrix4@t@ *vsg_matrix4@t@_rotate_cardan_new (@type@ ax,
                                                 @type@ ay,
                                                 @type@ az);

VsgMatrix4@t@ *vsg_matrix4@t@_rotate_euler_new (@type@ alpha,
                                                @type@ beta,
                                                @type@ gamma);

VsgMatrix4@t@ *vsg_matrix4@t@_translate_new (@type@ x, @type@ y, @type@ z);

VsgMatrix4@t@ *vsg_matrix4@t@_scale_new (@type@ x, @type@ y, @type@ z);

VsgMatrix4@t@ *vsg_matrix4@t@_quaternion@t@_new (const VsgQuaternion@t@ *quat);



void vsg_matrix4@t@_set (VsgMatrix4@t@ *mat,
                         @type@ a00, @type@ a01, @type@ a02, @type@ a03,
                         @type@ a10, @type@ a11, @type@ a12, @type@ a13,
                         @type@ a20, @type@ a21, @type@ a22, @type@ a23,
                         @type@ a30, @type@ a31, @type@ a32, @type@ a33);

void vsg_matrix4@t@_identity (VsgMatrix4@t@ *mat);

void vsg_matrix4@t@_copy (const VsgMatrix4@t@ *src,
                          VsgMatrix4@t@ *dst);

VsgMatrix4@t@ *vsg_matrix4@t@_clone (const VsgMatrix4@t@ *src);

void vsg_matrix4@t@_rotate_cardan (VsgMatrix4@t@ *mat,
                                   @type@ ax,
                                   @type@ ay,
                                   @type@ az);

void vsg_matrix4@t@_rotate_x (VsgMatrix4@t@ *mat, @type@ angle);
void vsg_matrix4@t@_rotate_y (VsgMatrix4@t@ *mat, @type@ angle);
void vsg_matrix4@t@_rotate_z (VsgMatrix4@t@ *mat, @type@ angle);

void vsg_matrix4@t@_rotate_euler (VsgMatrix4@t@ *mat,
                                  @type@ alpha,
                                  @type@ beta,
                                  @type@ gamma);

void vsg_matrix4@t@_translate (VsgMatrix4@t@ *mat,
                               @type@ x, @type@ y, @type@ z);

void vsg_matrix4@t@_scale (VsgMatrix4@t@ *mat, @type@ x, @type@ y, @type@ z);

void vsg_matrix4@t@_quaternion@t@_set (VsgMatrix4@t@ *mat,
                                       const VsgQuaternion@t@ *quat);

void vsg_matrix4@t@_quaternion@t@ (VsgMatrix4@t@ *mat,
                                   const VsgQuaternion@t@ *quat);

void vsg_matrix4@t@_add (const VsgMatrix4@t@ *mat,
                         const VsgMatrix4@t@ *other,
                         VsgMatrix4@t@ *result);

void vsg_matrix4@t@_sub (const VsgMatrix4@t@ *mat,
                         const VsgMatrix4@t@ *other,
                         VsgMatrix4@t@ *result);

void vsg_matrix4@t@_matmult (const VsgMatrix4@t@ *mat,
                             const VsgMatrix4@t@ *other,
                             VsgMatrix4@t@ *result);

void vsg_matrix4@t@_vecmult (const VsgMatrix4@t@ *mat,
                             const VsgVector3@t@ *vec,
                             VsgVector3@t@ *result);

void vsg_matrix4@t@_vec4mult (const VsgMatrix4@t@ *mat,
                              const VsgQuaternion@t@ *vec,
                              VsgQuaternion@t@ *result);

void vsg_matrix4@t@_vec4mult_T (const VsgMatrix4@t@ *mat,
                                const VsgQuaternion@t@ *vec,
                                VsgQuaternion@t@ *result);

@type@ vsg_matrix4@t@_det (const VsgMatrix4@t@ *mat);

gboolean vsg_matrix4@t@_invert (const VsgMatrix4@t@ *mat,
                                VsgMatrix4@t@ *result);

@type@ vsg_matrix4@t@_component (const VsgMatrix4@t@ *mat,
                                 guint i, guint j);

void vsg_matrix4@t@_transpose (const VsgMatrix4@t@ *mat,
                               VsgMatrix4@t@ *result);

void vsg_matrix4@t@_write (const VsgMatrix4@t@ *mat, FILE *file);

void vsg_matrix4@t@_print (const VsgMatrix4@t@ *mat);

G_END_DECLS;

#endif /* __VSG_MATRIX4@T@_H__ */
