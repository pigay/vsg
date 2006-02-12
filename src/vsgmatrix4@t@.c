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

#include "vsg-config.h"

#include "vsgmatrix4@t@.h"
#include "vsgquaternion@t@.h"

#include <math.h>

#include <glib/gprintf.h>

/* 
 * MAT(i,j)= components[i+4*j]
 * this is because we want to get OpenGL compliant matrices.
 */

#define VSG_MATRIX4@T@_COMP(mat, i, j) ( \
* ((mat) -> components + i + 4*j) \
)

#define VSG_MATRIX4@T@_PREALLOC 128

static const @type@ degrad = M_PI / 180.;

static GMemChunk *vsg_matrix4@t@_mem_chunk = 0;
static guint vsg_matrix4@t@_instances_count = 0;

static void vsg_matrix4@t@_finalize ();
static VsgMatrix4@t@ *_matrix4@t@_alloc ();

static void vsg_matrix4@t@_finalize ()
{
  if (vsg_matrix4@t@_mem_chunk)
    {
      g_mem_chunk_destroy (vsg_matrix4@t@_mem_chunk);
      vsg_matrix4@t@_mem_chunk = 0;
    }
}

static VsgMatrix4@t@ *_matrix4@t@_alloc ()
{
  if (!vsg_matrix4@t@_mem_chunk)
    {
      vsg_matrix4@t@_mem_chunk = g_mem_chunk_create (VsgMatrix4@t@,
                                                        VSG_MATRIX4@T@_PREALLOC,
                                                        G_ALLOC_ONLY);
    }

  vsg_matrix4@t@_instances_count ++;

  return g_chunk_new (VsgMatrix4@t@, vsg_matrix4@t@_mem_chunk);
}

/* private function */
void vsg_matrix4@t@_init ()
{
  static gboolean wasinit = FALSE;

  if (! wasinit)
    {
      wasinit = TRUE;
      g_atexit (vsg_matrix4@t@_finalize);
    }
}

/* public functions */

/**
 * VSG_TYPE_MATRIX4@T@:
 *
 * The #GBoxed #GType associated to #VsgMatrix4@t@.
 */

GType vsg_matrix4@t@_get_type (void)
{
  static GType matrix4@t@_type = 0;

  if (!matrix4@t@_type)
    {
      matrix4@t@_type =
        g_boxed_type_register_static ("VsgMatrix4@t@",
                                      (GBoxedCopyFunc) vsg_matrix4@t@_clone,
                                      (GBoxedFreeFunc) vsg_matrix4@t@_free);

    }

  return matrix4@t@_type;
}

/**
 * vsg_matrix4@t@_new:
 * @a00: a #@type@
 * @a01: a #@type@
 * @a02: a #@type@
 * @a03: a #@type@
 * @a10: a #@type@
 * @a11: a #@type@
 * @a12: a #@type@
 * @a13: a #@type@
 * @a20: a #@type@
 * @a21: a #@type@
 * @a22: a #@type@
 * @a23: a #@type@
 * @a30: a #@type@
 * @a31: a #@type@
 * @a32: a #@type@
 * @a33: a #@type@
 *
 * Allocates a new #VsgMatrix4@t@. Arguments specify new matrix components.
 *
 * Returns: new #VsgMatrix4@t@ instance
 */
VsgMatrix4@t@ *
vsg_matrix4@t@_new (@type@ a00, @type@ a01, @type@ a02, @type@ a03,
                    @type@ a10, @type@ a11, @type@ a12, @type@ a13,
                    @type@ a20, @type@ a21, @type@ a22, @type@ a23,
                    @type@ a30, @type@ a31, @type@ a32, @type@ a33)
{
  VsgMatrix4@t@ *result = _matrix4@t@_alloc ();

  vsg_matrix4@t@_set (result,
                      a00, a01, a02, a03,
                      a10, a11, a12, a13,
                      a20, a21, a22, a23,
                      a30, a31, a32, a33);

  return result;
}

/**
 * vsg_matrix4@t@_free:
 * @mat: a #VsgMatrix4@t@
 *
 * Deallocates @mat.
 */
