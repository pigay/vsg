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

#ifndef __VSG_VECTOR2@T@_INLINE_H__ /* __VSG_VECTOR2@T@_INLINE_H__ */
#define __VSG_VECTOR2@T@_INLINE_H__

#include <math.h>

#include <vsg/vsgvector2@t@.h>

#include <vsg/vsgvector2@alt_t@-inline.h>

G_BEGIN_DECLS;


/* public functions */

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_set vsg_vector2@t@_set_inline
#endif
static inline void vsg_vector2@t@_set_inline (VsgVector2@t@ *vec,
                                              @type@ x, @type@ y)
{
  vec->x = x;
  vec->y = y;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_copy vsg_vector2@t@_copy_inline
#endif
static inline void vsg_vector2@t@_copy_inline (const VsgVector2@t@ *src,
                                       VsgVector2@t@ *dst)
{
  dst->x = src->x;
  dst->y = src->y;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_dotp vsg_vector2@t@_dotp_inline
#endif
static inline @type@ vsg_vector2@t@_dotp_inline (const VsgVector2@t@ *vec,
                                                 const VsgVector2@t@ *other)
{
  @type@ result;

  result = vec->x * other->x;
  result += vec->y * other->y;

  return result;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_scalp vsg_vector2@t@_scalp_inline
#endif
static inline void vsg_vector2@t@_scalp_inline (const VsgVector2@t@ *vec,
                                                @type@ scal,
                                                VsgVector2@t@ *res)
{
  res->x = vec->x * scal;
  res->y = vec->y * scal;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_vecp vsg_vector2@t@_vecp_inline
#endif
static inline @type@ vsg_vector2@t@_vecp_inline (const VsgVector2@t@ *vec,
                                                 const VsgVector2@t@ *other)
{
  return vec->x * other->y - vec->y * other->x;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_add vsg_vector2@t@_add_inline
#endif
static inline void vsg_vector2@t@_add_inline (const VsgVector2@t@ *vec,
                                              const VsgVector2@t@ *other,
                                              VsgVector2@t@ *result)
{
  result->x = vec->x + other->x;
  result->y = vec->y + other->y;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_sub vsg_vector2@t@_sub_inline
#endif
static inline void vsg_vector2@t@_sub_inline (const VsgVector2@t@ *vec,
                                              const VsgVector2@t@ *other,
                                              VsgVector2@t@ *result)
{
  result->x = vec->x - other->x;
  result->y = vec->y - other->y;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_square_norm vsg_vector2@t@_square_norm_inline
#endif
static inline @type@
vsg_vector2@t@_square_norm_inline (const VsgVector2@t@ *vec)
{
  return vsg_vector2@t@_dotp_inline (vec,vec);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_norm vsg_vector2@t@_norm_inline
#endif
static inline @type@ vsg_vector2@t@_norm_inline (const VsgVector2@t@ *vec)
{
  return sqrt (vsg_vector2@t@_dotp_inline (vec, vec));
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_dist vsg_vector2@t@_dist_inline
#endif
static inline @type@ vsg_vector2@t@_dist_inline (const VsgVector2@t@ *vec,
                                                 const VsgVector2@t@ *other)
{
  VsgVector2@t@ tmp;

  vsg_vector2@t@_sub_inline (vec, other, &tmp);

  return vsg_vector2@t@_norm_inline (&tmp);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_normalize vsg_vector2@t@_normalize_inline
#endif
static inline void vsg_vector2@t@_normalize_inline (VsgVector2@t@ *vec)
{
  @type@ n;

  n = vsg_vector2@t@_norm_inline (vec);

  if (n == 0.) return;

  vsg_vector2@t@_scalp_inline (vec, 1./n, vec);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_lerp vsg_vector2@t@_lerp_inline
#endif
static inline void vsg_vector2@t@_lerp_inline (const VsgVector2@t@ *vec,
                                               const VsgVector2@t@ *other,
                                               @type@ factor,
                                               VsgVector2@t@ *result)
{
  @type@ one_minus_factor = (1.-factor);

  result->x = one_minus_factor * vec->x + factor * other->x;
  result->y = one_minus_factor * vec->y + factor * other->y;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_copy@alt_t@ vsg_vector2@t@_copy@alt_t@_inline
#endif
static inline void vsg_vector2@t@_copy@alt_t@_inline (const VsgVector2@t@ *src,
                                                      VsgVector2@alt_t@ *dst)
{
  dst->x = (@alt_type@) src->x;
  dst->y = (@alt_type@) src->y;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_is_zero vsg_vector2@t@_is_zero_inline
#endif
static inline gboolean vsg_vector2@t@_is_zero_inline (const VsgVector2@t@ *vec)
{
  return vsg_vector2@t@_square_norm_inline (vec) == 0.;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_to_polar_internal \
vsg_vector2@t@_to_polar_internal_inline
#endif
static inline void
vsg_vector2@t@_to_polar_internal_inline (const VsgVector2@t@ *vec,
                                         @type@ *r, @type@ *cost, @type@ *sint)
{
  *r = vsg_vector2@t@_norm_inline (vec);

  if (*r > 0.)
    {
      *cost = vec->x / *r;
      *sint = vec->y / *r;
    }
  else
    {
      *r = 0.;
      *cost = 1.;
      *sint = 0.;
    }
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_to_polar vsg_vector2@t@_to_polar_inline
#endif
static inline void vsg_vector2@t@_to_polar_inline (const VsgVector2@t@ *vec,
                                                   @type@ *r, @type@ *theta)
{
  @type@ cost, sint;

  vsg_vector2@t@_to_polar_internal_inline (vec, r, &cost, &sint);

  *theta = acos (cost);

  if (sint < 0.) *theta = - *theta;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_from_polar_internal \
vsg_vector2@t@_from_polar_internal_inline
#endif
static inline void
vsg_vector2@t@_from_polar_internal_inline (VsgVector2@t@ *vec,
                                           @type@ r, @type@ cost, @type@ sint)
{
  vec->x = r*cost;
  vec->y = r*sint;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_from_polar vsg_vector2@t@_from_polar_inline
#endif
static inline void vsg_vector2@t@_from_polar_inline (VsgVector2@t@ *vec,
                                                     @type@ r, @type@ theta)
{
  @type@ cost, sint;

  cost = cos (theta);
  sint = sin (theta);

  vsg_vector2@t@_from_polar_internal_inline (vec, r, cost, sint);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_orient vsg_vector2@t@_orient_inline
#endif
static inline @type@ vsg_vector2@t@_orient_inline (const VsgVector2@t@ *pa,
                                                   const VsgVector2@t@ *pb,
                                                   const VsgVector2@t@ *pc)
{
  @type@ a00, a01, a10, a11;

  a00 = pa->x - pc->x;
  a01 = pa->y - pc->y;

  a10 = pb->x - pc->x;
  a11 = pb->y - pc->y;

  return a00*a11 - a10*a01;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_incircle vsg_vector2@t@_incircle_inline
#endif
static inline @type@ vsg_vector2@t@_incircle_inline (const VsgVector2@t@ *pa,
                                                     const VsgVector2@t@ *pb,
                                                     const VsgVector2@t@ *pc,
                                                     const VsgVector2@t@ *pd)
{
  @type@ a00, a01, a02, a10, a11, a12, a20, a21, a22;

  a00 = pa->x - pd->x;
  a01 = pa->y - pd->y;
  a02 = a00*a00 + a01*a01;

  a10 = pb->x - pd->x;
  a11 = pb->y - pd->y;
  a12 = a10*a10 + a11*a11;

  a20 = pc->x - pd->x;
  a21 = pc->y - pd->y;
  a22 = a20*a20 + a21*a21;

  return a00 * (a11*a22 - a21*a12)
    - a01 * (a10*a22 - a20*a12)
    + a02 * (a10*a21 - a20*a11);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_vector2@t@_locfunc \
vsg_vector2@t@_vector2@t@_locfunc_inline
#endif
static inline vsgloc2
vsg_vector2@t@_vector2@t@_locfunc_inline (const VsgVector2@t@ *candidate,
                                          const VsgVector2@t@ *center)
{
  vsgloc2 res = 0;

  if (candidate->x >= center->x)
    res |= VSG_LOC2_X;

  if (candidate->y >= center->y)
    res |= VSG_LOC2_Y;

  return res;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector2@t@_vector2@alt_t@_locfunc \
vsg_vector2@t@_vector2@alt_t@_locfunc_inline
#endif
static inline vsgloc2
vsg_vector2@t@_vector2@alt_t@_locfunc_inline (const VsgVector2@t@ *candidate,
                                              const VsgVector2@alt_t@ *center)
{
  vsgloc2 res = 0;

  if (candidate->x >= center->x)
    res |= VSG_LOC2_X;

  if (candidate->y >= center->y)
    res |= VSG_LOC2_Y;

  return res;
}

G_END_DECLS;

#endif /* __VSG_VECTOR2@T@_INLINE_H__ */
