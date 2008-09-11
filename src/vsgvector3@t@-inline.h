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

#ifndef __VSG_VECTOR3@T@_INLINE_H__ /* __VSG_VECTOR3@T@_INLINE_H__ */
#define __VSG_VECTOR3@T@_INLINE_H__

#include <math.h>

#include <vsg/vsgvector3@t@.h>

#include <vsg/vsgvector3@alt_t@-inline.h>

G_BEGIN_DECLS;


/* public functions */

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_set vsg_vector3@t@_set_inline
#endif
static inline void vsg_vector3@t@_set_inline (VsgVector3@t@ *vec,
                                              @type@ x, @type@ y, @type@ z)
{
  vec->x = x;
  vec->y = y;
  vec->z = z;
}


#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_copy vsg_vector3@t@_copy_inline
#endif
static inline void vsg_vector3@t@_copy_inline (const VsgVector3@t@ *src,
                                               VsgVector3@t@ *dst)
{
  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_scalp vsg_vector3@t@_scalp_inline
#endif
static inline void
vsg_vector3@t@_scalp_inline (const VsgVector3@t@ *vec, @type@ scal,
                             VsgVector3@t@ *res)
{
  res->x = vec->x * scal;
  res->y = vec->y * scal;
  res->z = vec->z * scal;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_dotp vsg_vector3@t@_dotp_inline
#endif
static inline @type@ vsg_vector3@t@_dotp_inline (const VsgVector3@t@ *vec,
                                                 const VsgVector3@t@ *other)
{
  @type@ result;

  result = vec->x * other->x;
  result += vec->y * other->y;
  result += vec->z * other->z;

  return result;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_vecp vsg_vector3@t@_vecp_inline
#endif
static inline void vsg_vector3@t@_vecp_inline (const VsgVector3@t@ *vec,
                                               const VsgVector3@t@ *other,
                                               VsgVector3@t@ *result)
{
  VsgVector3@t@ tmp; /* avoid argument aliasing */

  tmp.x = vec->y * other->z - vec->z * other->y;
  tmp.y = vec->z * other->x - vec->x * other->z;
  tmp.z = vec->x * other->y - vec->y * other->x;

  vsg_vector3@t@_copy_inline (&tmp, result);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_add vsg_vector3@t@_add_inline
#endif
static inline void vsg_vector3@t@_add_inline (const VsgVector3@t@ *vec,
                                              const VsgVector3@t@ *other,
                                              VsgVector3@t@ *result)
{
  result->x = vec->x + other->x;
  result->y = vec->y + other->y;
  result->z = vec->z + other->z;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_sub vsg_vector3@t@_sub_inline
#endif
static inline void vsg_vector3@t@_sub_inline (const VsgVector3@t@ *vec,
                                              const VsgVector3@t@ *other,
                                              VsgVector3@t@ *result)
{
  result->x = vec->x - other->x;
  result->y = vec->y - other->y;
  result->z = vec->z - other->z;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_square_norm vsg_vector3@t@_square_norm_inline
#endif
static inline @type@
vsg_vector3@t@_square_norm_inline (const VsgVector3@t@ *vec)
{
  return vsg_vector3@t@_dotp_inline (vec,vec);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_norm vsg_vector3@t@_norm_inline
#endif
static inline @type@ vsg_vector3@t@_norm_inline (const VsgVector3@t@ *vec)
{
  return sqrt (vsg_vector3@t@_dotp_inline (vec,vec));
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_dist vsg_vector3@t@_dist_inline
#endif
static inline @type@ vsg_vector3@t@_dist_inline (const VsgVector3@t@ *vec,
                                                 const VsgVector3@t@ *other)
{
  VsgVector3@t@ tmp;

  vsg_vector3@t@_sub_inline (vec, other, &tmp);

  return vsg_vector3@t@_norm_inline (&tmp);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_normalize vsg_vector3@t@_normalize_inline
#endif
static inline void vsg_vector3@t@_normalize_inline (VsgVector3@t@ *vec)
{
  @type@ n;

  n = vsg_vector3@t@_norm_inline (vec);

  if (n == 0.) return;

  vsg_vector3@t@_scalp_inline (vec, 1./n, vec);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_lerp vsg_vector3@t@_lerp_inline
#endif
static inline void vsg_vector3@t@_lerp_inline (const VsgVector3@t@ *vec,
                                               const VsgVector3@t@ *other,
                                               @type@ factor,
                                               VsgVector3@t@ *result)
{
  @type@ one_minus_factor = (1.-factor);

  result->x = one_minus_factor * vec->x + factor * other->x;
  result->y = one_minus_factor * vec->y + factor * other->y;
  result->z = one_minus_factor * vec->z + factor * other->z;
}


#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_copy@alt_t@ vsg_vector3@t@_copy@alt_t@_inline
#endif
static inline void vsg_vector3@t@_copy@alt_t@_inline (const VsgVector3@t@ *src,
                                                      VsgVector3@alt_t@ *dst)
{
  dst->x = (@alt_type@) src->x;
  dst->y = (@alt_type@) src->y;
  dst->z = (@alt_type@) src->z;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_is_zero vsg_vector3@t@_is_zero_inline
#endif
static inline gboolean vsg_vector3@t@_is_zero_inline (const VsgVector3@t@ *vec)
{
  return vsg_vector3@t@_square_norm_inline (vec) == 0.;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_to_spherical_internal \
vsg_vector3@t@_to_spherical_internal_inline
#endif
static inline void
vsg_vector3@t@_to_spherical_internal_inline (const VsgVector3@t@ *vec,
                                             @type@ *r,
                                             @type@ *cost, @type@ *sint,
                                             @type@ *cosp, @type@ *sinp)
{
  @type@ r0;

  r0 = vec->x*vec->x + vec->y*vec->y;

  *r = sqrt (r0 + vec->z*vec->z);

  if (*r > 0.)
    {
      r0 = sqrt (r0);
      *cost = vec->z / *r;
      *sint = r0 / *r;

      if (r0 > 0.)
        {
          *cosp = vec->x / r0;
          *sinp = vec->y / r0;
        }
      else
        {
          *cosp = 1.;
          *sinp = 0.;
        }
    }
  else
    {
      *r = 0.;
      *cost = 1.;
      *sint = 0.;
      *cosp = 1.;
      *sinp = 0.;
    }
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_to_spherical vsg_vector3@t@_to_spherical_inline
#endif
static inline void vsg_vector3@t@_to_spherical_inline (const VsgVector3@t@ *vec,
                                                       @type@ *r,
                                                       @type@ *theta,
                                                       @type@ *phi)
{
  @type@ cost, sint, cosp, sinp;

  vsg_vector3@t@_to_spherical_internal_inline (vec, r, &cost, &sint, &cosp,
                                               &sinp);

  *theta = acos (cost); /* theta is always positive */

  *phi = acos (cosp);
  if (sinp < 0.) *phi = - *phi;
}


#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_from_spherical_internal \
vsg_vector3@t@_from_spherical_internal_inline
#endif
static inline void
vsg_vector3@t@_from_spherical_internal_inline (VsgVector3@t@ *vec,
                                               @type@ r,
                                               @type@ cost, @type@ sint,
                                               @type@ cosp, @type@ sinp)
{
  vec->x = r*cosp*sint;
  vec->y = r*sinp*sint;
  vec->z = r*cost;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_from_spherical vsg_vector3@t@_from_spherical_inline
#endif
static inline void vsg_vector3@t@_from_spherical_inline (VsgVector3@t@ *vec,
                                                         @type@ r,
                                                         @type@ theta,
                                                         @type@ phi)
{
  @type@ cost, sint, cosp, sinp;

  cost = cos (theta);
  sint = sin (theta);

  cosp = cos (phi);
  sinp = sin (phi);

  vsg_vector3@t@_from_spherical_internal_inline (vec, r, cost, sint, cosp,
                                                 sinp);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_orient vsg_vector3@t@_orient_inline
#endif
static inline @type@ vsg_vector3@t@_orient_inline (const VsgVector3@t@ *pa,
                                                   const VsgVector3@t@ *pb,
                                                   const VsgVector3@t@ *pc,
                                                   const VsgVector3@t@ *pd)
{
  @type@ a00, a01, a02, a10, a11, a12, a20, a21, a22;

  a00 = pa->x - pd->x;
  a01 = pa->y - pd->y;
  a02 = pa->z - pd->z;

  a10 = pb->x - pd->x;
  a11 = pb->y - pd->y;
  a12 = pb->z - pd->z;

  a20 = pc->x - pd->x;
  a21 = pc->y - pd->y;
  a22 = pc->z - pd->z;

  /* we return opposite of matrix det to be consistant with dim 2 since
   * dim 3 (being odd dimension) implies a opposite result.
   */
  return - (a00 * (a11*a22 - a21*a12)
            - a01 * (a10*a22 - a20*a12)
            + a02 * (a10*a21 - a20*a11));
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_insphere vsg_vector3@t@_insphere_inline
#endif
static inline @type@ vsg_vector3@t@_insphere_inline (const VsgVector3@t@ *pa,
                                                     const VsgVector3@t@ *pb,
                                                     const VsgVector3@t@ *pc,
                                                     const VsgVector3@t@ *pd,
                                                     const VsgVector3@t@ *pe)
{
  @type@ a00, a01, a02, a03,
    a10, a11, a12, a13,
    a20, a21, a22, a23,
    a30, a31, a32, a33;

  a00 = pa->x - pe->x;
  a01 = pa->y - pe->y;
  a02 = pa->z - pe->z;
  a03 = a00*a00 + a01*a01 + a02*a02;

  a10 = pb->x - pe->x;
  a11 = pb->y - pe->y;
  a12 = pb->z - pe->z;
  a13 = a10*a10 + a11*a11 + a12*a12;


  a20 = pc->x - pe->x;
  a21 = pc->y - pe->y;
  a22 = pc->z - pe->z;
  a23 = a20*a20 + a21*a21 + a22*a22;

  a30 = pd->x - pe->x;
  a31 = pd->y - pe->y;
  a32 = pd->z - pe->z;
  a33 = a30*a30 + a31*a31 + a32*a32;

  /* we return opposite of matrix det to be consistant with dim 2 since
   * dim 3 (being odd dimension) implies a opposite result.
   */
  return - ((a00*a11 - a01*a10)*(a22*a33 - a23*a32)
            -(a00*a12 - a02*a10)*(a21*a33 - a23*a31)
            +(a00*a13 - a03*a10)*(a21*a32 - a22*a31)
            +(a01*a12 - a02*a11)*(a20*a33 - a23*a30)
            -(a01*a13 - a03*a11)*(a20*a32 - a22*a30)
            +(a02*a13 - a03*a12)*(a20*a31 - a21*a30));
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_vector3@t@_locfunc \
vsg_vector3@t@_vector3@t@_locfunc_inline
#endif
static inline vsgloc3
vsg_vector3@t@_vector3@t@_locfunc_inline (const VsgVector3@t@ *candidate,
                                          const VsgVector3@t@ *center)
{
  vsgloc3 res = 0;

  if (candidate->x >= center->x)
    res |= VSG_LOC3_X;

  if (candidate->y >= center->y)
    res |= VSG_LOC3_Y;

  if (candidate->z >= center->z)
    res |= VSG_LOC3_Z;

  return res;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_vector3@t@_vector3@alt_t@_locfunc \
vsg_vector3@t@_vector3@alt_t@_locfunc_inline
#endif
static inline vsgloc3
vsg_vector3@t@_vector3@alt_t@_locfunc_inline (const VsgVector3@t@ *candidate,
                                              const VsgVector3@alt_t@ *center)
{
  vsgloc3 res = 0;

  if (candidate->x >= center->x)
    res |= VSG_LOC3_X;

  if (candidate->y >= center->y)
    res |= VSG_LOC3_Y;

  if (candidate->z >= center->z)
    res |= VSG_LOC3_Z;

  return res;
}

G_END_DECLS;

#endif /* __VSG_VECTOR3@T@_INLINE_H__ */