void vsg_matrix4@t@_free (VsgMatrix4@t@ *mat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat!=NULL);
#endif

  g_chunk_free (mat, vsg_matrix4@t@_mem_chunk);

  vsg_matrix4@t@_instances_count --;

  if (vsg_matrix4@t@_instances_count == 0)
    vsg_matrix4@t@_finalize ();
}

/**
 * vsg_matrix4@t@_identity_new:
 *
 * Allocates a new #VsgMatrix4@t@ with Identity component (%1 on the diagonal,
 * %0 elsewhere).
 *
 * Returns: new #VsgMatrix4@t@ instance
 */
VsgMatrix4@t@ *vsg_matrix4@t@_identity_new ()
{
  VsgMatrix4@t@ *result = _matrix4@t@_alloc ();

  vsg_matrix4@t@_identity (result);

  return result;
}

/**
 * vsg_matrix4@t@_rotate_cardan_new:
 * @ax: a #@type@
 * @ay: a #@type@
 * @az: a #@type@
 *
 * Creates a new #VsgMatrix4@t@ corresponding to a 3D rotation of Cardan angles
 * @ax, @ay, @az.
 *
 * Returns: new #VsgMatrix4@t@ instance
 */
VsgMatrix4@t@ *vsg_matrix4@t@_rotate_cardan_new (@type@ ax,
                                                 @type@ ay,
                                                 @type@ az)
{
  VsgMatrix4@t@ *result = vsg_matrix4@t@_identity_new ();

  vsg_matrix4@t@_rotate_cardan (result, ax, ay, az);

  return result;
}

/**
 * vsg_matrix4@t@_rotate_euler_new:
 * @alpha: a #@type@
 * @beta: a #@type@
 * @gamma: a #@type@
 *
 * Creates a new #VsgMatrix4@t@ corresponding to a 3D rotation of Euler angles
 * @alpha, @beta, @gamma in ZYZ convetion.
 * 
 *
 * Returns: new #VsgMatrix4@t@ instance
 */
VsgMatrix4@t@ *vsg_matrix4@t@_rotate_euler_new (@type@ alpha,
                                                @type@ beta,
                                                @type@ gamma)
{
  VsgMatrix4@t@ *result = vsg_matrix4@t@_identity_new ();

  vsg_matrix4@t@_rotate_euler (result, alpha, beta, gamma);

  return result;
}

/**
 * vsg_matrix4@t@_translate_new:
 * @x: a #@type@
 * @y: a #@type@
 * @z: a #@type@
 *
 * Creates a new #VsgMatrix4@t@ corresponding to a 3D translation with
 * coordinates (@x,@y,@z).
 *
 * Returns: new #VsgMatrix4@t@ instance
 */
VsgMatrix4@t@ *vsg_matrix4@t@_translate_new (@type@ x, @type@ y, @type@ z)
{
  VsgMatrix4@t@ *result = vsg_matrix4@t@_identity_new ();

  vsg_matrix4@t@_translate (result, x, y, z);

  return result;

}

/**
 * vsg_matrix4@t@_scale_new:
 * @x: a #@type@
 * @y: a #@type@
 * @z: a #@type@
 *
 * Creates a new #VsgMatrix4@t@ corresponding to a 3D scaling with
 * coordinates (@x,@y,@z).
 *
 * Returns: new #VsgMatrix4@t@ instance
 */
VsgMatrix4@t@ *vsg_matrix4@t@_scale_new (@type@ x, @type@ y, @type@ z)
{
  VsgMatrix4@t@ *result = vsg_matrix4@t@_identity_new ();

  vsg_matrix4@t@_scale(result, x, y, z);

  return result;
}

/**
 * vsg_matrix4@t@_quaternion@t@_new:
 * @quat: a #VsgQuaternion@t@
 *
 * Creates a new #VsgMatrix4@t@ corresponding to a 3D quaternion 
 * rotation
 *
 * Returns: new #VsgMatrix4@t@ instance
 */
