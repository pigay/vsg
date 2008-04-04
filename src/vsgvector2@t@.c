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

#include "vsg-config.h"

#include "vsgvector2@t@.h"
#include "vsgvector2@alt_t@.h"

#define VSG_NO_IMPLICIT_INLINE
#include "vsgvector2@t@-inline.h"
#include "vsgvector2@alt_t@-inline.h"

#include <math.h>

#include <glib/gprintf.h>

#define VSG_VECTOR2@T@_PREALLOC 8192

static GMemChunk *vsg_vector2@t@_mem_chunk = 0;
static guint vsg_vector2@t@_instances_count = 0;

/* static functions: */
static void vsg_vector2@t@_finalize ();
static VsgVector2@t@ *_vector2@t@_alloc ();

static void vsg_vector2@t@_finalize ()
{
  if (vsg_vector2@t@_mem_chunk)
    {
      g_mem_chunk_destroy (vsg_vector2@t@_mem_chunk);
      vsg_vector2@t@_mem_chunk = 0;
    }
}

static VsgVector2@t@ *_vector2@t@_alloc ()
{
  if (!vsg_vector2@t@_mem_chunk)
    {
      vsg_vector2@t@_mem_chunk = g_mem_chunk_create (VsgVector2@t@,
                                                     VSG_VECTOR2@T@_PREALLOC,
                                                     G_ALLOC_ONLY);
    }

  vsg_vector2@t@_instances_count ++;

  return g_chunk_new (VsgVector2@t@, vsg_vector2@t@_mem_chunk);
}

/* private function */
void vsg_vector2@t@_init ()
{
  static gboolean wasinit = FALSE;

  if (! wasinit)
    {
      wasinit = TRUE;
      g_atexit (vsg_vector2@t@_finalize);
    }
}

/* public functions */

/**
 * VSG_TYPE_VECTOR2@T@:
 *
 * The #GBoxed #GType associated to #VsgVector2@t@.
 */

GType vsg_vector2@t@_get_type (void)
{
  static GType vector2@t@_type = 0;

  if (!vector2@t@_type)
    {
      vector2@t@_type =
        g_boxed_type_register_static ("VsgVector2@t@",
                                      (GBoxedCopyFunc) vsg_vector2@t@_clone,
                                      (GBoxedFreeFunc) vsg_vector2@t@_free);

    }

  return vector2@t@_type;
}

/**
 * vsg_vector2@t@_new:
 * @x: abscissa
 * @y: ordinate
 *
 * Allocates a new instance of #VsgVector2@t@. This instance MUST be freed only
 * with vsg_vector2@t@_free().
 *
 * Returns: new #VsgVector2@t@ instance.
 */
VsgVector2@t@ *vsg_vector2@t@_new (@type@ x, @type@ y)
{
  VsgVector2@t@ *result = _vector2@t@_alloc ();

  vsg_vector2@t@_set_inline (result, x, y);

  return result;
}

/**
 * vsg_vector2@t@_free:
 * @vec: instance to deallocate.
 *
 * Frees a #VsgVector2@t@ instance previously allocated with
 * vsg_vector2@t@_new().
 */
void vsg_vector2@t@_free (VsgVector2@t@ *vec)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  g_chunk_free (vec, vsg_vector2@t@_mem_chunk);

  vsg_vector2@t@_instances_count --;

  if (vsg_vector2@t@_instances_count == 0)
    vsg_vector2@t@_finalize ();
}

/**
 * vsg_vector2@t@_set:
 * @vec: a #VsgVector2@t@
 * @x: abscissa
 * @y: ordinate
 *
 * Sets @vec to coordinates: @x, @y.
 */
void vsg_vector2@t@_set(VsgVector2@t@ *vec,
                        @type@ x, @type@ y)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  vsg_vector2@t@_set_inline (vec, x, y);
}


/**
 * vsg_vector2@t@_copy:
 * @dst: a #VsgVector2@t@
 * @src: a #VsgVector2@t@
 *
 * Copies @src to @dst.
 */
void vsg_vector2@t@_copy(const VsgVector2@t@ *src, VsgVector2@t@ *dst)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (src != NULL);
  g_return_if_fail (dst != NULL);
#endif

  vsg_vector2@t@_copy_inline (src, dst);
}

