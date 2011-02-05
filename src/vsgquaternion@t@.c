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

#include "vsgquaternion@t@.h"

#define VSG_NO_IMPLICIT_INLINE
#include "vsgquaternion@t@-inline.h"

#include <math.h>
#include <glib/gprintf.h>

#define _USE_G_SLICES GLIB_CHECK_VERSION (2, 10, 0)

#if ! _USE_G_SLICES

#define VSG_QUATERNION@T@_PREALLOC 128

static GMemChunk *vsg_quaternion@t@_mem_chunk = 0;
static guint vsg_quaternion@t@_instances_count = 0;

static void vsg_quaternion@t@_finalize ();
static VsgQuaternion@t@ *_quaternion@t@_alloc ();

static void vsg_quaternion@t@_finalize ()
{
  if (vsg_quaternion@t@_mem_chunk)
    {
      g_mem_chunk_destroy (vsg_quaternion@t@_mem_chunk);
      vsg_quaternion@t@_mem_chunk = 0;
    }
}

static VsgQuaternion@t@ *_quaternion@t@_alloc ()
{
  if (!vsg_quaternion@t@_mem_chunk)
    {
      vsg_quaternion@t@_mem_chunk = g_mem_chunk_create (VsgQuaternion@t@,
                                                        VSG_QUATERNION@T@_PREALLOC,
                                                        G_ALLOC_ONLY);
    }

  vsg_quaternion@t@_instances_count ++;

  return g_chunk_new (VsgQuaternion@t@, vsg_quaternion@t@_mem_chunk);
}
#endif /* ! _USE_G_SLICES */

/* private function */
void vsg_quaternion@t@_init ()
{
#if ! _USE_G_SLICES
  static gboolean wasinit = FALSE;

  if (! wasinit)
    {
      wasinit = TRUE;
      g_atexit (vsg_quaternion@t@_finalize);
    }
#endif /* ! _USE_G_SLICES */
}

/* public functions */

/**
 * VSG_TYPE_QUATERNION@T@:
 *
 * The #GBoxed #GType associated to #VsgQuaternion@t@.
 */

GType vsg_quaternion@t@_get_type (void)
{
  static GType quaternion@t@_type = 0;

  if (!quaternion@t@_type)
    {
      quaternion@t@_type =
        g_boxed_type_register_static ("VsgQuaternion@t@",
                                      (GBoxedCopyFunc) vsg_quaternion@t@_clone,
                                      (GBoxedFreeFunc) vsg_quaternion@t@_free);

    }

  return quaternion@t@_type;
}

#ifdef VSG_HAVE_MPI
/**
 * VSG_MPI_TYPE_QUATERNION@T@:
 *
 * The #MPI_Datatype associated to #VsgQuaternion@t@.
 */

MPI_Datatype vsg_quaternion@t@_get_mpi_type (void)
{
  static MPI_Datatype quaternion@t@_mpi_type = MPI_DATATYPE_NULL;

  if (quaternion@t@_mpi_type == MPI_DATATYPE_NULL)
    {
      MPI_Type_contiguous (4, @MPI_DATATYPE@, &quaternion@t@_mpi_type);
      MPI_Type_commit (&quaternion@t@_mpi_type);
    }

  return quaternion@t@_mpi_type;
}
#endif

/**
 * vsg_quaternion@t@_new:
 * @x: abscissa
 * @y: ordinate
 * @z: Z coordinate
 * @w: W coordinate
 *
 * Allocates a new instance of #VsgQuaternion@t@. This instance MUST
 * be freed only with vsg_quaternion@t@_free().
 *
 * Returns: new #VsgQuaternion@t@ instance.
 */
VsgQuaternion@t@ *vsg_quaternion@t@_new (@type@ x,
                                         @type@ y,
                                         @type@ z,
                                         @type@ w)
{
#if _USE_G_SLICES
  VsgQuaternion@t@ *result = g_slice_new (VsgQuaternion@t@);
#else
  VsgQuaternion@t@ *result = _quaternion@t@_alloc ();
#endif

  vsg_quaternion@t@_set_inline (result, x, y, z, w);

  return result;
}

