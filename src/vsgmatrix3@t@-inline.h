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

#ifndef __VSG_MATRIX3@T@_INLINE_H__ /* __VSG_MATRIX3@T@_INLINE_H__ */
#define __VSG_MATRIX3@T@_INLINE_H__

#include <math.h>

#include <vsg/vsgvector2@t@-inline.h>
#include <vsg/vsgvector3@t@-inline.h>

#include <vsg/vsgmatrix3@t@.h>

G_BEGIN_DECLS;


/* macros */

/* 
 * VSG_MATRIX3@T@_COMP(i,j)= components[i+3*j]
 * this is because we want to get OpenGL compliant matrices.
 */

#define VSG_MATRIX3@T@_COMP(mat, i, j) ( \
* ((mat) -> components + i + 3*j) \
)

/* public functions */

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_set vsg_matrix3@t@_set_inline
#endif
static inline void
vsg_matrix3@t@_set_inline (VsgMatrix3@t@ *mat,
                           @type@ a00, @type@ a01, @type@ a02,
                           @type@ a10, @type@ a11, @type@ a12,
                           @type@ a20, @type@ a21, @type@ a22)
{
  VSG_MATRIX3@T@_COMP (mat, 0, 0) = a00;
  VSG_MATRIX3@T@_COMP (mat, 0, 1) = a01;
  VSG_MATRIX3@T@_COMP (mat, 0, 2) = a02;

  VSG_MATRIX3@T@_COMP (mat, 1, 0) = a10;
  VSG_MATRIX3@T@_COMP (mat, 1, 1) = a11;
  VSG_MATRIX3@T@_COMP (mat, 1, 2) = a12;

  VSG_MATRIX3@T@_COMP (mat, 2, 0) = a20;
  VSG_MATRIX3@T@_COMP (mat, 2, 1) = a21;
  VSG_MATRIX3@T@_COMP (mat, 2, 2) = a22;

}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_copy vsg_matrix3@t@_copy_inline
#endif
static inline void vsg_matrix3@t@_copy_inline (const VsgMatrix3@t@ *src,
                                               VsgMatrix3@t@ *dst)
{
  guint i, j;

  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      VSG_MATRIX3@T@_COMP (dst, i, j) =
        VSG_MATRIX3@T@_COMP (src, i, j);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_add vsg_matrix3@t@_add_inline
#endif
static inline void vsg_matrix3@t@_add_inline (const VsgMatrix3@t@ *mat,
                                              const VsgMatrix3@t@ *other,
                                              VsgMatrix3@t@ *result)
{
  guint i, j;

  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
        {
          VSG_MATRIX3@T@_COMP (result, i, j) = 
            VSG_MATRIX3@T@_COMP (mat, i, j) +
            VSG_MATRIX3@T@_COMP (other, i, j);
        }
    }
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_sub vsg_matrix3@t@_sub_inline
#endif
static inline void vsg_matrix3@t@_sub_inline (const VsgMatrix3@t@ *mat,
                                              const VsgMatrix3@t@ *other,
                                              VsgMatrix3@t@ *result)
{
  guint i, j;

  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
        {
          VSG_MATRIX3@T@_COMP (result, i, j) = 
            VSG_MATRIX3@T@_COMP (mat, i, j) -
            VSG_MATRIX3@T@_COMP (other, i, j);
        }
    }
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_matmult vsg_matrix3@t@_matmult_inline
#endif
static inline void vsg_matrix3@t@_matmult_inline (const VsgMatrix3@t@ *mat,
                                                  const VsgMatrix3@t@ *other,
                                                  VsgMatrix3@t@ *result)
{
  VsgMatrix3@t@ tmp = VSG_M3@T@_ZERO;
  guint i, j, k;

  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
        {
          @type@ rtmp = 0.;

          for (k = 0; k < 3; k++)
            {
              rtmp += VSG_MATRIX3@T@_COMP (mat, i, k) *
                VSG_MATRIX3@T@_COMP (other, k, j);
            }

          VSG_MATRIX3@T@_COMP (&tmp, i, j) = rtmp;
        }
    }

  vsg_matrix3@t@_copy_inline (&tmp, result);

}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_vecmult vsg_matrix3@t@_vecmult_inline
#endif
static inline void vsg_matrix3@t@_vecmult_inline (const VsgMatrix3@t@ *mat,
                                                  const VsgVector2@t@ *vec,
                                                  VsgVector2@t@ *result)
{
  guint i, j;
  @type@ tmpvec[3] = {vec->x, vec->y, 1.};
  @type@ tmpres[3] = {0., 0., 0.};

  for (i = 0; i < 2; i++)
    {
      @type@ rtmp = 0;
      for (j = 0; j < 3; j++)
        {
          rtmp += VSG_MATRIX3@T@_COMP (mat, i, j) * tmpvec[j];
        }
      tmpres[i] = rtmp;
    }

  vsg_vector2@t@_set_inline (result, tmpres[0], tmpres[1]);
}