/**
 * vsg_vector2@t@_clone:
 * @src: a #VsgVector2@t@.
 *
 * Duplicates @src.
 *
 * Returns: a newly allocated copy of @src.
 */
VsgVector2@t@ *vsg_vector2@t@_clone (const VsgVector2@t@ *src)
{
  VsgVector2@t@ *dst;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (src != NULL, NULL);
#endif

  dst = _vector2@t@_alloc ();
  vsg_vector2@t@_copy_inline (src, dst);

  return dst;
}

/**
 * vsg_vector2@t@_square_norm:
 * @vec: a #VsgVector2@t@.
 *
 * Computes square of Euclidean norm of @vec: x*x+y*y.
 *
 * Returns square of @vec norm.
 */
@type@ vsg_vector2@t@_square_norm (const VsgVector2@t@ *vec)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, 0.);
#endif

  return vsg_vector2@t@_square_norm_inline (vec);
}

/**
 * vsg_vector2@t@_norm:
 * @vec: a #VsgVector2@t@
 *
 * Computes Euclidean norm of @vec sqrt(x*x+y*y).
 *
 * Returns @vec norm.
 */
@type@ vsg_vector2@t@_norm (const VsgVector2@t@ *vec)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, 0.);
#endif

  return vsg_vector2@t@_norm_inline (vec);
}

/**
 * vsg_vector2@t@_dist:
 * @vec: a #VsgVector2@t@
 * @other: a #VsgVector2@t@
 *
 * Computes Euclidean norm of the substraction between @vec and @other.
 *
 * Returns square of @vec-@other norm.
 */
@type@ vsg_vector2@t@_dist (const VsgVector2@t@ *vec,
                            const VsgVector2@t@ *other)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, 0.);
  g_return_val_if_fail (other != NULL, 0.);
#endif

  return vsg_vector2@t@_dist_inline (vec, other);
}

/**
 * vsg_vector2@t@_normalize:
 * @vec: a #VsgVector2@t@
 *
 * Modifies @vec so that its Euclidean norm becomes %1. Former direction
 * of @vec is unchanged.
 */
void vsg_vector2@t@_normalize (VsgVector2@t@ *vec)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  vsg_vector2@t@_normalize_inline (vec);
}

/**
 * vsg_vector2@t@_scalp:
 * @vec: a #VsgVector2@t@
 * @scal: a #@type@
 * @res: a #VsgVector2@t@ holding the result.
 *
 * Computes scalar product of @vec by @scal and stores the result in
 * @res. Argument aliasing is allowed.
 */
void vsg_vector2@t@_scalp (const VsgVector2@t@ *vec, @type@ scal,
                           VsgVector2@t@ *res)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
  g_return_if_fail (res != NULL);
#endif

  vsg_vector2@t@_scalp_inline (vec,scal, res);
}

/**
 * vsg_vector2@t@_dotp:
 * @vec: a #VsgVector2@t@
 * @other: a #VsgVector2@t@
 *
 * Computes the dot product of @vec by @other.
 *
 * Returns: result of the dot product.
 */
@type@ vsg_vector2@t@_dotp (const VsgVector2@t@ *vec,
                            const VsgVector2@t@ *other)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, 0.);
  g_return_val_if_fail (other != NULL, 0.);
#endif

  return vsg_vector2@t@_dotp_inline (vec, other);
;
}

/**
 * vsg_vector2@t@_vecp:
 * @vec: a #VsgVector2@t@
 * @other: a #VsgVector2@t@
 *
 * Computes the cross product of @vec by @other.
 *
 * Returns: result of the cross product.
 */
@type@ vsg_vector2@t@_vecp (const VsgVector2@t@ *vec,
                            const VsgVector2@t@ *other)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, 0.);
  g_return_val_if_fail (other != NULL, 0.);
#endif

  return vsg_vector2@t@_vecp_inline (vec, other);
}

/**
 * vsg_vector2@t@_add:
 * @vec: a #VsgVector2@t@
 * @other: a #VsgVector2@t@
 * @result: a #VsgVector2@t@
 *
 * Computes vectorial addition of @vec and @other in @result. Argument aliasing
 * is allowed.
 */
void vsg_vector2@t@_add (const VsgVector2@t@ *vec,
                         const VsgVector2@t@ *other,
                         VsgVector2@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_vector2@t@_add_inline (vec, other, result);
}

