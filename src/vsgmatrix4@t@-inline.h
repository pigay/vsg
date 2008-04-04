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

#ifndef __VSG_MATRIX4@T@_INLINE_H__ /* __VSG_MATRIX4@T@_INLINE_H__ */
#define __VSG_MATRIX4@T@_INLINE_H__

#include <math.h>

#include <vsg/vsgvector3@t@-inline.h>
#include <vsg/vsgquaternion@t@-inline.h>

#include <vsg/vsgmatrix4@t@.h>

G_BEGIN_DECLS;


/* macros */

/* 
 * MAT(i,j)= components[i+4*j]
 * this is because we want to get OpenGL compliant matrices.
 */

#define VSG_MATRIX4@T@_COMP(mat, i, j) ( \
* ((mat) -> components + i + 4*j) \
)

/* public functions */

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_set vsg_matrix4@t@_set_inline
#endif
static inline void
vsg_matrix4@t@_set_inline (VsgMatrix4@t@ *mat,
                           @type@ a00, @type@ a01, @type@ a02, @type@ a03,
                           @type@ a10, @type@ a11, @type@ a12, @type@ a13,
                           @type@ a20, @type@ a21, @type@ a22, @type@ a23,
                           @type@ a30, @type@ a31, @type@ a32, @type@ a33)
{
  VSG_MATRIX4@T@_COMP (mat, 0, 0) = a00;
  VSG_MATRIX4@T@_COMP (mat, 0, 1) = a01;
  VSG_MATRIX4@T@_COMP (mat, 0, 2) = a02;
  VSG_MATRIX4@T@_COMP (mat, 0, 3) = a03;

  VSG_MATRIX4@T@_COMP (mat, 1, 0) = a10;
  VSG_MATRIX4@T@_COMP (mat, 1, 1) = a11;
  VSG_MATRIX4@T@_COMP (mat, 1, 2) = a12;
  VSG_MATRIX4@T@_COMP (mat, 1, 3) = a13;

  VSG_MATRIX4@T@_COMP (mat, 2, 0) = a20;
  VSG_MATRIX4@T@_COMP (mat, 2, 1) = a21;
  VSG_MATRIX4@T@_COMP (mat, 2, 2) = a22;
  VSG_MATRIX4@T@_COMP (mat, 2, 3) = a23;

  VSG_MATRIX4@T@_COMP (mat, 3, 0) = a30;
  VSG_MATRIX4@T@_COMP (mat, 3, 1) = a31;
  VSG_MATRIX4@T@_COMP (mat, 3, 2) = a32;
  VSG_MATRIX4@T@_COMP (mat, 3, 3) = a33;

}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_copy vsg_matrix4@t@_copy_inline
#endif
static inline void vsg_matrix4@t@_copy_inline (const VsgMatrix4@t@ *src,
                                               VsgMatrix4@t@ *dst)
{
  guint i, j;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      VSG_MATRIX4@T@_COMP (dst, i, j) = VSG_MATRIX4@T@_COMP (src, i, j) ;
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_add vsg_matrix4@t@_add_inline
#endif
static inline void vsg_matrix4@t@_add_inline (const VsgMatrix4@t@ *mat,
                                              const VsgMatrix4@t@ *other,
                                              VsgMatrix4@t@ *result)
{
  guint i, j;

  for (i = 0; i < 4; i++)
    {
      for (j = 0; j < 4; j++)
        {
          VSG_MATRIX4@T@_COMP (result, i, j) = 
            VSG_MATRIX4@T@_COMP (mat, i, j) +
            VSG_MATRIX4@T@_COMP (other, i, j);
        }
    }
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_sub vsg_matrix4@t@_sub_inline
#endif
static inline void vsg_matrix4@t@_sub_inline (const VsgMatrix4@t@ *mat,
                                              const VsgMatrix4@t@ *other,
                                              VsgMatrix4@t@ *result)
{
  guint i, j;

  for (i = 0; i < 4; i++)
    {
      for (j = 0; j < 4; j++)
        {
          VSG_MATRIX4@T@_COMP (result, i, j) = 
            VSG_MATRIX4@T@_COMP (mat, i, j) -
            VSG_MATRIX4@T@_COMP (other, i, j);
        }
    }
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_matmult vsg_matrix4@t@_matmult_inline
#endif
static inline void vsg_matrix4@t@_matmult_inline (const VsgMatrix4@t@ *mat,
                                                  const VsgMatrix4@t@ *other,
                                                  VsgMatrix4@t@ *result)
{
  VsgMatrix4@t@ tmp = VSG_M4@T@_ZERO;
  guint i, j, k;

  for (i=0; i<4; i++)
    {
      for (j=0; j<4; j++)
        {
          @type@ rtmp = 0.;

          for (k=0; k<4; k++)
            {
              rtmp += VSG_MATRIX4@T@_COMP (mat, i, k) *
                VSG_MATRIX4@T@_COMP (other, k, j);
            }

          VSG_MATRIX4@T@_COMP (&tmp, i, j) = rtmp;
        }
    }

  vsg_matrix4@t@_copy_inline (&tmp, result);

}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_vecmult vsg_matrix4@t@_vecmult_inline
#endif
static inline void vsg_matrix4@t@_vecmult_inline (const VsgMatrix4@t@ *mat,
                                                  const VsgVector3@t@ *vec,
                                                  VsgVector3@t@ *result)
{
  guint i, j;
  @type@ tmpvec[4] = {vec->x, vec->y, vec->z, 1.};
  @type@ tmpres[4] = {0., 0., 0., 0.};

  for (i=0; i<3; i++)
    {
      @type@ rtmp = 0;
      for (j=0; j<4; j++)
        {
          rtmp += VSG_MATRIX4@T@_COMP (mat, i, j) * tmpvec[j];
        }
      tmpres[i] = rtmp;
    }

  vsg_vector3@t@_set_inline (result,
                             tmpres[0],
                             tmpres[1],
                             tmpres[2]);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_vec4mult vsg_matrix4@t@_vec4mult_inline
#endif
static inline void vsg_matrix4@t@_vec4mult_inline (const VsgMatrix4@t@ *mat,
                                                   const VsgQuaternion@t@ *vec,
                                                   VsgQuaternion@t@ *result)
{
  guint i, j;

  @type@ tmpvec[4] = {vec->x, vec->y, vec->z, vec->w};
  @type@ tmpres[4] = {0., 0., 0., 0.}; /* avoid argument aliasing */

  for (i=0; i<4; i++)
    {
      @type@ rtmp = 0;
      for (j=0; j<4; j++)
        {
          rtmp += VSG_MATRIX4@T@_COMP (mat, i, j) * tmpvec[j];
        }
      tmpres[i] = rtmp;
    }

  result->x = tmpres[0];
  result->y = tmpres[1];
  result->z = tmpres[2];
  result->w = tmpres[3];
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_vec4mult_T vsg_matrix4@t@_vec4mult_T_inline
#endif
static inline void
vsg_matrix4@t@_vec4mult_T_inline (const VsgMatrix4@t@ *mat,
                                  const VsgQuaternion@t@ *vec,
                                  VsgQuaternion@t@ *result)
{
  guint i, j;

  @type@ tmpvec[4] = {vec->x, vec->y, vec->z, vec->w};
  @type@ tmpres[4] = {0., 0., 0., 0.}; /* avoid argument aliasing */

  for (i=0; i<4; i++)
    {
      @type@ rtmp = 0;
      for (j=0; j<4; j++)
        {
          rtmp += VSG_MATRIX4@T@_COMP (mat, j, i) * tmpvec[j];
        }
      tmpres[i] = rtmp;
    }

  result->x = tmpres[0];
  result->y = tmpres[1];
  result->z = tmpres[2];
  result->w = tmpres[3];
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_identity vsg_matrix4@t@_identity_inline
#endif
static inline void vsg_matrix4@t@_identity_inline (VsgMatrix4@t@ *mat)
{
  vsg_matrix4@t@_copy_inline (&VSG_M4@T@_ID, mat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_rotate_x vsg_matrix4@t@_rotate_x_inline
#endif
static inline void vsg_matrix4@t@_rotate_x_inline (VsgMatrix4@t@ *mat,
                                                   @type@ angle)
{
  VsgMatrix4@t@ rot;
  @type@ ca, sa;

  ca = cos (angle);
  sa = sin (angle);

  vsg_matrix4@t@_set_inline (&rot,
                             1., 0.,  0., 0.,
                             0., ca, -sa, 0.,
                             0., sa,  ca, 0.,
                             0., 0.,  0., 1.);

  vsg_matrix4@t@_matmult_inline (mat, &rot, mat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_rotate_y vsg_matrix4@t@_rotate_y_inline
#endif
static inline void vsg_matrix4@t@_rotate_y_inline (VsgMatrix4@t@ *mat,
                                                   @type@ angle)
{
  VsgMatrix4@t@ rot;
  @type@ ca, sa;

  ca = cos (angle);
  sa = sin (angle);

  vsg_matrix4@t@_set_inline (&rot,
                             ca, 0., -sa, 0.,
                             0., 1.,  0., 0.,
                             sa, 0.,  ca, 0.,
                             0., 0.,  0., 1.);

  vsg_matrix4@t@_matmult_inline (mat, &rot, mat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_rotate_z vsg_matrix4@t@_rotate_z_inline
#endif
static inline void vsg_matrix4@t@_rotate_z_inline (VsgMatrix4@t@ *mat,
                                                   @type@ angle)
{
  VsgMatrix4@t@ rot;
  @type@ ca, sa;

  ca = cos (angle);
  sa = sin (angle);

  vsg_matrix4@t@_set_inline (&rot,
                             ca, -sa, 0., 0.,
                             sa,  ca, 0., 0.,
                             0.,  0., 1., 0.,
                             0.,  0., 0., 1.);

  vsg_matrix4@t@_matmult_inline (mat, &rot, mat);

}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_rotate_cardan vsg_matrix4@t@_rotate_cardan_inline
#endif
static inline void vsg_matrix4@t@_rotate_cardan_inline (VsgMatrix4@t@ *mat,
                                                        @type@ ax,
                                                        @type@ ay,
                                                        @type@ az)
{
  VsgMatrix4@t@ rotx, roty, rotz;
  @type@ cax, sax;
  @type@ cay, say;
  @type@ caz, saz;

  cax = cos (ax);
  sax = sin (ax);

  cay = cos (ay);
  say = sin (ay);

  caz = cos (az);
  saz = sin (az);

  /* insert rotxyz set */
  vsg_matrix4@t@_set_inline (&rotx,
                             1., 0.,  0.,   0.,
                             0., cax, -sax, 0.,
                             0., sax, cax,  0.,
                             0., 0.,  0.,   1.
                             );

  vsg_matrix4@t@_set_inline (&roty,
                             cay, 0., -say, 0.,
                             0.,  1., 0.,   0.,
                             say, 0., cay,  0.,
                             0.,  0., 0.,   1.
                             );

  vsg_matrix4@t@_set_inline (&rotz,
                             caz, -saz, 0., 0.,
                             saz, caz,  0., 0.,
                             0.,  0.,   1., 0.,
                             0.,  0.,   0., 1.
                             );


  vsg_matrix4@t@_matmult_inline (mat, &rotz, mat);
  vsg_matrix4@t@_matmult_inline (mat, &roty, mat);
  vsg_matrix4@t@_matmult_inline (mat, &rotx, mat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_rotate_euler vsg_matrix4@t@_rotate_euler_inline
#endif
static inline void vsg_matrix4@t@_rotate_euler_inline (VsgMatrix4@t@ *mat,
                                                       @type@ alpha,
                                                       @type@ beta,
                                                       @type@ gamma)
{
  VsgMatrix4@t@ rota, rotb, rotg;
  @type@ ca, sa;
  @type@ cb, sb;
  @type@ cg, sg;

  ca = cos (alpha);
  sa = sin (alpha);

  cb = cos (beta);
  sb = sin (beta);

  cg = cos (gamma);
  sg = sin (gamma);

  /* insert rotabg set */
  vsg_matrix4@t@_set_inline (&rota,
                             ca, -sa, 0., 0.,
                             sa, ca,  0., 0.,
                             0.,  0.,   1., 0.,
                             0.,  0.,   0., 1.
                             );

  vsg_matrix4@t@_set_inline (&rotb,
                             cb, 0., -sb, 0.,
                             0.,  1., 0.,   0.,
                             sb, 0., cb,  0.,
                             0.,  0., 0.,   1.
                             );

  vsg_matrix4@t@_set_inline (&rotg,
                             cg, -sg, 0., 0.,
                             sg, cg,  0., 0.,
                             0.,  0.,   1., 0.,
                             0.,  0.,   0., 1.
                             );


  vsg_matrix4@t@_matmult_inline (mat, &rotg, mat);
  vsg_matrix4@t@_matmult_inline (mat, &rotb, mat);
  vsg_matrix4@t@_matmult_inline (mat, &rota, mat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_translate vsg_matrix4@t@_translate_inline
#endif
static inline void vsg_matrix4@t@_translate_inline (VsgMatrix4@t@ *mat,
                                                    @type@ x, @type@ y,
                                                    @type@ z)
{
  VsgMatrix4@t@ trans;

  vsg_matrix4@t@_set_inline (&trans,
                             1., 0., 0., x,
                             0., 1., 0., y,
                             0., 0., 1., z,
                             0., 0., 0., 1.);

  vsg_matrix4@t@_matmult_inline (mat, &trans, mat);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_scale vsg_matrix4@t@_scale_inline
#endif
static inline void vsg_matrix4@t@_scale_inline (VsgMatrix4@t@ *mat, @type@ x,
                                                @type@ y, @type@ z)
{
  VsgMatrix4@t@ scale;

  vsg_matrix4@t@_set_inline (&scale,
                             x, 0., 0., 0.,
                             0., y, 0., 0.,
                             0., 0., z, 0.,
                             0., 0., 0., 1.);

  vsg_matrix4@t@_matmult_inline (mat, &scale, mat);

}


#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_quaternion@t@_set vsg_matrix4@t@_quaternion@t@_set_inline
#endif
static inline void
vsg_matrix4@t@_quaternion@t@_set_inline (VsgMatrix4@t@ *mat,
                                         const VsgQuaternion@t@ *quat)
{
  /*
   * Source: http://www.j3d.org/matrix4_faq/matrfaq_latest.html
   */
  @type@ xx, xy, xz, xw, yy, yz, yw, zz, zw, ww;

  xx = quat->x * quat->x; /* g_print("xx: %@tcode@\n", xx); */
  xy = quat->x * quat->y; /* g_print("xy: %@tcode@\n", xy); */
  xz = quat->x * quat->z; /* g_print("xz: %@tcode@\n", xz); */
  xw = quat->x * quat->w; /* g_print("xw: %@tcode@\n", xw); */
  yy = quat->y * quat->y; /* g_print("yy: %@tcode@\n", yy); */
  yz = quat->y * quat->z; /* g_print("yz: %@tcode@\n", yz); */
  yw = quat->y * quat->w; /* g_print("yw: %@tcode@\n", yw); */
  zz = quat->z * quat->z; /* g_print("zz: %@tcode@\n", zz); */
  zw = quat->z * quat->w; /* g_print("zw: %@tcode@\n", zw); */
  ww = quat->w * quat->w; /* g_print("ww: %@tcode@\n", ww); */
        
  vsg_matrix4@t@_set_inline (mat,
                             1. - 2. * (yy+zz), 2. * (xy+zw), 2. * (xz-yw), 0.,
                             2. * (xy-zw), 1. - 2. * (xx+zz), 2. * (yz+xw), 0.,
                             2. * (xz+yw), 2. * (yz-xw), 1. - 2. * (xx+yy), 0.,
                             0., 0., 0., 1.);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_quaternion@t@ vsg_matrix4@t@_quaternion@t@_inline
#endif
static inline void
vsg_matrix4@t@_quaternion@t@_inline (VsgMatrix4@t@ *mat,
                                     const VsgQuaternion@t@ *quat)
{
  VsgMatrix4@t@ qrot;

  vsg_matrix4@t@_quaternion@t@_set_inline (&qrot, quat);
  vsg_matrix4@t@_matmult_inline (mat, &qrot, mat);
}

/* inspired from www.nebula.org */

#define _M4_00 VSG_MATRIX4@T@_COMP(mat,0,0)
#define _M4_01 VSG_MATRIX4@T@_COMP(mat,0,1)
#define _M4_02 VSG_MATRIX4@T@_COMP(mat,0,2)
#define _M4_03 VSG_MATRIX4@T@_COMP(mat,0,3)
#define _M4_10 VSG_MATRIX4@T@_COMP(mat,1,0)
#define _M4_11 VSG_MATRIX4@T@_COMP(mat,1,1)
#define _M4_12 VSG_MATRIX4@T@_COMP(mat,1,2)
#define _M4_13 VSG_MATRIX4@T@_COMP(mat,1,3)
#define _M4_20 VSG_MATRIX4@T@_COMP(mat,2,0)
#define _M4_21 VSG_MATRIX4@T@_COMP(mat,2,1)
#define _M4_22 VSG_MATRIX4@T@_COMP(mat,2,2)
#define _M4_23 VSG_MATRIX4@T@_COMP(mat,2,3)
#define _M4_30 VSG_MATRIX4@T@_COMP(mat,3,0)
#define _M4_31 VSG_MATRIX4@T@_COMP(mat,3,1)
#define _M4_32 VSG_MATRIX4@T@_COMP(mat,3,2)
#define _M4_33 VSG_MATRIX4@T@_COMP(mat,3,3)

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_det vsg_matrix4@t@_det_inline
#endif
static inline @type@ vsg_matrix4@t@_det_inline (const VsgMatrix4@t@ *mat)
{
  return
    (_M4_00*_M4_11 - _M4_01*_M4_10)*(_M4_22*_M4_33 - _M4_23*_M4_32)
    -(_M4_00*_M4_12 - _M4_02*_M4_10)*(_M4_21*_M4_33 - _M4_23*_M4_31)
    +(_M4_00*_M4_13 - _M4_03*_M4_10)*(_M4_21*_M4_32 - _M4_22*_M4_31)
    +(_M4_01*_M4_12 - _M4_02*_M4_11)*(_M4_20*_M4_33 - _M4_23*_M4_30)
    -(_M4_01*_M4_13 - _M4_03*_M4_11)*(_M4_20*_M4_32 - _M4_22*_M4_30)
    +(_M4_02*_M4_13 - _M4_03*_M4_12)*(_M4_20*_M4_31 - _M4_21*_M4_30);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_invert vsg_matrix4@t@_invert_inline
#endif
static inline gboolean vsg_matrix4@t@_invert_inline (const VsgMatrix4@t@ *mat,
                                                     VsgMatrix4@t@ *result)
{
  @type@ s;

  s = vsg_matrix4@t@_det_inline (mat);

  if (s == 0.0)
    {
      g_warning ("invalid VsgMatrix4@t@ for inversion. Skipping.\n");
      return FALSE;
    }

  s = 1. / s;
  vsg_matrix4@t@_set_inline ( result,
                              s * (_M4_11*(_M4_22*_M4_33 - _M4_23*_M4_32) + _M4_12*(_M4_23*_M4_31 - _M4_21*_M4_33) + _M4_13*(_M4_21*_M4_32 - _M4_22*_M4_31)),
                              s * (_M4_21*(_M4_02*_M4_33 - _M4_03*_M4_32) + _M4_22*(_M4_03*_M4_31 - _M4_01*_M4_33) + _M4_23*(_M4_01*_M4_32 - _M4_02*_M4_31)),
                              s * (_M4_31*(_M4_02*_M4_13 - _M4_03*_M4_12) + _M4_32*(_M4_03*_M4_11 - _M4_01*_M4_13) + _M4_33*(_M4_01*_M4_12 - _M4_02*_M4_11)),
                              s * (_M4_01*(_M4_13*_M4_22 - _M4_12*_M4_23) + _M4_02*(_M4_11*_M4_23 - _M4_13*_M4_21) + _M4_03*(_M4_12*_M4_21 - _M4_11*_M4_22)),
                              s * (_M4_12*(_M4_20*_M4_33 - _M4_23*_M4_30) + _M4_13*(_M4_22*_M4_30 - _M4_20*_M4_32) + _M4_10*(_M4_23*_M4_32 - _M4_22*_M4_33)),
                              s * (_M4_22*(_M4_00*_M4_33 - _M4_03*_M4_30) + _M4_23*(_M4_02*_M4_30 - _M4_00*_M4_32) + _M4_20*(_M4_03*_M4_32 - _M4_02*_M4_33)),
                              s * (_M4_32*(_M4_00*_M4_13 - _M4_03*_M4_10) + _M4_33*(_M4_02*_M4_10 - _M4_00*_M4_12) + _M4_30*(_M4_03*_M4_12 - _M4_02*_M4_13)),
                              s * (_M4_02*(_M4_13*_M4_20 - _M4_10*_M4_23) + _M4_03*(_M4_10*_M4_22 - _M4_12*_M4_20) + _M4_00*(_M4_12*_M4_23 - _M4_13*_M4_22)),
                              s * (_M4_13*(_M4_20*_M4_31 - _M4_21*_M4_30) + _M4_10*(_M4_21*_M4_33 - _M4_23*_M4_31) + _M4_11*(_M4_23*_M4_30 - _M4_20*_M4_33)),
                              s * (_M4_23*(_M4_00*_M4_31 - _M4_01*_M4_30) + _M4_20*(_M4_01*_M4_33 - _M4_03*_M4_31) + _M4_21*(_M4_03*_M4_30 - _M4_00*_M4_33)),
                              s * (_M4_33*(_M4_00*_M4_11 - _M4_01*_M4_10) + _M4_30*(_M4_01*_M4_13 - _M4_03*_M4_11) + _M4_31*(_M4_03*_M4_10 - _M4_00*_M4_13)),
                              s * (_M4_03*(_M4_11*_M4_20 - _M4_10*_M4_21) + _M4_00*(_M4_13*_M4_21 - _M4_11*_M4_23) + _M4_01*(_M4_10*_M4_23 - _M4_13*_M4_20)),
                              s * (_M4_10*(_M4_22*_M4_31 - _M4_21*_M4_32) + _M4_11*(_M4_20*_M4_32 - _M4_22*_M4_30) + _M4_12*(_M4_21*_M4_30 - _M4_20*_M4_31)),
                              s * (_M4_20*(_M4_02*_M4_31 - _M4_01*_M4_32) + _M4_21*(_M4_00*_M4_32 - _M4_02*_M4_30) + _M4_22*(_M4_01*_M4_30 - _M4_00*_M4_31)),
                              s * (_M4_30*(_M4_02*_M4_11 - _M4_01*_M4_12) + _M4_31*(_M4_00*_M4_12 - _M4_02*_M4_10) + _M4_32*(_M4_01*_M4_10 - _M4_00*_M4_11)),
                              s * (_M4_00*(_M4_11*_M4_22 - _M4_12*_M4_21) + _M4_01*(_M4_12*_M4_20 - _M4_10*_M4_22) + _M4_02*(_M4_10*_M4_21 - _M4_11*_M4_20)));

  return TRUE;
}

#undef _M4_00
#undef _M4_01
#undef _M4_02
#undef _M4_03
#undef _M4_10
#undef _M4_11
#undef _M4_12
#undef _M4_13
#undef _M4_20
#undef _M4_21
#undef _M4_22
#undef _M4_23
#undef _M4_30
#undef _M4_31
#undef _M4_32
#undef _M4_33

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_component vsg_matrix4@t@_component_inline
#endif
static inline @type@ vsg_matrix4@t@_component_inline (const VsgMatrix4@t@ *mat,
                                                      guint i, guint j)
{
  return VSG_MATRIX4@T@_COMP (mat, i, j);
}

#ifndef VSG_NO_IMPLICIT_INLINE
#define vsg_matrix4@t@_transpose vsg_matrix4@t@_transpose_inline
#endif
static inline void vsg_matrix4@t@_transpose_inline (const VsgMatrix4@t@ *mat,
                                                    VsgMatrix4@t@ *result)
{
  @type@ tmp;
  int i, j;

  for (i=0; i<4; i++)
    {
      VSG_MATRIX4@T@_COMP (result, i, i) = VSG_MATRIX4@T@_COMP (mat, i, i);
      for (j=0; j<i; j++)
        {
          tmp = VSG_MATRIX4@T@_COMP (mat, i, j);
          VSG_MATRIX4@T@_COMP (result, i, j) = VSG_MATRIX4@T@_COMP (mat, j, i);
          VSG_MATRIX4@T@_COMP (result, j, i) = tmp;
        }
    }
}

G_END_DECLS;

#endif /* __VSG_MATRIX4@T@_INLINE_H__ */