VsgMatrix4@t@ *vsg_matrix4@t@_quaternion@t@_new (const VsgQuaternion@t@ *quat)
{
  VsgMatrix4@t@ *result;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (quat!=NULL, NULL);
#endif

  result = vsg_matrix4@t@_identity_new ();

  vsg_matrix4@t@_quaternion@t@ (result,quat);

  return result;

}

/**
 * vsg_matrix4@t@_set:
 * @mat: a #VsgMatrix4@t@
 * @a00: a #@type@
 * @a01: a #@type@
 * @a02: a #@type@
 * @a03: a #@type@
 * @a10: a #@type@
 * @a11: a #@type@
 * @a12: a #@type@
 * @a13: a #@type@
 * @a20: a #@type@
 * @a21: a #@type@
 * @a22: a #@type@
 * @a23: a #@type@
 * @a30: a #@type@
 * @a31: a #@type@
 * @a32: a #@type@
 * @a33: a #@type@
 *
 * Sets components of @mat to values specified by other arguments.
 */
void vsg_matrix4@t@_set (VsgMatrix4@t@ *mat,
                         @type@ a00, @type@ a01, @type@ a02, @type@ a03,
                         @type@ a10, @type@ a11, @type@ a12, @type@ a13,
                         @type@ a20, @type@ a21, @type@ a22, @type@ a23,
                         @type@ a30, @type@ a31, @type@ a32, @type@ a33)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

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

/**
 * vsg_matrix4@t@_identity:
 * @mat: a #VsgMatrix4@t@
 *
 * Sets components of @mat to values of Identity.
 */
void vsg_matrix4@t@_identity (VsgMatrix4@t@ *mat)
{
  vsg_matrix4@t@_copy (&VSG_M4@T@_ID, mat);
}

/**
 * vsg_matrix4@t@_copy:
 * @dst: a #VsgMatrix4@t@
 * @src: a #VsgMatrix4@t@
 *
 * Copies @src to @dst.
 */
void vsg_matrix4@t@_copy (const VsgMatrix4@t@ *src, VsgMatrix4@t@ *dst)
{
  guint i, j;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (dst != NULL);
  g_return_if_fail (src != NULL);
#endif

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      VSG_MATRIX4@T@_COMP (dst, i, j) = VSG_MATRIX4@T@_COMP (src, i, j) ;
}

/**
 * vsg_matrix4@t@_clone:
 * @src: a #VsgMatrix4@t@.
 *
 * Duplicates @src.
 *
 * Returns: a copy of @src.
 */
VsgMatrix4@t@ *vsg_matrix4@t@_clone (const VsgMatrix4@t@ *src)
{
  VsgMatrix4@t@ *dst;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (src != NULL, NULL);
#endif

  dst = _matrix4@t@_alloc ();

  vsg_matrix4@t@_copy (src, dst);

  return dst;
}

/**
 * vsg_matrix4@t@_rotate_x:
 * @mat: a #VsgMatrix4@t@
 * @angle: a #@type@.
 *
 * Performs a rotation of angle @angle about X axis on matrix @mat.
 */
void vsg_matrix4@t@_rotate_x (VsgMatrix4@t@ *mat, @type@ angle)
{
  VsgMatrix4@t@ rot;
  @type@ ca, sa;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  ca = cos (angle);
  sa = sin (angle);

  vsg_matrix4@t@_set (&rot,
                      1., 0.,  0., 0.,
                      0., ca, -sa, 0.,
                      0., sa,  ca, 0.,
                      0., 0.,  0., 1.);

  vsg_matrix4@t@_matmult (mat, &rot, mat);
}

/**
 * vsg_matrix4@t@_rotate_y:
 * @mat: a #VsgMatrix4@t@
 * @angle: a #@type@.
 *
 * Performs a rotation of angle @angle about Y axis on matrix @mat.
 */
void vsg_matrix4@t@_rotate_y (VsgMatrix4@t@ *mat, @type@ angle)
{
  VsgMatrix4@t@ rot;
  @type@ ca, sa;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  ca = cos (angle);
  sa = sin (angle);

  vsg_matrix4@t@_set (&rot,
                      ca, 0., -sa, 0.,
                      0., 1.,  0., 0.,
                      sa, 0.,  ca, 0.,
                      0., 0.,  0., 1.);

  vsg_matrix4@t@_matmult (mat, &rot, mat);
}