/**
 * vsg_vector2@t@_sub:
 * @vec: a #VsgVector2@t@
 * @other: a #VsgVector2@t@
 * @result: a #VsgVector2@t@
 *
 * Computes vectorial substraction of @vec and @other in @result. Argument
 * aliasing is allowed.
 */
void vsg_vector2@t@_sub (const VsgVector2@t@ *vec,
                         const VsgVector2@t@ *other,
                         VsgVector2@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_vector2@t@_sub_inline (vec, other, result);
}


/**
 * vsg_vector2@t@_lerp:
 * @vec: a #VsgVector2@t@
 * @other: a #VsgVector2@t@
 * @factor: a #@type@
 * @result: a #VsgVector2@t@
 *
 * Computes linear interpolation between @vec and @other with a factor of
 * @factor. Formula is: @result = (1-@factor)*@vec + @factor*@other. Argument aliasing is
 * allowed.
 */
void vsg_vector2@t@_lerp (const VsgVector2@t@ *vec,
                          const VsgVector2@t@ *other,
                          @type@ factor,
                          VsgVector2@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  vsg_vector2@t@_lerp_inline (vec, other, factor, result);
}

/**
 * vsg_vector2@t@_copy@alt_t@:
 * @dst: a #VsgVector2@alt_t@
 * @src: a #VsgVector2@t@
 *
 * Copies @src to @dst, taking care of type conversion.
 */
void vsg_vector2@t@_copy@alt_t@ (const VsgVector2@t@ *src,
                                 VsgVector2@alt_t@ *dst)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (dst != NULL);
  g_return_if_fail (src != NULL);
#endif

  vsg_vector2@t@_copy@alt_t@_inline (src, dst);
}

/**
 * vsg_vector2@t@_is_zero:
 * @vec: a #VsgVector2@t@
 *
 * Tests if @vec is at the origin.
 *
 * Returns %true if @vec is origin, %false otherwise
 */
gboolean vsg_vector2@t@_is_zero (const VsgVector2@t@ *vec)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, FALSE);
#endif

  return vsg_vector2@t@_is_zero_inline (vec);
}

/**
 * vsg_vector2@t@_write:
 * @vec: a #VsgVector2@t@
 * @file: a #FILE pointer
 *
 * Writes @vec coordinates to @file.
 */
void vsg_vector2@t@_write(const VsgVector2@t@ *vec, FILE *file)
{
  gchar bufx[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *x;
  gchar bufy[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *y;

  g_return_if_fail(vec != NULL);
  g_return_if_fail(file != NULL);

  /* fprintf(file, "[%@tcode@,%@tcode@]", vec->x, vec->y); */

  /* avoid locale problems */
  x = g_ascii_dtostr (bufx, G_ASCII_DTOSTR_BUF_SIZE, vec->x);
  y = g_ascii_dtostr (bufy, G_ASCII_DTOSTR_BUF_SIZE, vec->y);
  fprintf(file, "[%s,%s]", x, y);
}

/**
 * vsg_vector2@t@_print:
 * @vec: a #VsgVector2@t@
 *
 * Writes @vec coordinates to %stdout.
 */
void vsg_vector2@t@_print(const VsgVector2@t@ *vec)
{
  vsg_vector2@t@_write(vec, stdout);
}

/**
 * vsg_vector2@t@_to_polar_internal:
 * @vec: a #VsgVector2@t@
 * @r: radius
 * @cost: cos (theta)
 * @sint: sin (theta)
 *
 * Computes @vec polar preliminary coordinates (sine and cosine).
 */
void vsg_vector2@t@_to_polar_internal (const VsgVector2@t@ *vec,
                                       @type@ *r, @type@ *cost, @type@ *sint)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  vsg_vector2@t@_to_polar_internal_inline (vec, r, cost, sint);
}

/**
 * vsg_vector2@t@_to_polar:
 * @vec: a #VsgVector2@t@
 * @r: radius
 * @theta: angle.
 *
 * Computes @vec polar preliminary coordinates.
 */
void vsg_vector2@t@_to_polar (const VsgVector2@t@ *vec,
                              @type@ *r, @type@ *theta)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  vsg_vector2@t@_to_polar_inline (vec, r, theta);
}

/**
 * vsg_vector2@t@_from_polar_internal:
 * @vec: a #VsgVector2@t@
 * @r: radius
 * @cost: cos (theta).
 * @sint: sin (theta).
 *
 * Sets @vec to cartesian coordinates coorresponding to @r, @cost and @sint
 * polar preliminary coordinates.
 */