/**
 * vsg_quaternion@t@_free:
 * @quat: #VsgQuaternion@t@ instance to deallocate.
 *
 * Frees a #VsgQuaternion@t@ instance previously allocated with
 * vsg_quaternion@t@_new().
 */
void vsg_quaternion@t@_free (VsgQuaternion@t@ *quat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
#endif

#if _USE_G_SLICES
  g_slice_free (VsgQuaternion@t@, quat);
#else
  g_chunk_free (quat, vsg_quaternion@t@_mem_chunk);

  vsg_quaternion@t@_instances_count --;

  if (vsg_quaternion@t@_instances_count == 0)
    vsg_quaternion@t@_finalize ();
#endif /* _USE_G_SLICES */
}

/**
 * vsg_quaternion@t@_identity_new:
 *
 * Allocates a new #VsgQuaternion@t@ with Identity component.
 *
 * Returns: new #VsgQuaternion@t@ instance
 */
VsgQuaternion@t@ *vsg_quaternion@t@_identity_new ()
{
#if _USE_G_SLICES
  VsgQuaternion@t@ *result = g_slice_new (VsgQuaternion@t@);
#else
  VsgQuaternion@t@ *result = _quaternion@t@_alloc ();
#endif

  vsg_quaternion@t@_identity_inline (result);

  return result;
}

/**
 * vsg_quaternion@t@_vector3@t@_new:
 * @vector: a #VsgVector3@t@
 *
 * Allocates a new #VsgQuaternion@t@ with @vector component.
 *
 * Returns: new #VsgQuaternion@t@ instance
 */
VsgQuaternion@t@ *
vsg_quaternion@t@_vector3@t@_new (const VsgVector3@t@ *vector)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vector != NULL, NULL);
#endif

  return vsg_quaternion@t@_new (vector->x,
                                vector->y,
                                vector->z,
                                (@type@) 1.);
}

/**
 * vsg_quaternion@t@_vector3@alt_t@_new:
 * @vector: a #VsgVector3@alt_t@
 *
 * Allocates a new #VsgQuaternion@t@ with @vector component.
 *
 * Returns: new #VsgQuaternion@t@ instance
 */
VsgQuaternion@t@ *
vsg_quaternion@t@_vector3@alt_t@_new (const VsgVector3@alt_t@ *vector)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vector != NULL, NULL);
#endif

  return vsg_quaternion@t@_new ((@type@) vector->x,
                                (@type@) vector->y,
                                (@type@) vector->z,
                                (@type@) 1.);
}

/**
 * vsg_quaternion@t@_set:
 * @quat: a #VsgQuaternion@t@
 * @x: abscissa
 * @y: ordinate
 * @z: Z coordinate
 * @w: W coordinate
 *
 * Sets @quat components to (@x, @y, @z, @w)
 */
void vsg_quaternion@t@_set (VsgQuaternion@t@ *quat,
                            @type@ x, @type@ y,
                            @type@ z, @type@ w)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
#endif

  vsg_quaternion@t@_set_inline (quat, x,  y, z, w);
}

/**
 * vsg_quaternion@t@_vector3@t@_set:
 * @quat: a #VsgQuaternion@t@
 * @vector: a #VsgVector3@t@
 *
 * Sets @quat to components to @vector.
 */
void vsg_quaternion@t@_vector3@t@_set (VsgQuaternion@t@ *quat,
                                       const VsgVector3@t@ *vector)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
  g_return_if_fail (vector != NULL);
#endif

  vsg_quaternion@t@_vector3@t@_set_inline (quat, vector);
}

/**
 * vsg_quaternion@t@_vector3@alt_t@_set:
 * @quat: a #VsgQuaternion@t@
 * @vector: a #VsgVector3@alt_t@
 *
 * Sets @quat to components to @vector.
 */
void
vsg_quaternion@t@_vector3@alt_t@_set (VsgQuaternion@t@ *quat,
                                      const VsgVector3@alt_t@ *vector)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
  g_return_if_fail (vector != NULL);
#endif

  vsg_quaternion@t@_vector3@alt_t@_set_inline (quat, vector);
}