/**
 * vsg_matrix4@t@_rotate_z:
 * @mat: a #VsgMatrix4@t@
 * @angle: a #@type@.
 *
 * Performs a rotation of angle @angle about Z axis on matrix @mat.
 */
void vsg_matrix4@t@_rotate_z (VsgMatrix4@t@ *mat, @type@ angle)
{
  VsgMatrix4@t@ rot;
  @type@ ca, sa;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  ca = cos (angle);
  sa = sin (angle);

  vsg_matrix4@t@_set (&rot,
                      ca, -sa, 0., 0.,
                      sa,  ca, 0., 0.,
                      0.,  0., 1., 0.,
                      0.,  0., 0., 1.);

  vsg_matrix4@t@_matmult (mat, &rot, mat);

}

/**
 * vsg_matrix4@t@_rotate_cardan:
 * @mat: a #VsgMatrix4@t@
 * @ax: a #@type@
 * @ay: a #@type@
 * @az: a #@type@
 *
 * Performs a 3D rotation of Cardan angles @ax, @ay, @az (ie. Cardano
 * convention) on matrix @mat.
 */
void vsg_matrix4@t@_rotate_cardan (VsgMatrix4@t@ *mat,
                                   @type@ ax,
                                   @type@ ay,
                                   @type@ az)
{
  VsgMatrix4@t@ rotx, roty, rotz;
  @type@ cax, sax;
  @type@ cay, say;
  @type@ caz, saz;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  cax = cos (ax);
  sax = sin (ax);

  cay = cos (ay);
  say = sin (ay);

  caz = cos (az);
  saz = sin (az);

  /* insert rotxyz set */
  vsg_matrix4@t@_set (&rotx,
                      1., 0.,  0.,   0.,
                      0., cax, -sax, 0.,
                      0., sax, cax,  0.,
                      0., 0.,  0.,   1.
                      );

  vsg_matrix4@t@_set (&roty,
                      cay, 0., -say, 0.,
                      0.,  1., 0.,   0.,
                      say, 0., cay,  0.,
                      0.,  0., 0.,   1.
                      );

  vsg_matrix4@t@_set (&rotz,
                      caz, -saz, 0., 0.,
                      saz, caz,  0., 0.,
                      0.,  0.,   1., 0.,
                      0.,  0.,   0., 1.
                      );


  vsg_matrix4@t@_matmult (mat, &rotz, mat);
  vsg_matrix4@t@_matmult (mat, &roty, mat);
  vsg_matrix4@t@_matmult (mat, &rotx, mat);
}

/**
 * vsg_matrix4@t@_rotate_euler:
 * @mat: a #VsgMatrix4@t@
 * @alpha: a #@type@
 * @beta: a #@type@
 * @gamma: a #@type@
 *
 * Performs a 3D rotation of Euler angles  (ie. ZYZ convention) on matrix
 * @mat.
 */
void vsg_matrix4@t@_rotate_euler (VsgMatrix4@t@ *mat,
                                  @type@ alpha,
                                  @type@ beta,
                                  @type@ gamma)
{
  VsgMatrix4@t@ rota, rotb, rotg;
  @type@ ca, sa;
  @type@ cb, sb;
  @type@ cg, sg;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  ca = cos (alpha);
  sa = sin (alpha);

  cb = cos (beta);
  sb = sin (beta);

  cg = cos (gamma);
  sg = sin (gamma);

  /* insert rotabg set */
  vsg_matrix4@t@_set (&rota,
                      ca, -sa, 0., 0.,
                      sa, ca,  0., 0.,
                      0.,  0.,   1., 0.,
                      0.,  0.,   0., 1.
                      );

  vsg_matrix4@t@_set (&rotb,
                      cb, 0., -sb, 0.,
                      0.,  1., 0.,   0.,
                      sb, 0., cb,  0.,
                      0.,  0., 0.,   1.
                      );

  vsg_matrix4@t@_set (&rotg,
                      cg, -sg, 0., 0.,
                      sg, cg,  0., 0.,
                      0.,  0.,   1., 0.,
                      0.,  0.,   0., 1.
                      );


  vsg_matrix4@t@_matmult (mat, &rotg, mat);
  vsg_matrix4@t@_matmult (mat, &rotb, mat);
  vsg_matrix4@t@_matmult (mat, &rota, mat);
}

