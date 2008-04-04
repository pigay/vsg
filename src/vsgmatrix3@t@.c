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

#include "vsgmatrix3@t@.h"

#define VSG_NO_IMPLICIT_INLINE
#include "vsgmatrix3@t@-inline.h"

#include <math.h>

#include <glib/gprintf.h>

#define VSG_MATRIX3@T@_PREALLOC 128

static const @type@ degrad = M_PI / 180.;

static GMemChunk *vsg_matrix3@t@_mem_chunk = 0;
static guint vsg_matrix3@t@_instances_count = 0;

static void vsg_matrix3@t@_finalize ();
static VsgMatrix3@t@ *_matrix3@t@_alloc ();

static void vsg_matrix3@t@_finalize ()
{
  if (vsg_matrix3@t@_mem_chunk)
    {
      g_mem_chunk_destroy (vsg_matrix3@t@_mem_chunk);
      vsg_matrix3@t@_mem_chunk = 0;
    }
}

static VsgMatrix3@t@ *_matrix3@t@_alloc ()
{
  if (!vsg_matrix3@t@_mem_chunk)
    {
      vsg_matrix3@t@_mem_chunk = g_mem_chunk_create (VsgMatrix3@t@,
                                                        VSG_MATRIX3@T@_PREALLOC,
                                                        G_ALLOC_ONLY);
    }

  vsg_matrix3@t@_instances_count ++;

  return g_chunk_new (VsgMatrix3@t@, vsg_matrix3@t@_mem_chunk);
}

/* private function */
void vsg_matrix3@t@_init ()
{
  static gboolean wasinit = FALSE;

  if (! wasinit)
    {
      wasinit = TRUE;
      g_atexit (vsg_matrix3@t@_finalize);
    }
}

/* public functions */

/**
 * VSG_TYPE_MATRIX3@T@:
 *
 * The #GBoxed #GType associated to #VsgMatrix3@t@.
 */

GType vsg_matrix3@t@_get_type (void)
{
  static GType matrix3@t@_type = 0;

  if (!matrix3@t@_type)
    {
      matrix3@t@_type =
        g_boxed_type_register_static ("VsgMatrix3@t@",
                                      (GBoxedCopyFunc) vsg_matrix3@t@_clone,
                                      (GBoxedFreeFunc) vsg_matrix3@t@_free);

    }

  return matrix3@t@_type;
}

/**
 * vsg_matrix3@t@_new:
 * @a00: a #@type@
 * @a01: a #@type@
 * @a02: a #@type@
 * @a10: a #@type@
 * @a11: a #@type@
 * @a12: a #@type@
 * @a20: a #@type@
 * @a21: a #@type@
 * @a22: a #@type@
 *
 * Allocates a new #VsgMatrix3@t@. Arguments specify new matrix components.
 *
 * Returns: new #VsgMatrix3@t@ instance
 */
VsgMatrix3@t@ *vsg_matrix3@t@_new (@type@ a00, @type@ a01,
                                   @type@ a02, @type@ a10,
                                   @type@ a11, @type@ a12,
                                   @type@ a20, @type@ a21,
                                   @type@ a22)
{
  VsgMatrix3@t@ *result = _matrix3@t@_alloc ();

  vsg_matrix3@t@_set_inline (result,
                             a00, a01, a02, a10, a11, a12, a20, a21, a22);

  return result;
}

/**
 * vsg_matrix3@t@_free:
 * @mat: a #VsgMatrix3@t@
 *
 * Deallocates @mat.
 */
void vsg_matrix3@t@_free (VsgMatrix3@t@ *mat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  g_chunk_free (mat, vsg_matrix3@t@_mem_chunk);

  vsg_matrix3@t@_instances_count --;

  if (vsg_matrix3@t@_instances_count == 0)
    vsg_matrix3@t@_finalize ();
}

/**
 * vsg_matrix3@t@_identity_new:
 *
 * Allocates a new #VsgMatrix3@t@ with Identity component (%1 on the diagonal,
 * %0 elsewhere).
 *
 * Returns: new #VsgMatrix3@t@ instance
 */
