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

#ifndef __VSG_QUATERNION@T@_H__ /* __VSG_QUATERNION@T@_H__ */
#define __VSG_QUATERNION@T@_H__

#include <vsg/vsgvector3@t@.h>
#include <vsg/vsgvector3@alt_t@.h>


#include <glib.h>
#include <glib-object.h>

#include <stdio.h>

G_BEGIN_DECLS;

/* macros */
#define VSG_TYPE_QUATERNION@T@ (vsg_quaternion@t@_get_type ())

#define VSG_QUATERNION@T@_COMP(quat,i) ( \
((@type@ *) (quat)) + (i) \
)

/* forward declarations */
typedef struct _VsgQuaternion@t@ VsgQuaternion@t@;

/* structure definition */
struct _VsgQuaternion@t@
{

  @type@ x;
  @type@ y;
  @type@ z;
  @type@ w;

};

/* constants */

static const VsgQuaternion@t@ VSG_Q@T@_ID = {0., 0., 0., 1.};

/* functions */
GType vsg_quaternion@t@_get_type () G_GNUC_CONST;

VsgQuaternion@t@ *vsg_quaternion@t@_new (@type@ x,
                                         @type@ y,
                                         @type@ z,
                                         @type@ w);

void vsg_quaternion@t@_free (VsgQuaternion@t@ *quat);

VsgQuaternion@t@ *vsg_quaternion@t@_identity_new ();

VsgQuaternion@t@ *
vsg_quaternion@t@_vector3@t@_new (const VsgVector3@t@ *vector);

VsgQuaternion@t@ *
vsg_quaternion@t@_vector3@alt_t@_new (const VsgVector3@alt_t@ *vector);

void vsg_quaternion@t@_set (VsgQuaternion@t@ *quat,
                            @type@ x,
                            @type@ y,
                            @type@ z,
                            @type@ w);


void vsg_quaternion@t@_vector3@t@_set (VsgQuaternion@t@ *quat,
                                       const VsgVector3@t@ *vector);
void
vsg_quaternion@t@_vector3@alt_t@_set (VsgQuaternion@t@ *quat,
                                      const VsgVector3@alt_t@ *vector);

void vsg_quaternion@t@_identity (VsgQuaternion@t@ *quat);

void vsg_quaternion@t@_make_rotate@t@ (VsgQuaternion@t@ *quat,
                                       @type@ angle,
                                       const VsgVector3@t@ *vec);

void vsg_quaternion@t@_rotate@t@_set (VsgQuaternion@t@ *quat,
                                      const VsgVector3@t@ *from,
                                      const VsgVector3@t@ *to);

void vsg_quaternion@t@_rotate_x (VsgQuaternion@t@ *quat, @type@ angle);
void vsg_quaternion@t@_rotate_y (VsgQuaternion@t@ *quat, @type@ angle);
void vsg_quaternion@t@_rotate_z (VsgQuaternion@t@ *quat, @type@ angle);

void vsg_quaternion@t@_rotate_cardan (VsgQuaternion@t@ *quat,
                                      @type@ ax,
                                      @type@ ay,
                                      @type@ az);

void vsg_quaternion@t@_rotate_euler (VsgQuaternion@t@ *quat,
                                     @type@ alpha,
                                     @type@ beta,
                                     @type@ gamma);

void vsg_quaternion@t@_copy (const VsgQuaternion@t@ *src,
                             VsgQuaternion@t@ *dst);

VsgQuaternion@t@ *vsg_quaternion@t@_clone (const VsgQuaternion@t@ *src);

@type@ vsg_quaternion@t@_square_norm (const VsgQuaternion@t@ *quat);

@type@ vsg_quaternion@t@_norm (const VsgQuaternion@t@ *quat);

void vsg_quaternion@t@_normalize (VsgQuaternion@t@ *quat);
void vsg_quaternion@t@_scalp (const VsgQuaternion@t@ *quat,
                              @type@ scal,
                              VsgQuaternion@t@ *result);

@type@ vsg_quaternion@t@_dotp (const VsgQuaternion@t@ *quat,
                               const VsgQuaternion@t@ *other);

void vsg_quaternion@t@_mult (const VsgQuaternion@t@ *quat,
                             const VsgQuaternion@t@ *other,
                             VsgQuaternion@t@ *result);

gboolean vsg_quaternion@t@_invert (const VsgQuaternion@t@ *quat,
                                   VsgQuaternion@t@ *result);


void vsg_quaternion@t@_slerp (const VsgQuaternion@t@ *quat1,
                              const VsgQuaternion@t@ *quat2,
                              @type@ t,
                              VsgQuaternion@t@ *result);

void vsg_quaternion@t@_conjugate (const VsgQuaternion@t@ *quat,
                                  VsgQuaternion@t@ *result);

void vsg_quaternion@t@_write (const VsgQuaternion@t@ *quat,
                              FILE *file);

void vsg_quaternion@t@_print (const VsgQuaternion@t@ *quat);

G_END_DECLS

#endif /* __VSG_QUATERNION@T@_H__ */
