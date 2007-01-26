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

#ifndef __VSG_VECTOR2@T@_H__ /* __VSG_VECTOR2@T@_H__ */
#define __VSG_VECTOR2@T@_H__

#include <glib.h>
#include <glib-object.h>

#include <stdio.h>

#include <vsg/vsgprtree2-common.h>

G_BEGIN_DECLS;

/* macros */
#define VSG_TYPE_VECTOR2@T@ (vsg_vector2@t@_get_type ())

#define VSG_VECTOR2@T@_COMP(vec,i) ( \
((@type@ *) (vec)) + (i) \
)

/* forward declarations */
#ifndef __VSG_VECTOR2@T@_GOT__
#define __VSG_VECTOR2@T@_GOT__
typedef struct _VsgVector2@t@ VsgVector2@t@;
#endif

#ifndef __VSG_VECTOR2@ALT_T@_GOT__
#define __VSG_VECTOR2@ALT_T@_GOT__
typedef struct _VsgVector2@alt_t@ VsgVector2@alt_t@;
#endif

/* structure definition */
struct _VsgVector2@t@ {

  @type@ x;
  @type@ y;

};

/* constants */

static const VsgVector2@t@ VSG_V2@T@_ZERO = {0., 0.};
static const VsgVector2@t@ VSG_V2@T@_I = {1., 0.};
static const VsgVector2@t@ VSG_V2@T@_J = {0., 1.};

/* functions */
GType vsg_vector2@t@_get_type (void) G_GNUC_CONST;

VsgVector2@t@ *vsg_vector2@t@_new (@type@ x, @type@ y);

void vsg_vector2@t@_free (VsgVector2@t@ *vec);

void vsg_vector2@t@_set (VsgVector2@t@ *vec,
                         @type@ x, @type@ y);

void vsg_vector2@t@_copy (const VsgVector2@t@ *src, VsgVector2@t@ *dst);

VsgVector2@t@ *vsg_vector2@t@_clone (const VsgVector2@t@ *src);

@type@ vsg_vector2@t@_square_norm (const VsgVector2@t@ *vec);

@type@ vsg_vector2@t@_norm (const VsgVector2@t@ *vec);

@type@ vsg_vector2@t@_dist (const VsgVector2@t@ *vec,
                            const VsgVector2@t@ *other);

void vsg_vector2@t@_normalize (VsgVector2@t@ *vec);

void vsg_vector2@t@_scalp(const VsgVector2@t@ *vec,
                          @type@ scal,
                          VsgVector2@t@ *res);

@type@ vsg_vector2@t@_dotp (const VsgVector2@t@ *vec,
                            const VsgVector2@t@ *other);

@type@ vsg_vector2@t@_vecp (const VsgVector2@t@ *vec,
                            const VsgVector2@t@ *other);

void vsg_vector2@t@_add (const VsgVector2@t@ *vec,
                         const VsgVector2@t@ *other,
                         VsgVector2@t@ *result);

void vsg_vector2@t@_sub (const VsgVector2@t@ *vec,
                         const VsgVector2@t@ *other,
                         VsgVector2@t@ *result);

void vsg_vector2@t@_lerp (const VsgVector2@t@ *vec,
                          const VsgVector2@t@ *other,
                          @type@ factor,
                          VsgVector2@t@ *result);

void vsg_vector2@t@_copy@alt_t@ (const VsgVector2@t@ *src,
                                 VsgVector2@alt_t@ *dst);

gboolean vsg_vector2@t@_is_zero (const VsgVector2@t@ *vec);

void vsg_vector2@t@_write (const VsgVector2@t@ *vec, FILE *file);

void vsg_vector2@t@_print (const VsgVector2@t@ *vec);

void vsg_vector2@t@_to_polar_internal (const VsgVector2@t@ *vec,
				       @type@ *r, @type@ *cost, @type@ *sint);

void vsg_vector2@t@_to_polar (const VsgVector2@t@ *vec,
			      @type@ *r, @type@ *theta);

void vsg_vector2@t@_from_polar_internal (VsgVector2@t@ *vec,
					 @type@ r, @type@ cost, @type@ sint);

void vsg_vector2@t@_from_polar (VsgVector2@t@ *vec,
				@type@ r, @type@ theta);

@type@ vsg_vector2@t@_orient (const VsgVector2@t@ *pa,
                              const VsgVector2@t@ *pb,
                              const VsgVector2@t@ *pc);

@type@ vsg_vector2@t@_incircle (const VsgVector2@t@ *pa,
                                const VsgVector2@t@ *pb,
                                const VsgVector2@t@ *pc,
                                const VsgVector2@t@ *pd);

vsgloc2
vsg_vector2@t@_vector2@t@_locfunc (const VsgVector2@t@ *candidate,
                                   const VsgVector2@t@ *center);

vsgloc2
vsg_vector2@t@_vector2@alt_t@_locfunc (const VsgVector2@t@ *candidate,
                                       const VsgVector2@alt_t@ *center);

G_END_DECLS

#endif /* __VSG_VECTOR2@T@_H__ */