VsgMatrix3@t@ *vsg_matrix3@t@_identity_new ()
{
  VsgMatrix3@t@ *result = _matrix3@t@_alloc ();

  vsg_matrix3@t@_identity_inline (result);

  return result;
}

/**
 * vsg_matrix3@t@_rotate_new:
 * @a: a #@type@
 *
 * Creates a new #VsgMatrix3@t@ corresponding to a 2D rotation of angle @a.
 *
 * Returns: new #VsgMatrix3@t@ instance
 */
VsgMatrix3@t@ *vsg_matrix3@t@_rotate_new (@type@ a)
{
  VsgMatrix3@t@ *result = vsg_matrix3@t@_identity_new ();

  vsg_matrix3@t@_rotate_inline (result, a);

  return result;
}

/**
 * vsg_matrix3@t@_translate_new:
 * @x: a #@type@
 * @y: a #@type@
 *
 * Creates a new #VsgMatrix3@t@ corresponding to a 2D translation with
 * coordinates (@x,@y).
 *
 * Returns: new #VsgMatrix3@t@ instance
 */
VsgMatrix3@t@ *vsg_matrix3@t@_translate_new (@type@ x, @type@ y)
{
  VsgMatrix3@t@ *result = vsg_matrix3@t@_identity_new ();

  vsg_matrix3@t@_translate_inline (result, x, y);

  return result;

}

/**
 * vsg_matrix3@t@_scale_new:
 * @x: a #@type@
 * @y: a #@type@
 *
 * Creates a new #VsgMatrix3@t@ corresponding to a 2D scaling with
 * coordinates (@x,@y).
 *
 * Returns: new #VsgMatrix3@t@ instance
 */
VsgMatrix3@t@ *vsg_matrix3@t@_scale_new (@type@ x, @type@ y)
{
  VsgMatrix3@t@ *result = vsg_matrix3@t@_identity_new ();

  vsg_matrix3@t@_scale_inline (result, x, y);

  return result;
}

/**
 * vsg_matrix3@t@_set:
 * @mat: a #VsgMatrix3@t@
 * @a00: a #@type@
 * @a01: a #@type@
 * @a02: a #@type@
 * @a10: a #@type@
 * @a11: a #@type@
 * @a12: a #@type@
 * @a20: a #@type@
 * @a21: a #@type@
 * @a22: a #@type@
 *
 * Sets components of @mat to values specified by other arguments.
 */
void vsg_matrix3@t@_set (VsgMatrix3@t@ *mat,
                         @type@ a00, @type@ a01, @type@ a02,
                         @type@ a10, @type@ a11, @type@ a12,
                         @type@ a20, @type@ a21, @type@ a22)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix3@t@_set_inline (mat,
                             a00,  a01, a02,
                             a10, a11, a12,
                             a20, a21, a22);
}

/**
 * vsg_matrix3@t@_identity:
 * @mat: a #VsgMatrix3@t@
 *
 * Sets components of @mat to values of Identity.
 */
void vsg_matrix3@t@_identity (VsgMatrix3@t@ *mat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix3@t@_identity_inline (mat);
}

/**
 * vsg_matrix3@t@_copy:
 * @dst: a #VsgMatrix3@t@
 * @src: a #VsgMatrix3@t@
 *
 * Copies @src to @dst.
 */
void vsg_matrix3@t@_copy (const VsgMatrix3@t@ *src, VsgMatrix3@t@ *dst)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (dst != NULL);
  g_return_if_fail (src != NULL);
#endif

  vsg_matrix3@t@_copy_inline (src, dst);
}

/**
 * vsg_matrix3@t@_clone:
 * @src: a #VsgMatrix3@t@.
 *
 * Duplicates @src.
 *
 * Returns: a copy of @src.
 */
VsgMatrix3@t@ *vsg_matrix3@t@_clone (const VsgMatrix3@t@ *src)
{
  VsgMatrix3@t@ *dst;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (src != NULL, NULL);
#endif

  dst = _matrix3@t@_alloc ();

  vsg_matrix3@t@_copy_inline (src, dst);

  return dst;
}

