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

#include "vsg-config.h"

#include "vsgmatrix4@t@.h"
#include "vsgquaternion@t@.h"

#define VSG_NO_IMPLICIT_INLINE
#include "vsgmatrix4@t@-inline.h"

#include <math.h>

#include <glib/gprintf.h>

/* 
 * MAT(i,j)= components[i+4*j]
 * this is because we want to get OpenGL compliant matrices.
 */

#define VSG_MATRIX4@T@_COMP(mat, i, j) ( \
* ((mat) -> components + i + 4*j) \
)

#define _USE_G_SLICES GLIB_CHECK_VERSION (2, 10, 0)

#if ! _USE_G_SLICES

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
#endif /* ! _USE_G_SLICES */

/* private function */
void vsg_matrix4@t@_init ()
{
#if ! _USE_G_SLICES
  static gboolean wasinit = FALSE;

  if (! wasinit)
    {
      wasinit = TRUE;
      g_atexit (vsg_matrix4@t@_finalize);
    }
#endif /* ! _USE_G_SLICES */
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

#ifdef VSG_HAVE_MPI
/**
 * VSG_MPI_TYPE_MATRIX4@T@:
 *
 * The #MPI_Datatype associated to #VsgMatrix4@t@.
 */

MPI_Datatype vsg_matrix4@t@_get_mpi_type (void)
{
  static MPI_Datatype matrix4@t@_mpi_type = MPI_DATATYPE_NULL;

  if (matrix4@t@_mpi_type == MPI_DATATYPE_NULL)
    {
      MPI_Type_contiguous (9, @MPI_DATATYPE@, &matrix4@t@_mpi_type);
      MPI_Type_commit (&matrix4@t@_mpi_type);
    }

  return matrix4@t@_mpi_type;
}
#endif

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
#if _USE_G_SLICES
  VsgMatrix4@t@ *result = g_slice_new (VsgMatrix4@t@);
#else
  VsgMatrix4@t@ *result = _matrix4@t@_alloc ();
#endif

  vsg_matrix4@t@_set_inline (result,
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

#if _USE_G_SLICES
  g_slice_free (VsgMatrix4@t@, mat);
#else
  g_chunk_free (mat, vsg_matrix4@t@_mem_chunk);

  vsg_matrix4@t@_instances_count --;

  if (vsg_matrix4@t@_instances_count == 0)
    vsg_matrix4@t@_finalize ();
#endif /* _USE_G_SLICES */
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
#if _USE_G_SLICES
  VsgMatrix4@t@ *result = g_slice_new (VsgMatrix4@t@);
#else
  VsgMatrix4@t@ *result = _matrix4@t@_alloc ();
#endif

  vsg_matrix4@t@_identity_inline (result);

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

  vsg_matrix4@t@_rotate_cardan_inline (result, ax, ay, az);

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

  vsg_matrix4@t@_rotate_euler_inline (result, alpha, beta, gamma);

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

  vsg_matrix4@t@_translate_inline (result, x, y, z);

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

  vsg_matrix4@t@_scale_inline (result, x, y, z);

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

  vsg_matrix4@t@_quaternion@t@_inline (result,quat);

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

  vsg_matrix4@t@_set_inline (mat,
                             a00, a01, a02, a03,
                             a10, a11, a12, a13,
                             a20, a21, a22, a23,
                             a30, a31, a32, a33);
}

/**
 * vsg_matrix4@t@_identity:
 * @mat: a #VsgMatrix4@t@
 *
 * Sets components of @mat to values of Identity.
 */
void vsg_matrix4@t@_identity (VsgMatrix4@t@ *mat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix4@t@_identity_inline (mat);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (dst != NULL);
  g_return_if_fail (src != NULL);
#endif

  vsg_matrix4@t@_copy_inline (src, dst);
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

#if _USE_G_SLICES
  dst = g_slice_new (VsgMatrix4@t@);
#else
  dst = _matrix4@t@_alloc ();
#endif

  vsg_matrix4@t@_copy_inline (src, dst);

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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix4@t@_rotate_x_inline (mat, angle);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix4@t@_rotate_y_inline (mat, angle);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix4@t@_rotate_z_inline (mat, angle);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix4@t@_rotate_cardan_inline (mat, ax, ay, az);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix4@t@_rotate_euler_inline (mat, alpha, beta, gamma);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix4@t@_translate_inline (mat, x, y, z);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix4@t@_scale_inline (mat, x, y, z);
}


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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (quat != NULL);
#endif

  vsg_matrix4@t@_quaternion@t@_set_inline (mat, quat);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (quat != NULL);
#endif

  vsg_matrix4@t@_quaternion@t@_inline (mat, quat);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix4@t@_add_inline (mat, other, result);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix4@t@_sub_inline (mat, other, result);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix4@t@_matmult_inline (mat, other, result);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (vec != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix4@t@_vecmult_inline (mat, vec, result);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (vec != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix4@t@_vec4mult_inline (mat, vec, result);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (vec != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix4@t@_vec4mult_T_inline (mat, vec, result);
}

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

  return vsg_matrix4@t@_det_inline (mat);
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
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (mat != NULL, 0.);
  g_return_val_if_fail (result != NULL, 0.);
#endif

  return vsg_matrix4@t@_invert_inline (mat, result);
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

  return vsg_matrix4@t@_component_inline (mat, i, j);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix4@t@_transpose_inline (mat, result);
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

/**
 * VSG_MATRIX4@T@_COMP:
 *
 * Returns: @mat component of coordinates (@i,@j).
 */