/**
 * vsg_quaternion@t@_make_rotate@t@:
 * @quat: a #VsgQuaternion@t@
 * @angle: a #@type@
 * @vec: a #VsgVector3@t@
 *
 * Sets @quat components so as to correspond to a 3D rotation of angle
 * @angle and axis @vec.
 */
void vsg_quaternion@t@_make_rotate@t@ (VsgQuaternion@t@ *quat,
                                       @type@ angle,
                                       const VsgVector3@t@ *vec)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
  g_return_if_fail (vec!= NULL);
#endif

  vsg_quaternion@t@_make_rotate@t@_inline (quat, angle, vec);
}

/**
 * vsg_quaternion@t@_rotate@t@_set:
 * @quat: a #VsgQuaternion@t@
 * @from: a #VsgVector3@t@
 * @to: a #VsgVector3@t@
 *
 * Sets @quat components so as to correspond to a 3D rotation between vector
 * @from and vector @to.
 */
void vsg_quaternion@t@_rotate@t@_set (VsgQuaternion@t@ *quat,
                                      const VsgVector3@t@ *from,
                                      const VsgVector3@t@ *to)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
  g_return_if_fail (from != NULL);
  g_return_if_fail (to != NULL);
#endif

  vsg_quaternion@t@_rotate@t@_set_inline (quat, from, to);
}

/**
 * vsg_quaternion@t@_identity:
 * @quat: a #VsgQuaternion@t@
 *
 * Sets @quat components to Identity.
 */
void vsg_quaternion@t@_identity (VsgQuaternion@t@ *quat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
#endif

  vsg_quaternion@t@_identity_inline (quat);
}

/**
 * vsg_quaternion@t@_rotate_x:
 * @quat: a #VsgQuaternion@t@
 * @angle: a @type@
 *
 * Sets @quat components to a 3D rotation of angle @angle and axis X.
 */
void vsg_quaternion@t@_rotate_x (VsgQuaternion@t@ *quat, @type@ angle)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
#endif

  vsg_quaternion@t@_rotate_x_inline (quat, angle);
}

/**
 * vsg_quaternion@t@_rotate_y:
 * @quat: a #VsgQuaternion@t@
 * @angle: a @type@
 *
 * Sets @quat components to a 3D rotation of angle @angle and axis Y.
 */
void vsg_quaternion@t@_rotate_y (VsgQuaternion@t@ *quat, @type@ angle)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
#endif

  vsg_quaternion@t@_rotate_y_inline (quat, angle);
}

/**
 * vsg_quaternion@t@_rotate_z:
 * @quat: a #VsgQuaternion@t@
 * @angle: a @type@
 *
 * Sets @quat components to a 3D rotation of angle @angle and axis Z.
 */
void vsg_quaternion@t@_rotate_z (VsgQuaternion@t@ *quat, @type@ angle)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
#endif

  vsg_quaternion@t@_rotate_z_inline (quat, angle);
}

/**
 * vsg_quaternion@t@_rotate_cardan:
 * @quat: a #VsgQuaternion@t@
 * @ax: a #@type@
 * @ay: a #@type@
 * @az: a #@type@
 *
 * Sets @quat components to  a 3D rotation of cardan angles 
 * @ax, @ay, @az.
 */
void vsg_quaternion@t@_rotate_cardan (VsgQuaternion@t@ *quat,
                                      @type@ ax,
                                      @type@ ay,
                                      @type@ az)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
#endif

  vsg_quaternion@t@_rotate_cardan_inline (quat, ax, ay, az);
}

/**
 * vsg_quaternion@t@_rotate_euler:
 * @quat: a #VsgQuaternion@t@
 * @alpha: a #@type@
 * @beta: a #@type@
 * @gamma: a #@type@
 *
 * Sets @quat components to  a 3D rotation of euler angles 
 * @alpha, @beta, @gamma.
 */
void vsg_quaternion@t@_rotate_euler (VsgQuaternion@t@ *quat,
                                     @type@ alpha,
                                     @type@ beta,
                                     @type@ gamma)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
#endif

  vsg_quaternion@t@_rotate_euler_inline (quat, alpha, beta, gamma);
}