void vsg_vector2@t@_from_polar_internal (VsgVector2@t@ *vec,
                                         @type@ r, @type@ cost, @type@ sint)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  vsg_vector2@t@_from_polar_internal_inline (vec, r, cost, sint);
}

/**
 * vsg_vector2@t@_from_polar:
 * @vec: a #VsgVector2@t@
 * @r: radius
 * @theta: angle.
 *
 * Sets @vec to cartesian coordinates coorresponding to @r @theta polar
 * coordinates.
 */
void vsg_vector2@t@_from_polar (VsgVector2@t@ *vec,
                                @type@ r, @type@ theta)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  vsg_vector2@t@_from_polar_inline (vec, r,  theta);
}

/**
 * vsg_vector2@t@_orient:
 * @pa: a #VsgVector2@t@
 * @pb: a #VsgVector2@t@
 * @pc: a #VsgVector2@t@
 *
 * Computes Orient2D predicate.
 *
 * Returns: a positive value if the points @pa, @pb and @pc are in clockwise
 * order, negative value otherwise.
 */
@type@ vsg_vector2@t@_orient (const VsgVector2@t@ *pa,
                              const VsgVector2@t@ *pb,
                              const VsgVector2@t@ *pc)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (pa != NULL, 0.);
  g_return_val_if_fail (pb != NULL, 0.);
  g_return_val_if_fail (pc != NULL, 0.);
#endif

  return vsg_vector2@t@_orient_inline (pa, pb, pc);
}

/**
 * vsg_vector2@t@_incircle:
 * @pa: a #VsgVector2@t@
 * @pb: a #VsgVector2@t@
 * @pc: a #VsgVector2@t@
 * @pd: a #VsgVector2@t@
 *
 * Computes wether @pd is in @pa,@pb,@pc circle or not.
 *
 * Returns: a positive value if @pd is in the circle, a negative value
 * otherwise.
 */
@type@ vsg_vector2@t@_incircle (const VsgVector2@t@ *pa,
                                const VsgVector2@t@ *pb,
                                const VsgVector2@t@ *pc,
                                const VsgVector2@t@ *pd)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (pa != NULL, 0.);
  g_return_val_if_fail (pb != NULL, 0.);
  g_return_val_if_fail (pc != NULL, 0.);
  g_return_val_if_fail (pd != NULL, 0.);
#endif

  return vsg_vector2@t@_incircle_inline (pa, pb, pc, pd);
}

/**
 * vsg_vector2@t@_vector2@t@_locfunc:
 * @candidate: a #VsgVector2@t@
 * @center: a #VsgVector2@t@
 *
 * Computes location of @candidate relatively to @center position. Return
 * value is a 2D generalization of 1D point comparison.
 *
 * Returns: a #vsgloc2 location
 */
vsgloc2 vsg_vector2@t@_vector2@t@_locfunc (const VsgVector2@t@ *candidate,
                                           const VsgVector2@t@ *center)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (candidate != NULL, 0);
  g_return_val_if_fail (center != NULL, 0);
#endif

  return vsg_vector2@t@_vector2@t@_locfunc_inline (candidate, center);
}

/**
 * vsg_vector2@t@_vector2@alt_t@_locfunc:
 * @candidate: a #VsgVector2@t@
 * @center: a #VsgVector2@alt_t@
 *
 * Computes location of @cadidate relatively to @center position. Return
 * value is a 2D generalization of 1D point comparison. This function
 * takes care of floating point conversions between @type@ and @alt_type@.
 *
 * Returns: a #vsgloc2 location
 */
vsgloc2 vsg_vector2@t@_vector2@alt_t@_locfunc (const VsgVector2@t@ *candidate,
                                               const VsgVector2@alt_t@ *center)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (candidate != NULL, 0);
  g_return_val_if_fail (center != NULL, 0);
#endif

  return vsg_vector2@t@_vector2@alt_t@_locfunc_inline (candidate, center);
}

/**
 * VsgVector2@t@:
 * @x: abscissa.
 * @y: ordinate.
 *
 * A 2D Point.
 */

/**
 * VSG_VECTOR2@T@_COMP:
 * @vec: a #VsgVector2@t@
 * @i : an integer
 *
 * Provides access to the @i th coordinate of @vec.
 *
 */