/**
 * vsg_matrix3@t@_rotate:
 * @mat: a #VsgMatrix3@t@
 * @a: a #@type@
 *
 * Performs a 2D rotation of angle @a on matrix @mat.
 */
void vsg_matrix3@t@_rotate (VsgMatrix3@t@ *mat, @type@ a)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix3@t@_rotate_inline (mat, a);
}

/**
 * vsg_matrix3@t@_translate:
 * @mat: a #VsgMatrix3@t@
 * @x: a #@type@
 * @y: a #@type@
 *
 * Performs a 2D translation of coordinates (@x,@y) on matrix @mat.
 */
void vsg_matrix3@t@_translate (VsgMatrix3@t@ *mat, @type@ x, @type@ y)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix3@t@_translate_inline (mat, x, y);
}

/**
 * vsg_matrix3@t@_scale:
 * @mat: a #VsgMatrix3@t@
 * @x: a #@type@
 * @y: a #@type@
 *
 * Performs a 2D scaling of coordinates (@x,@y) on matrix @mat.
 */
void vsg_matrix3@t@_scale (VsgMatrix3@t@ *mat, @type@ x, @type@ y)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
#endif

  vsg_matrix3@t@_scale_inline (mat, x, y);
}

/**
 * vsg_matrix3@t@_add:
 * @mat: a #VsgMatrix3@t@
 * @other: a #VsgMatrix3@t@
 * @result: a #VsgMatrix3@t@
 *
 * Performs matrix addition between @mat and @other and stores
 * the result in @result. Argument aliasing is allowed.
 */
void vsg_matrix3@t@_add (const VsgMatrix3@t@ *mat,
                         const VsgMatrix3@t@ *other,
                         VsgMatrix3@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix3@t@_add_inline (mat, other, result);
}

/**
 * vsg_matrix3@t@_sub:
 * @mat: a #VsgMatrix3@t@
 * @other: a #VsgMatrix3@t@
 * @result: a #VsgMatrix3@t@
 *
 * Performs matrix subtraction between @mat and @other and stores
 * the result in @result. Argument aliasing is allowed.
 */
void vsg_matrix3@t@_sub (const VsgMatrix3@t@ *mat,
                         const VsgMatrix3@t@ *other,
                         VsgMatrix3@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix3@t@_sub_inline (mat, other, result);
}

/**
 * vsg_matrix3@t@_matmult:
 * @mat: a #VsgMatrix3@t@
 * @other: a #VsgMatrix3@t@
 * @result: a #VsgMatrix3@t@
 *
 * Performs matrix product of @mat by @other and stores the result in
 * @result. Argument aliasing is allowed.
 */
void vsg_matrix3@t@_matmult (const VsgMatrix3@t@ *mat,
                             const VsgMatrix3@t@ *other,
                             VsgMatrix3@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix3@t@_matmult_inline (mat, other, result);
}

/**
 * vsg_matrix3@t@_vecmult:
 * @mat: a #VsgMatrix3@t@
 * @vec: a #VsgVector2@t@
 * @result: a #VsgVector2@t@
 *
 * Performs matrix-vector product of @mat by @vec and stores the result
 * in @result. Argument aliasing is allowed.
 */