/**
 * vsg_matrix4@t@_translate:
 * @mat: a #VsgMatrix4@t@
 * @x: a #@type@
 * @y: a #@type@
 * @z: a #@type@
 *
 * Performs a 3D translation of coordinates (@x,@y,@z) on matrix @mat.
 */
void vsg_matrix4@t@_translate (VsgMatrix4@t@ *mat,
                               @type@ x, @type@ y, @type@ z)
{
  VsgMatrix4@t@ trans;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix4@t@_set (&trans,
                      1., 0., 0., x,
                      0., 1., 0., y,
                      0., 0., 1., z,
                      0., 0., 0., 1.);

  vsg_matrix4@t@_matmult (mat, &trans, mat);
}

/**
 * vsg_matrix4@t@_scale:
 * @mat: a #VsgMatrix4@t@
 * @x: a #@type@
 * @y: a #@type@
 * @z: a #@type@
 *
 * Performs a 3D scaling of coordinates (@x,@y,@z) on matrix @mat.
 */
void vsg_matrix4@t@_scale (VsgMatrix4@t@ *mat, @type@ x, @type@ y, @type@ z)
{
  VsgMatrix4@t@ scale;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix4@t@_set (&scale,
                      x, 0., 0., 0.,
                      0., y, 0., 0.,
                      0., 0., z, 0.,
                      0., 0., 0., 1.);

  vsg_matrix4@t@_matmult (mat, &scale, mat);

}


/*
 * Source: http://www.j3d.org/matrix4_faq/matrfaq_latest.html
 */
/**
 * vsg_matrix4@t@_quaternion@t@_set:
 * @mat: a #VsgMatrix4@t@
 * @quat: a #VsgQuaternion@t@
 *
 * Sets the components of @mat to the rotation represented by @quat.
 */
void vsg_matrix4@t@_quaternion@t@_set (VsgMatrix4@t@ *mat,
                                       const VsgQuaternion@t@ *quat)
{
  @type@ xx, xy, xz, xw, yy, yz, yw, zz, zw, ww;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (quat != NULL);
#endif

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
        
  vsg_matrix4@t@_set (mat,
                      1. - 2. * (yy+zz), 2. * (xy+zw), 2. * (xz-yw), 0.,
                      2. * (xy-zw), 1. - 2. * (xx+zz), 2. * (yz+xw), 0.,
                      2. * (xz+yw), 2. * (yz-xw), 1. - 2. * (xx+yy), 0.,
                      0., 0., 0., 1.);
}

/**
 * vsg_matrix4@t@_quaternion@t@:
 * @mat: a #VsgMatrix4@t@.
 * @quat: a #VsgQuaternion@t@.
 *
 * Right multiplies @mat by the rotation matrix associated to @quat.
 */
void vsg_matrix4@t@_quaternion@t@ (VsgMatrix4@t@ *mat,
                                   const VsgQuaternion@t@ *quat)
{
  VsgMatrix4@t@ qrot;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (quat != NULL);
#endif

  vsg_matrix4@t@_quaternion@t@_set (&qrot, quat);
  vsg_matrix4@t@_matmult (mat, &qrot, mat);
}

/**
 * vsg_matrix4@t@_add:
 * @mat: a #VsgMatrix4@t@
 * @other: a #VsgMatrix4@t@
 * @result: a #VsgMatrix4@t@
 *
 * Performs matrix addition between @mat and @other and stores
 * the result in @result. Argument aliasing is allowed.
 */
