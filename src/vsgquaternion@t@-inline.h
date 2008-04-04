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

#ifndef __VSG_QUATERNION@T@_INLINE_H__ /* __VSG_QUATERNION@T@_INLINE_H__ */
#define __VSG_QUATERNION@T@_INLINE_H__

#include <math.h>

#include <vsg/vsgvector3@t@-inline.h>

#include <vsg/vsgquaternion@t@.h>

G_BEGIN_DECLS;


/* public functions */

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_set vsg_quaternion@t@_set_inline
#endif
static inline void vsg_quaternion@t@_set_inline (VsgQuaternion@t@ *quat,
                            @type@ x, @type@ y,
                            @type@ z, @type@ w)
{
  quat->x = x;
  quat->y = y;
  quat->z = z;
  quat->w = w;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_vector3@t@_set vsg_quaternion@t@_vector3@t@_set_inline
#endif
static inline void
vsg_quaternion@t@_vector3@t@_set_inline (VsgQuaternion@t@ *quat,
                                         const VsgVector3@t@ *vector)
{
  quat->x = vector->x;
  quat->y = vector->y;
  quat->z = vector->z;
  quat->w = (@type@) 1.;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_vector3@alt_t@_set \
vsg_quaternion@t@_vector3@alt_t@_set_inline
#endif
static inline void
vsg_quaternion@t@_vector3@alt_t@_set_inline (VsgQuaternion@t@ *quat,
                                             const VsgVector3@alt_t@ *vector)
{
  quat->x = (@type@) vector->x;
  quat->y = (@type@) vector->y;
  quat->z = (@type@) vector->z;
  quat->w = (@type@) 1.;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_scalp vsg_quaternion@t@_scalp_inline
#endif
static inline void
vsg_quaternion@t@_scalp_inline (const VsgQuaternion@t@ *quat,
                                @type@ scal,
                                VsgQuaternion@t@ *result)
{
  result->x = quat->x * scal;
  result->y = quat->y * scal;
  result->z = quat->z * scal;
  result->w = quat->w * scal;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_dotp vsg_quaternion@t@_dotp_inline
#endif
static inline @type@
vsg_quaternion@t@_dotp_inline (const VsgQuaternion@t@ *quat,
                               const VsgQuaternion@t@ *other)
{
  return
    quat->x * other->x +
    quat->y * other->y +
    quat->z * other->z +
    quat->w * other->w;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_conjugate vsg_quaternion@t@_conjugate_inline
#endif
static inline void
vsg_quaternion@t@_conjugate_inline (const VsgQuaternion@t@ *quat,
                                    VsgQuaternion@t@ *result)
{
  result->x = -quat->x;
  result->y = -quat->y;
  result->z = -quat->z;
  result->w = quat->w;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_copy vsg_quaternion@t@_copy_inline
#endif
static inline void vsg_quaternion@t@_copy_inline (const VsgQuaternion@t@ *src,
                                                  VsgQuaternion@t@ *dst)
{
  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;
  dst->w = src->w;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_mult vsg_quaternion@t@_mult_inline
#endif
static inline void vsg_quaternion@t@_mult_inline (const VsgQuaternion@t@ *quat,
                                                  const VsgQuaternion@t@ *other,
                                                  VsgQuaternion@t@ *result)
{
  VsgQuaternion@t@ tmp;                /* avoid argument aliasing */

  tmp.x = quat->w * other->x + quat->x * other->w +
    quat->y * other->z - quat->z * other->y;

  tmp.y = quat->w * other->y + quat->y * other->w +
    quat->z * other->x - quat->x * other->z;

  tmp.z = quat->w * other->z + quat->z * other->w +
    quat->x * other->y - quat->y * other->x;

  tmp.w = quat->w * other->w
    - quat->x * other->x - quat->y * other->y - quat->z * other->z;

  vsg_quaternion@t@_copy_inline (&tmp, result);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_make_rotate@t@ \
vsg_quaternion@t@_make_rotate@t@_inline
#endif
static inline void
vsg_quaternion@t@_make_rotate@t@_inline (VsgQuaternion@t@ *quat,
                                         @type@ angle,
                                         const VsgVector3@t@ *vec)
{
  @type@ ca = cos (angle * .5);
  @type@ sa = sin (angle * .5);

  VsgVector3@t@ axis = *vec;

  vsg_vector3@t@_normalize_inline (&axis);

  vsg_quaternion@t@_set_inline (quat,
                                axis.x*sa,
                                axis.y*sa,
                                axis.z*sa,
                                ca);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_rotate@t@_set vsg_quaternion@t@_rotate@t@_set_inline
#endif
static inline void
vsg_quaternion@t@_rotate@t@_set_inline (VsgQuaternion@t@ *quat,
                                        const VsgVector3@t@ *from,
                                        const VsgVector3@t@ *to)
{
  /*
   * code inspired from OpenSceneGraph Project:
   * http://www.openscenegraph.org/
   */
  VsgVector3@t@ axis;
  @type@ cosangle, angle;

  cosangle = vsg_vector3@t@_dotp_inline (from, to)
    /(vsg_vector3@t@_norm_inline (from) * vsg_vector3@t@_norm_inline (to));

  if (cosangle == 1.)
    {
      vsg_quaternion@t@_set_inline (quat, 0., 0., 0., 1.);
    }
  else if (cosangle == -1.)
    {
      if (fabs (from->x) < fabs (from->y))
        if (fabs(from->x) < fabs(from->z))
          vsg_vector3@t@_set_inline (&axis, 1., 0., 0.);
        else vsg_vector3@t@_set_inline (&axis, 0., 0., 1.);
      else if (fabs (from->y) < fabs(from->z))
        vsg_vector3@t@_set_inline (&axis, 0., 1., 0.);
      else vsg_vector3@t@_set_inline (&axis, 0., 0., 1.);

      vsg_vector3@t@_vecp_inline (from,&axis,&axis);
      vsg_vector3@t@_normalize_inline (&axis);

      vsg_quaternion@t@_set_inline (quat, axis.x, axis.y, axis.z, 0.);
    }
  else
    {
      vsg_vector3@t@_vecp_inline (from, to, &axis);

      angle = acos (cosangle);

      vsg_vector3@t@_normalize_inline (&axis);

      vsg_quaternion@t@_make_rotate@t@_inline (quat, angle, &axis);
    }
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_identity vsg_quaternion@t@_identity_inline
#endif
static inline void vsg_quaternion@t@_identity_inline (VsgQuaternion@t@ *quat)
{
  vsg_quaternion@t@_copy_inline (&VSG_Q@T@_ID, quat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_rotate_x vsg_quaternion@t@_rotate_x_inline
#endif
static inline void vsg_quaternion@t@_rotate_x_inline (VsgQuaternion@t@ *quat,
                                                      @type@ angle)
{
  VsgQuaternion@t@ tmp;
  @type@ ca = cos (angle * .5);
  @type@ sa = sin (angle * .5);

  vsg_quaternion@t@_set_inline (&tmp, sa, 0., 0., ca);
  vsg_quaternion@t@_mult_inline (quat, &tmp, quat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_rotate_y vsg_quaternion@t@_rotate_y_inline
#endif
static inline void vsg_quaternion@t@_rotate_y_inline (VsgQuaternion@t@ *quat,
                                                      @type@ angle)
{
  VsgQuaternion@t@ tmp;
  @type@ ca = cos (angle * .5);
  @type@ sa = sin (angle * .5);

  vsg_quaternion@t@_set_inline (&tmp, 0., sa, 0., ca);
  vsg_quaternion@t@_mult_inline (quat, &tmp, quat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_rotate_z vsg_quaternion@t@_rotate_z_inline
#endif
static inline void vsg_quaternion@t@_rotate_z_inline (VsgQuaternion@t@ *quat,
                                                      @type@ angle)
{
  VsgQuaternion@t@ tmp;
  @type@ ca = cos (angle * .5);
  @type@ sa = sin (angle * .5);

  vsg_quaternion@t@_set_inline (&tmp, 0., 0., sa, ca);
  vsg_quaternion@t@_mult_inline (quat, &tmp, quat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_rotate_cardan vsg_quaternion@t@_rotate_cardan_inline
#endif
static inline void
vsg_quaternion@t@_rotate_cardan_inline (VsgQuaternion@t@ *quat,
                                        @type@ ax,
                                        @type@ ay,
                                        @type@ az)
{
  vsg_quaternion@t@_rotate_z_inline (quat, az);

  vsg_quaternion@t@_rotate_y_inline (quat, ay);

  vsg_quaternion@t@_rotate_x_inline (quat, ax);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_rotate_euler vsg_quaternion@t@_rotate_euler_inline
#endif
static inline void
vsg_quaternion@t@_rotate_euler_inline (VsgQuaternion@t@ *quat,
                                       @type@ alpha,
                                       @type@ beta,
                                       @type@ gamma)
{
  vsg_quaternion@t@_rotate_z_inline (quat, gamma);

  vsg_quaternion@t@_rotate_y_inline (quat, beta);

  vsg_quaternion@t@_rotate_z_inline (quat, alpha);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_square_norm vsg_quaternion@t@_square_norm_inline
#endif
static inline @type@
vsg_quaternion@t@_square_norm_inline (const VsgQuaternion@t@ *quat)
{
  return vsg_quaternion@t@_dotp_inline (quat, quat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_norm vsg_quaternion@t@_norm_inline
#endif
static inline @type@
vsg_quaternion@t@_norm_inline (const VsgQuaternion@t@ *quat)
{
  return sqrt (vsg_quaternion@t@_square_norm_inline (quat));
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_normalize vsg_quaternion@t@_normalize_inline
#endif
static inline void vsg_quaternion@t@_normalize_inline (VsgQuaternion@t@ *quat)
{
  @type@ norm;

  norm = vsg_quaternion@t@_norm_inline (quat);

  if (norm > 0.0)
    {
      vsg_quaternion@t@_scalp_inline (quat, 1. / norm, quat);
    }
  else
    {
      vsg_quaternion@t@_set_inline (quat, 0., 0., 0., 1.);
    }
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_invert vsg_quaternion@t@_invert_inline
#endif
static inline gboolean
vsg_quaternion@t@_invert_inline (const VsgQuaternion@t@ *quat,
                                 VsgQuaternion@t@ *result)
{
  @type@ n2;

  n2 = vsg_quaternion@t@_square_norm_inline (quat);

  vsg_quaternion@t@_conjugate_inline (quat, result);

  if (n2 > 0.)
    {
      vsg_quaternion@t@_scalp_inline (result, 1. / n2, result);
      return TRUE;
    }
  else
    {
      g_critical ("%s: inversion on null quaternion",
                  __PRETTY_FUNCTION__);
      vsg_quaternion@t@_set_inline (result, 0., 0., 0., 1.);
      return FALSE;
    }
  return TRUE;
}


#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_quaternion@t@_slerp vsg_quaternion@t@_slerp_inline
#endif
static inline void
vsg_quaternion@t@_slerp_inline (const VsgQuaternion@t@ *quat1,
                                const VsgQuaternion@t@ *quat2,
                                @type@ t, VsgQuaternion@t@ *result)
{
  /* code from ggt.sourceforge.net */

  @type@ cosom;

  // calc cosine theta
  cosom = vsg_quaternion@t@_dotp_inline (quat1, quat2);

  // Calculate coefficients
  @type@ sclp, sclq;

  if ((1.0 - cosom) > 0.0001) // 0.0001 -> some epsillon
    {
      // Standard case (slerp)
      @type@ omega, sinom;

      omega = acos (cosom); // extract theta from dot product's cos theta
      sinom = sin (omega);
      sclp  = sin ((1.0 - t) * omega) / sinom;
      sclq  = sin (t * omega) / sinom;
    }
  else
    {
      // Very close, do linear interp (because it's faster)
      sclp = 1.0 - t;
      sclq = t;
    }

   result->x = sclp * quat1->x + sclq * quat2->x;
   result->y = sclp * quat1->y + sclq * quat2->y;
   result->z = sclp * quat1->z + sclq * quat2->z;
   result->w = sclp * quat1->w + sclq * quat2->w;
}

G_END_DECLS;

#endif /* __VSG_QUATERNION@T@_INLINE_H__ */