void vsg_matrix3@t@_vecmult (const VsgMatrix3@t@ *mat,
                             const VsgVector2@t@ *vec,
                             VsgVector2@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (vec != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix3@t@_vecmult_inline (mat, vec, result);
}


/**
 * vsg_matrix3@t@_vec3mult:
 * @mat: a #VsgMatrix3@t@.
 * @vec: a #VsgVector3@t@.
 * @result: a #VsgVector3@t@.
 *
 * Performs matrix-vector product of @mat by @vec and stores the result
 * in @result. Argument aliasing is allowed.
 */
void vsg_matrix3@t@_vec3mult (const VsgMatrix3@t@ *mat,
                              const VsgVector3@t@ *vec,
                              VsgVector3@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (vec != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix3@t@_vec3mult_inline (mat, vec, result);
}

/**
 * vsg_matrix3@t@_vec3mult_T:
 * @mat: a #VsgMatrix3@t@.
 * @vec: a #VsgVector3@t@.
 * @result: a #VsgVector3@t@.
 *
 * Performs matrix-vector product of transpose(@mat) by @vec and stores
 * the result in @result. Argument aliasing is allowed.
 */
void vsg_matrix3@t@_vec3mult_T (const VsgMatrix3@t@ *mat,
                                const VsgVector3@t@ *vec,
                                VsgVector3@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (vec != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix3@t@_vec3mult_T_inline (mat, vec, result);
}

/**
 * vsg_matrix3@t@_det:
 * @mat: a #VsgMatrix3@t@
 *
 * Computes determinant of @mat.
 *
 * Returns: the determinant of @mat
 */
@type@ vsg_matrix3@t@_det (const VsgMatrix3@t@ *mat)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (mat != NULL, 0);
#endif

  return vsg_matrix3@t@_det_inline (mat);
}

/**
 * vsg_matrix3@t@_invert:
 * @mat: a #VsgMatrix3@t@
 * @result: a #VsgMatrix3@t@
 *
 * Computes matrix inversion of @mat and stores the result in @result.
 * Argument aliasing is allowed.
 *
 * Returns: a flag indicating success of the inversion.
 */
gboolean vsg_matrix3@t@_invert (const VsgMatrix3@t@ *mat,
                                VsgMatrix3@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (mat != NULL, FALSE);
  g_return_val_if_fail (result != NULL, FALSE);
#endif

  return vsg_matrix3@t@_invert_inline (mat, result);
}

/**
 * vsg_matrix3@t@_component:
 * @mat: a #VsgMatrix3@t@
 * @i: a #guint
 * @j: a #guint
 *
 * Reads component @mat[@i,@j].
 *
 * Returns: @mat[@i,@j]
 */
@type@ vsg_matrix3@t@_component (const VsgMatrix3@t@ *mat,
                                 guint i, guint j)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (mat != NULL, 0.);

  g_return_val_if_fail (i < 3, 0.);
  g_return_val_if_fail (i >= 0, 0.);
  g_return_val_if_fail (j < 3, 0.);
  g_return_val_if_fail (j >= 0, 0.);
#endif

  return vsg_matrix3@t@_component_inline (mat, i, j);
}

/**
 * vsg_matrix3@t@_transpose:
 * @mat: a #VsgMatrix3@t@
 * @result: a #VsgMatrix3@t@
 *
 * Performs matrix transposition on @mat and stores the result in
 * @result. Attribute aliasing is allowed.
 */
void vsg_matrix3@t@_transpose (const VsgMatrix3@t@ *mat, VsgMatrix3@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (mat != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_matrix3@t@_transpose_inline (mat, result);
}

/**
 * vsg_matrix3@t@_write:
 * @mat: a #VsgMatrix3@t@
 * @file: a #FILE pointer
 *
 * Writes @mat components to @file.
 */
void vsg_matrix3@t@_write (const VsgMatrix3@t@ *mat, FILE * file)
{
  int i, j;
  gchar bufx[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *x;

  g_return_if_fail (mat != NULL);
  g_return_if_fail (file != NULL);

  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
        {
/*           fprintf (file, "%@tcode@ ", VSG_MATRIX3@T@_COMP (mat, i, j)); */
          x = g_ascii_dtostr (bufx, G_ASCII_DTOSTR_BUF_SIZE,
                              VSG_MATRIX3@T@_COMP (mat, i, j));
          fprintf (file, "%s ", x);
        }
      fprintf (file, "\n");
    }
}

/**
 * vsg_matrix3@t@_print:
 * @mat: a #VsgMatrix3@t@
 *
 * Writes @mat components to %stdout.
 */
void vsg_matrix3@t@_print (const VsgMatrix3@t@ *mat)
{
  g_return_if_fail (mat != NULL);

  vsg_matrix3@t@_write (mat, stdout);
}

/**
 * VsgMatrix3@t@:
 * @components: matrix (3x3) components.
 *
 * A 3x3 matrix that may represent 2D affine transformations.
 */

/**
 * VSG_MATRIX3@T@_COMP:
 *
 * Returns @mat component of coordinates (@i,@j).
 */