void vsg_matrix4@t@_add (const VsgMatrix4@t@ *mat,
                         const VsgMatrix4@t@ *other,
                         VsgMatrix4@t@ *result)
{
  guint i, j;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

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

/**
 * vsg_matrix4@t@_sub:
 * @mat: a #VsgMatrix4@t@
 * @other: a #VsgMatrix4@t@
 * @result: a #VsgMatrix4@t@
 *
 * Performs matrix subtraction between @mat and @other and stores
 * the result in @result. Argument aliasing is allowed.
 */
void vsg_matrix4@t@_sub (const VsgMatrix4@t@ *mat,
                         const VsgMatrix4@t@ *other,
                         VsgMatrix4@t@ *result)
{
  guint i, j;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

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

/**
 * vsg_matrix4@t@_matmult:
 * @mat: a #VsgMatrix4@t@
 * @other: a #VsgMatrix4@t@
 * @result: a #VsgMatrix4@t@
 *
 * Performs matrix product of @mat by @other and stores the result in
 * @result. Argument aliasing is allowed.
 */
void vsg_matrix4@t@_matmult (const VsgMatrix4@t@ *mat,
                             const VsgMatrix4@t@ *other,
                             VsgMatrix4@t@ *result)
{
  VsgMatrix4@t@ tmp = VSG_M4@T@_ZERO;
  guint i, j, k;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

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

  vsg_matrix4@t@_copy (&tmp, result);

}

/**
 * vsg_matrix4@t@_vecmult:
 * @mat: a #VsgMatrix4@t@
 * @vec: a #VsgVector3@t@
 * @result: a #VsgVector3@t@
 *
 * Performs matrix-vector product of @mat by @vec and stores the result
 * in @result. Argument aliasing is allowed.
 */
void vsg_matrix4@t@_vecmult (const VsgMatrix4@t@ *mat,
                             const VsgVector3@t@ *vec,
                             VsgVector3@t@ *result)
{
  guint i, j;
  @type@ tmpvec[4] = {vec->x, vec->y, vec->z, 1.};
  @type@ tmpres[4] = {0., 0., 0., 0.};

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (vec != NULL);
  g_return_if_fail (result != NULL);
#endif

  for (i=0; i<3; i++)
    {
      @type@ rtmp = 0;
      for (j=0; j<4; j++)
        {
          rtmp += VSG_MATRIX4@T@_COMP (mat, i, j) * tmpvec[j];
        }
      tmpres[i] = rtmp;
    }

  vsg_vector3@t@_set (result,
                      tmpres[0],
                      tmpres[1],
                      tmpres[2]);
}

/**
 * vsg_matrix4@t@_vec4mult:
 * @mat: a #VsgMatrix4@t@.
 * @vec: a #VsgQuaternion@t@.
 * @result: a #VsgQuaternion@t@.
 *
 * Performs matrix-vector product of @mat by @vec and stores the result
 * in @result. Argument aliasing is allowed.
 */
void vsg_matrix4@t@_vec4mult (const VsgMatrix4@t@ *mat,
                              const VsgQuaternion@t@ *vec,
                              VsgQuaternion@t@ *result)
{
  guint i, j;

  @type@ tmpvec[4] = {vec->x, vec->y, vec->z, vec->w};
  @type@ tmpres[4] = {0., 0., 0., 0.}; /* avoid argument aliasing */

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (vec != NULL);
  g_return_if_fail (result != NULL);
#endif

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

/**
 * vsg_matrix4@t@_vec4mult_T:
 * @mat: a #VsgMatrix4@t@.
 * @vec: a #VsgQuaternion@t@.
 * @result: a #VsgQuaternion@t@.
 *
 * Performs matrix-vector product of transpose(@mat) by @vec and stores
 * the result in @result. Argument aliasing is allowed.
 */
void vsg_matrix4@t@_vec4mult_T (const VsgMatrix4@t@ *mat,
                                const VsgQuaternion@t@ *vec,
                                VsgQuaternion@t@ *result)
{
  guint i, j;

  @type@ tmpvec[4] = {vec->x, vec->y, vec->z, vec->w};
  @type@ tmpres[4] = {0., 0., 0., 0.}; /* avoid argument aliasing */

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (vec != NULL);
  g_return_if_fail (result != NULL);
#endif

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

/* inspired from www.nebula.org */

#define C00 VSG_MATRIX4@T@_COMP(mat,0,0)
#define C01 VSG_MATRIX4@T@_COMP(mat,0,1)
#define C02 VSG_MATRIX4@T@_COMP(mat,0,2)
#define C03 VSG_MATRIX4@T@_COMP(mat,0,3)
#define C10 VSG_MATRIX4@T@_COMP(mat,1,0)
#define C11 VSG_MATRIX4@T@_COMP(mat,1,1)
#define C12 VSG_MATRIX4@T@_COMP(mat,1,2)
#define C13 VSG_MATRIX4@T@_COMP(mat,1,3)
#define C20 VSG_MATRIX4@T@_COMP(mat,2,0)
#define C21 VSG_MATRIX4@T@_COMP(mat,2,1)
#define C22 VSG_MATRIX4@T@_COMP(mat,2,2)
#define C23 VSG_MATRIX4@T@_COMP(mat,2,3)
#define C30 VSG_MATRIX4@T@_COMP(mat,3,0)
#define C31 VSG_MATRIX4@T@_COMP(mat,3,1)
#define C32 VSG_MATRIX4@T@_COMP(mat,3,2)
#define C33 VSG_MATRIX4@T@_COMP(mat,3,3)

/**
 * vsg_matrix4@t@_det:
 * @mat: a #VsgMatrix4@t@
 *
 * Computes determinant of @mat.
 *
 * Returns: the determinant of @mat
 */
@type@ vsg_matrix4@t@_det (const VsgMatrix4@t@ *mat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (mat != NULL, 0.);
#endif

  return
    (C00*C11 - C01*C10)*(C22*C33 - C23*C32)
    -(C00*C12 - C02*C10)*(C21*C33 - C23*C31)
    +(C00*C13 - C03*C10)*(C21*C32 - C22*C31)
    +(C01*C12 - C02*C11)*(C20*C33 - C23*C30)
    -(C01*C13 - C03*C11)*(C20*C32 - C22*C30)
    +(C02*C13 - C03*C12)*(C20*C31 - C21*C30);
}

/**
 * vsg_matrix4@t@_invert:
 * @mat: a #VsgMatrix4@t@
 * @result: a #VsgMatrix4@t@
 *
 * Computes matrix inversion of @mat and stores the result in @result. Argument
 * aliasing is allowed.
 *
 * Returns: a flag indicating success of the inversion.
 */
gboolean vsg_matrix4@t@_invert (const VsgMatrix4@t@ *mat,
                                VsgMatrix4@t@ *result)
{
  @type@ s;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (mat != NULL, 0.);
  g_return_val_if_fail (result != NULL, 0.);
#endif

  s = vsg_matrix4@t@_det (mat);

  if (s == 0.0)
    {
      g_warning ("invalid VsgMatrix4@t@ for inversion. Skipping.\n");
      return FALSE;
    }

  s = 1/s;
  vsg_matrix4@t@_set ( result,
                       s*(C11*(C22*C33 - C23*C32) + C12*(C23*C31 - C21*C33) + C13*(C21*C32 - C22*C31)),
                       s*(C21*(C02*C33 - C03*C32) + C22*(C03*C31 - C01*C33) + C23*(C01*C32 - C02*C31)),
                       s*(C31*(C02*C13 - C03*C12) + C32*(C03*C11 - C01*C13) + C33*(C01*C12 - C02*C11)),
                       s*(C01*(C13*C22 - C12*C23) + C02*(C11*C23 - C13*C21) + C03*(C12*C21 - C11*C22)),
                       s*(C12*(C20*C33 - C23*C30) + C13*(C22*C30 - C20*C32) + C10*(C23*C32 - C22*C33)),
                       s*(C22*(C00*C33 - C03*C30) + C23*(C02*C30 - C00*C32) + C20*(C03*C32 - C02*C33)),
                       s*(C32*(C00*C13 - C03*C10) + C33*(C02*C10 - C00*C12) + C30*(C03*C12 - C02*C13)),
                       s*(C02*(C13*C20 - C10*C23) + C03*(C10*C22 - C12*C20) + C00*(C12*C23 - C13*C22)),
                       s*(C13*(C20*C31 - C21*C30) + C10*(C21*C33 - C23*C31) + C11*(C23*C30 - C20*C33)),
                       s*(C23*(C00*C31 - C01*C30) + C20*(C01*C33 - C03*C31) + C21*(C03*C30 - C00*C33)),
                       s*(C33*(C00*C11 - C01*C10) + C30*(C01*C13 - C03*C11) + C31*(C03*C10 - C00*C13)),
                       s*(C03*(C11*C20 - C10*C21) + C00*(C13*C21 - C11*C23) + C01*(C10*C23 - C13*C20)),
                       s*(C10*(C22*C31 - C21*C32) + C11*(C20*C32 - C22*C30) + C12*(C21*C30 - C20*C31)),
                       s*(C20*(C02*C31 - C01*C32) + C21*(C00*C32 - C02*C30) + C22*(C01*C30 - C00*C31)),
                       s*(C30*(C02*C11 - C01*C12) + C31*(C00*C12 - C02*C10) + C32*(C01*C10 - C00*C11)),
                       s*(C00*(C11*C22 - C12*C21) + C01*(C12*C20 - C10*C22) + C02*(C10*C21 - C11*C20)));

  return TRUE;
}

/**
 * vsg_matrix4@t@_component:
 * @mat: a #VsgMatrix4@t@
 * @i: a #guint
 * @j: a #guint
 *
 * Reads component @mat[@i,@j].
 *
 * Returns: @mat[@i,@j]
 */
@type@ vsg_matrix4@t@_component (const VsgMatrix4@t@ *mat, guint i, guint j)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (mat != NULL, 0.);

  g_return_val_if_fail (i<4, 0.);
  g_return_val_if_fail (i>=0, 0.);
  g_return_val_if_fail (j<4, 0.);
  g_return_val_if_fail (j>=0, 0.);
#endif

  return VSG_MATRIX4@T@_COMP (mat, i, j);
}

/**
 * vsg_matrix4@t@_transpose:
 * @mat: a #VsgMatrix4@t@
 * @result: a #VsgMatrix4@t@
 *
 * Performs matrix transposition on @mat and stores the result in
 * @result. Attribute aliasing is allowed.
 */
void vsg_matrix4@t@_transpose (const VsgMatrix4@t@ *mat, VsgMatrix4@t@ *result)
{
  @type@ tmp;
  int i, j;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (result != NULL);
#endif

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

/**
 * vsg_matrix4@t@_write:
 * @mat: a #VsgMatrix4@t@
 * @file: a #FILE pointer
 *
 * Writes @mat components to @file.
 */
void vsg_matrix4@t@_write (const VsgMatrix4@t@ *mat, FILE *file)
{
  int i, j;
  gchar bufx[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *x;

  g_return_if_fail (mat != NULL);
  g_return_if_fail (file != NULL);

  for (i=0; i<4; i++)
    {
      for (j=0; j<4; j++)
        {
/*           fprintf (file, "%@tcode@ ", VSG_MATRIX4@T@_COMP (mat, i, j)); */
          x = g_ascii_dtostr (bufx, G_ASCII_DTOSTR_BUF_SIZE,
                              VSG_MATRIX4@T@_COMP (mat, i, j));
          fprintf (file, "%s ", x);
        }
      fprintf (file, "\n");
    }
}

/**
 * vsg_matrix4@t@_print:
 * @mat: a #VsgMatrix4@t@
 *
 * Writes @mat components to %stdout.
 */
void vsg_matrix4@t@_print(const VsgMatrix4@t@ *mat)
{
  g_return_if_fail (mat != NULL);

  vsg_matrix4@t@_write (mat, stdout);
}


/**
 * VsgMatrix4@t@:
 * @components: matrix (4x4) components.
 *
 * A 4x4 matrix that may represent 3D affine transformations.
 */