/**
 * vsg_quaternion@t@_copy:
 * @dst: a #VsgQuaternion@t@
 * @src: a #VsgQuaternion@t@
 *
 * Copies @src components to @dst.
 */
void vsg_quaternion@t@_copy (const VsgQuaternion@t@ *src,
                             VsgQuaternion@t@ *dst)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (dst != NULL);
  g_return_if_fail (src != NULL);
#endif

  vsg_quaternion@t@_copy_inline (src, dst);
}

/**
 * vsg_quaternion@t@_clone:
 * @src: a #VsgQuaternion@t@
 *
 * Duplicates @src.
 *
 * Returns: a newly allocated copy of @src.
 */
VsgQuaternion@t@ *vsg_quaternion@t@_clone (const VsgQuaternion@t@ *src)
{
  VsgQuaternion@t@ *dst;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (src != NULL, NULL);
#endif

#if _USE_G_SLICES
  dst = g_slice_new (VsgQuaternion@t@);
#else
  dst = _quaternion@t@_alloc ();
#endif

  vsg_quaternion@t@_copy_inline (src, dst);

  return dst;
}

/**
 * vsg_quaternion@t@_square_norm:
 * @quat: a #VsgQuaternion@t@
 *
 * Computes vsg_quaternion@t@_dotp(@quat, @quat).
 *
 * Returns: dot product of @quat by @quat.
 */
@type@ vsg_quaternion@t@_square_norm (const VsgQuaternion@t@ *quat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (quat != NULL, 0);
#endif

  return vsg_quaternion@t@_square_norm_inline (quat);

}

/**
 * vsg_quaternion@t@_norm:
 * @quat: a #VsgQuaternion@t@
 *
 * Computes @quat norm: sqrt (vsg_quaternion@t@_square_norm (quat)).
 *
 * Returns: @quat norm
 */
@type@ vsg_quaternion@t@_norm (const VsgQuaternion@t@ *quat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (quat != NULL, 0);
#endif

  return vsg_quaternion@t@_norm_inline (quat);
}

/**
 * vsg_quaternion@t@_normalize:
 * @quat: a #VsgQuaternion@t@
 *
 * Sets @quat components so its norm (vsg_quaternion@t@_norm) is %1 and its
 * "direction" is unchanged.
 */
void vsg_quaternion@t@_normalize (VsgQuaternion@t@ *quat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
#endif

  vsg_quaternion@t@_normalize_inline (quat);
}

/**
 * vsg_quaternion@t@_scalp:
 * @quat: a #VsgQuaternion@t@
 * @scal: a #@type@
 * @result: a #VsgQuaternion@t@
 *
 * Performs scalar product on @quat with @scal as a multiplier.
 */
void vsg_quaternion@t@_scalp (const VsgQuaternion@t@ *quat,
                              @type@ scal,
                              VsgQuaternion@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_quaternion@t@_scalp_inline (quat, scal, result);
}

/**
 * vsg_quaternion@t@_dotp:
 * @quat: a #VsgQuaternion@t@
 * @other: a #VsgQuaternion@t@
 *
 * Computes dot product of @quat with @other.
 *
 * Returns: dot product of @quat by @other.
 */
@type@ vsg_quaternion@t@_dotp (const VsgQuaternion@t@ *quat,
                               const VsgQuaternion@t@ *other)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (quat != NULL, 0);
  g_return_val_if_fail (other != NULL, 0);
#endif

  return vsg_quaternion@t@_dotp_inline (quat, other);

}

/**
 * vsg_quaternion@t@_mult:
 * @quat: a #VsgQuaternion@t@
 * @other: a #VsgQuaternion@t@
 * @result: a #VsgQuaternion@t@
 *
 * Performs #VsgQuaternion@t@ multiplication of @quat by @other and
 * stores the result in @result. Argument aliasing is allowed.
 */
void vsg_quaternion@t@_mult (const VsgQuaternion@t@ *quat,
                             const VsgQuaternion@t@ *other,
                             VsgQuaternion@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_quaternion@t@_mult_inline (quat, other, result);
}

