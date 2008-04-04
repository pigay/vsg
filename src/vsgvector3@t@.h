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

#ifndef __VSG_VECTOR3@T@_H__ /* __VSG_VECTOR3@T@_H__ */
#define __VSG_VECTOR3@T@_H__

#include <glib.h>
#include <glib-object.h>

#include <stdio.h>

#include <vsg/vsgprtree3-common.h>

G_BEGIN_DECLS;

/* macros */
#define VSG_TYPE_VECTOR3@T@ (vsg_vector3@t@_get_type ())

#define VSG_VECTOR3@T@_COMP(vec,i) ( \
((@type@ *) (vec)) + (i) \
)

/* forward declarations */
#ifndef __VSG_VECTOR3@T@_GOT__
#define __VSG_VECTOR3@T@_GOT__
typedef struct _VsgVector3@t@ VsgVector3@t@;
#endif

#ifndef __VSG_VECTOR3@ALT_T@_GOT__
#define __VSG_VECTOR3@ALT_T@_GOT__
typedef struct _VsgVector3@alt_t@ VsgVector3@alt_t@;
#endif

/* structure definition */
struct _VsgVector3@t@ {

  @type@ x;
  @type@ y;
  @type@ z;

};

/* constants */

static const VsgVector3@t@ VSG_V3@T@_ZERO = {0., 0., 0.};
static const VsgVector3@t@ VSG_V3@T@_I = {1., 0., 0.};
static const VsgVector3@t@ VSG_V3@T@_J = {0., 1., 0.};
static const VsgVector3@t@ VSG_V3@T@_K = {0., 0., 1.};

/* functions */
GType vsg_vector3@t@_get_type () G_GNUC_CONST;

VsgVector3@t@ *vsg_vector3@t@_new (@type@ x, @type@ y,
                                   @type@ z);

void vsg_vector3@t@_free (VsgVector3@t@ *vec);

void vsg_vector3@t@_set (VsgVector3@t@ *vec,
                         @type@ x, @type@ y, @type@ z);

void vsg_vector3@t@_copy (const VsgVector3@t@ *src, VsgVector3@t@ *dst);

VsgVector3@t@ *vsg_vector3@t@_clone (const VsgVector3@t@ *src);

@type@ vsg_vector3@t@_square_norm (const VsgVector3@t@ *vec);

@type@ vsg_vector3@t@_norm (const VsgVector3@t@ *vec);

@type@ vsg_vector3@t@_dist (const VsgVector3@t@ *vec,
                            const VsgVector3@t@ *other);

void vsg_vector3@t@_normalize(VsgVector3@t@ *vec);

void vsg_vector3@t@_scalp (const VsgVector3@t@ *vec,
                           @type@ scal,
                           VsgVector3@t@ *res);

@type@ vsg_vector3@t@_dotp (const VsgVector3@t@ *vec,
                            const VsgVector3@t@ *other);

void vsg_vector3@t@_vecp (const VsgVector3@t@ *vec,
                          const VsgVector3@t@ *other,
                          VsgVector3@t@ *result);

void vsg_vector3@t@_add (const VsgVector3@t@ *vec,
                         const VsgVector3@t@ *other,
                         VsgVector3@t@ *result);

void vsg_vector3@t@_sub (const VsgVector3@t@ *vec,
                         const VsgVector3@t@ *other,
                         VsgVector3@t@ *result);

void vsg_vector3@t@_lerp (const VsgVector3@t@ *vec,
                          const VsgVector3@t@ *other,
                          @type@ factor,
                          VsgVector3@t@ *result);

void vsg_vector3@t@_copy@alt_t@ (const VsgVector3@t@ *src,
                                 VsgVector3@alt_t@ *dst);

gboolean vsg_vector3@t@_is_zero (const VsgVector3@t@ *vec);

void vsg_vector3@t@_write (const VsgVector3@t@ *vec, FILE *file);

void vsg_vector3@t@_print (const VsgVector3@t@ *vec);

void vsg_vector3@t@_to_spherical_internal (const VsgVector3@t@ *vec,
					   @type@ *r,
					   @type@ *cost, @type@ *sint,
					   @type@ *cosp, @type@ *sinp);

void vsg_vector3@t@_to_spherical (const VsgVector3@t@ *vec,
				  @type@ *r, @type@ *theta, @type@ *phi);

void vsg_vector3@t@_from_spherical_internal (VsgVector3@t@ *vec,
					     @type@ r,
					     @type@ cost, @type@ sint,
					     @type@ cosp, @type@ sinp);

void vsg_vector3@t@_from_spherical (VsgVector3@t@ *vec,
				    @type@ r, @type@ theta, @type@ phi);

@type@ vsg_vector3@t@_orient (const VsgVector3@t@ *pa,
                              const VsgVector3@t@ *pb,
                              const VsgVector3@t@ *pc,
                              const VsgVector3@t@ *pd);

@type@ vsg_vector3@t@_insphere (const VsgVector3@t@ *pa,
                                const VsgVector3@t@ *pb,
                                const VsgVector3@t@ *pc,
                                const VsgVector3@t@ *pd,
                                const VsgVector3@t@ *pe);

vsgloc3 vsg_vector3@t@_vector3@t@_locfunc (const VsgVector3@t@ *candidate,
                                           const VsgVector3@t@ *center);

vsgloc3 vsg_vector3@t@_vector3@alt_t@_locfunc (const VsgVector3@t@ *candidate,
                                               const VsgVector3@alt_t@ *center);
G_END_DECLS

#endif /* __VSG_VECTOR3@T@_H__ */