#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_vec3mult vsg_matrix3@t@_vec3mult_inline
#endif
static inline void vsg_matrix3@t@_vec3mult_inline (const VsgMatrix3@t@ *mat,
                                                   const VsgVector3@t@ *vec,
                                                   VsgVector3@t@ *result)
{
  guint i, j;

  @type@ tmpvec[3] = {vec->x, vec->y, vec->z};
  @type@ tmpres[3] = {0., 0., 0.};  /* avoid argument aliasing */

  for (i = 0; i < 3; i++)
    {
      @type@ rtmp = 0;
      for (j = 0; j < 3; j++)
        {
          rtmp += VSG_MATRIX3@T@_COMP (mat, i, j) * tmpvec[j];
        }
      tmpres[i] = rtmp;
    }

  result->x = tmpres[0];
  result->y = tmpres[1];
  result->z = tmpres[2];
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_vec3mult_T vsg_matrix3@t@_vec3mult_T_inline
#endif
static inline void vsg_matrix3@t@_vec3mult_T_inline (const VsgMatrix3@t@ *mat,
                                                     const VsgVector3@t@ *vec,
                                                     VsgVector3@t@ *result)
{
  guint i, j;

  @type@ tmpvec[3] = {vec->x, vec->y, vec->z};
  @type@ tmpres[3] = {0., 0., 0.};  /* avoid argument aliasing */

  for (i = 0; i < 3; i++)
    {
      @type@ rtmp = 0;
      for (j = 0; j < 3; j++)
        {
          rtmp += VSG_MATRIX3@T@_COMP (mat, j, i) * tmpvec[j];
        }

      tmpres[i] = rtmp;
    }

  result->x = tmpres[0];
  result->y = tmpres[1];
  result->z = tmpres[2];
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_identity vsg_matrix3@t@_identity_inline
#endif
static inline void vsg_matrix3@t@_identity_inline (VsgMatrix3@t@ *mat)
{
  vsg_matrix3@t@_copy_inline (&VSG_M3@T@_ID, mat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_rotate vsg_matrix3@t@_rotate_inline
#endif
static inline void vsg_matrix3@t@_rotate_inline (VsgMatrix3@t@ *mat,
                                                 @type@ a)
{
  VsgMatrix3@t@ rot;
  @type@ ca, sa;

  ca = cos (a);
  sa = sin (a);

  vsg_matrix3@t@_set_inline (&rot, ca, -sa, 0., sa, ca, 0., 0., 0., 1.);


  vsg_matrix3@t@_matmult (mat, &rot, mat);

}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_translate vsg_matrix3@t@_translate_inline
#endif
static inline void vsg_matrix3@t@_translate_inline (VsgMatrix3@t@ *mat,
                                                    @type@ x, @type@ y)
{
  VsgMatrix3@t@ trans;

  vsg_matrix3@t@_set_inline (&trans, 1., 0., x, 0., 1., y, 0., 0., 1.);

  vsg_matrix3@t@_matmult_inline (mat, &trans, mat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_scale vsg_matrix3@t@_scale_inline
#endif
static inline void vsg_matrix3@t@_scale_inline (VsgMatrix3@t@ *mat, @type@ x,
                                                @type@ y)
{
  VsgMatrix3@t@ scale;

  vsg_matrix3@t@_set_inline (&scale, x, 0., 0., 0., y, 0., 0., 0., 1.);

  vsg_matrix3@t@_matmult_inline (mat, &scale, mat);

}

/* inspired from www.nebula.org */
#define _M3_00 VSG_MATRIX3@T@_COMP (mat,0,0)
#define _M3_01 VSG_MATRIX3@T@_COMP (mat,0,1)
#define _M3_02 VSG_MATRIX3@T@_COMP (mat,0,2)
#define _M3_10 VSG_MATRIX3@T@_COMP (mat,1,0)
#define _M3_11 VSG_MATRIX3@T@_COMP (mat,1,1)
#define _M3_12 VSG_MATRIX3@T@_COMP (mat,1,2)
#define _M3_20 VSG_MATRIX3@T@_COMP (mat,2,0)
#define _M3_21 VSG_MATRIX3@T@_COMP (mat,2,1)
#define _M3_22 VSG_MATRIX3@T@_COMP (mat,2,2)

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_det vsg_matrix3@t@_det_inline
#endif
static inline @type@ vsg_matrix3@t@_det_inline (const VsgMatrix3@t@ *mat)
{
  return
    _M3_00 * (_M3_11 * _M3_22 - _M3_21 * _M3_12)
    - _M3_01 * (_M3_10 * _M3_22 - _M3_20 * _M3_12)
    + _M3_02 * (_M3_10 * _M3_21 - _M3_20 * _M3_11);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_invert vsg_matrix3@t@_invert_inline
#endif
static inline gboolean vsg_matrix3@t@_invert_inline (const VsgMatrix3@t@ *mat,
                                                     VsgMatrix3@t@ *result)
{
  @type@ s;

  s = vsg_matrix3@t@_det (mat);

  if (s == 0.0)
    {
      g_critical ("invalid VsgMatrix3@t@ for inversion.\n");
      return FALSE;
    }

  s = 1. / s;
  vsg_matrix3@t@_set_inline (result,
                             s * (_M3_11 * _M3_22 - _M3_12 * _M3_21),
                             s * (_M3_21 * _M3_02 - _M3_22 * _M3_01),
                             s * (_M3_01 * _M3_12 - _M3_02 * _M3_11),
                             s * (_M3_12 * _M3_20 - _M3_10 * _M3_22),
                             s * (_M3_22 * _M3_00 - _M3_20 * _M3_02),
                             s * (_M3_02 * _M3_10 - _M3_00 * _M3_12),
                             s * (_M3_10 * _M3_21 - _M3_11 * _M3_20),
                             s * (_M3_20 * _M3_01 - _M3_21 * _M3_00),
                             s * (_M3_00 * _M3_11 - _M3_01 * _M3_10));

  return TRUE;
}

#undef _M3_00
#undef _M3_01
#undef _M3_02
#undef _M3_10
#undef _M3_11
#undef _M3_12
#undef _M3_20
#undef _M3_21
#undef _M3_22

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_component vsg_matrix3@t@_component_inline
#endif
static inline @type@ vsg_matrix3@t@_component_inline (const VsgMatrix3@t@ *mat,
                                                      guint i, guint j)
{
  return VSG_MATRIX3@T@_COMP (mat, i, j);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix3@t@_transpose vsg_matrix3@t@_transpose_inline
#endif
static inline void vsg_matrix3@t@_transpose_inline (const VsgMatrix3@t@ *mat,
                                                    VsgMatrix3@t@ *result)
{
  @type@ tmp;
  int i, j;

  for (i = 0; i < 3; i++)
    {
      VSG_MATRIX3@T@_COMP (result, i, i) = VSG_MATRIX3@T@_COMP (mat, i, i);
      for (j = 0; j < i; j++)
        {
          tmp = VSG_MATRIX3@T@_COMP (mat, i, j);  /* avoid argument aliasing */
          VSG_MATRIX3@T@_COMP (result, i, j) = VSG_MATRIX3@T@_COMP (mat, j, i);
          VSG_MATRIX3@T@_COMP (result, j, i) = tmp;
        }
    }
}

G_END_DECLS;

#endif /* __VSG_MATRIX3@T@_INLINE_H__ */