/**
 * vsg_quaternion@t@_invert:
 * @quat: a #VsgQuaternion@t@
 * @result: a #VsgQuaternion@t@
 *
 * Performs #VsgQuaternion@t@ (as a 3D rotation) of @quat and stores
 * the result in @result. Argument aliasing is allowed.
 *
 * Returns: a flag indicating success of the inversion.
 */
gboolean vsg_quaternion@t@_invert (const VsgQuaternion@t@ *quat,
                                   VsgQuaternion@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (quat != NULL, 0);
  g_return_val_if_fail (result != NULL, 0);
#endif

  return vsg_quaternion@t@_invert_inline (quat, result);
}


/**
 * vsg_quaternion@t@_slerp:
 * @quat1: a #VsgQuaternion@t@
 * @quat2: a #VsgQuaternion@t@
 * @t: a #@type@
 * @result: a #VsgQuaternion@t@
 *
 * Performs Spherical Linear Interpolation between @quat1 and @quat2
 * with parameter @t and stores the result in @result. Argument
 * aliasing is allowed.
 */
void vsg_quaternion@t@_slerp (const VsgQuaternion@t@ *quat1,
                              const VsgQuaternion@t@ *quat2,
                              @type@ t, VsgQuaternion@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat1 != NULL);
  g_return_if_fail (quat2 != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_quaternion@t@_slerp_inline (quat1, quat2, t, result);
}

/**
 * vsg_quaternion@t@_conjugate:
 * @quat: a #VsgQuaternion@t@
 * @result: a #VsgQuaternion@t@
 *
 * Computes @quat conjugate and stores the result in @result. Argument
 * aliasing is allowed.
 */
void vsg_quaternion@t@_conjugate (const VsgQuaternion@t@ *quat,
                                  VsgQuaternion@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (quat != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_quaternion@t@_conjugate_inline (quat,result);
}

/**
 * vsg_quaternion@t@_write:
 * @quat: a #VsgQuaternion@t@
 * @file: a @FILE
 *
 * Writes @quat in @file. 
 */
void vsg_quaternion@t@_write (const VsgQuaternion@t@ *quat,
                              FILE *file)
{
  gchar bufx[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *x;
  gchar bufy[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *y;
  gchar bufz[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *z;
  gchar bufw[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *w;

  g_return_if_fail (quat != NULL);
  g_return_if_fail (file != NULL);

/*   fprintf (file, "[%@tcode@,%@tcode@,%@tcode@,%@tcode@]", */
/*            quat->x, */
/*            quat->y, */
/*            quat->z, */
/*            quat->w); */

  /* avoid locale problems */
  x = g_ascii_dtostr (bufx, G_ASCII_DTOSTR_BUF_SIZE, quat->x);
  y = g_ascii_dtostr (bufy, G_ASCII_DTOSTR_BUF_SIZE, quat->y);
  z = g_ascii_dtostr (bufz, G_ASCII_DTOSTR_BUF_SIZE, quat->z);
  w = g_ascii_dtostr (bufw, G_ASCII_DTOSTR_BUF_SIZE, quat->w);
  fprintf(file, "[%s,%s,%s,%s]", x, y, z, w);
}

/**
 * vsg_quaternion@t@_print:
 * @quat: a #VsgQuaternion@t@
 *
 * Writes @quat in %stdout.
 */
void vsg_quaternion@t@_print (const VsgQuaternion@t@ *quat)
{
  vsg_quaternion@t@_write (quat, stdout);
}

/**
 * VsgQuaternion@t@:
 * @x: X coordinate
 * @y: Y coordinate
 * @z: Z coordinate
 * @w: w coordinate
 *
 * Embodies the Quaternion extension to 3D rotations. This data type
 * can be used to store 3D rotations without the Euler angles
 * drawback known as "Gimbal Lock". In order to get more informations
 * about Quaternions, one can visit the Matrix FAQ at
 * http://www.j3d.org/matrix_faq/
 */

/**
 * VSG_QUATERNION@T@_COMP:
 * @quat: a #VsgQuaternion@t@
 * @i: an integer
 *
 * Computes @i th component of a quaternion (in order x, y, z, w).
 */
