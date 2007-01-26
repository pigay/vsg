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

#include "vsgvector3@t@.h"
#include "vsgvector3@alt_t@.h"

#include <math.h>

#include <glib/gprintf.h>

#define VSG_VECTOR3@T@_PREALLOC 8192

static GMemChunk *vsg_vector3@t@_mem_chunk = 0;
static guint vsg_vector3@t@_instances_count = 0;

/* static functions: */
static void vsg_vector3@t@_finalize ();
static VsgVector3@t@ *_vector3@t@_alloc ();

static void vsg_vector3@t@_finalize ()
{
  if (vsg_vector3@t@_mem_chunk)
    {
      g_mem_chunk_destroy (vsg_vector3@t@_mem_chunk);
      vsg_vector3@t@_mem_chunk = 0;
    }
}

static VsgVector3@t@ *_vector3@t@_alloc ()
{
  if (!vsg_vector3@t@_mem_chunk)
    {
      vsg_vector3@t@_mem_chunk = g_mem_chunk_create (VsgVector3@t@,
                                                     VSG_VECTOR3@T@_PREALLOC,
                                                     G_ALLOC_ONLY);
    }

  vsg_vector3@t@_instances_count ++;

  return g_chunk_new (VsgVector3@t@, vsg_vector3@t@_mem_chunk);
}

/* private function */
void vsg_vector3@t@_init ()
{
  static gboolean wasinit = FALSE;

  if (! wasinit)
    {
      wasinit = TRUE;
      g_atexit (vsg_vector3@t@_finalize);
    }
}

/* public functions */

/**
 * VSG_TYPE_VECTOR3@T@:
 *
 * The #GBoxed #GType associated to #VsgVector3@t@.
 */

GType vsg_vector3@t@_get_type (void)
{
  static GType vector3@t@_type = 0;

  if (!vector3@t@_type)
    {
      vector3@t@_type =
        g_boxed_type_register_static ("VsgVector3@t@",
                                      (GBoxedCopyFunc) vsg_vector3@t@_clone,
                                      (GBoxedFreeFunc) vsg_vector3@t@_free);

    }

  return vector3@t@_type;
}


/**
 * vsg_vector3@t@_new:
 * @x: abscissa
 * @y: ordinate
 * @z: Z coordinate
 *
 * Allocates a new instance of #VsgVector3@t@. This instance MUST be freed only
 * with vsg_vector3@t@_free().
 *
 * Returns: new #VsgVector3@t@ instance.
 */
VsgVector3@t@ *vsg_vector3@t@_new(@type@ x, @type@ y,
                                  @type@ z)
{
  VsgVector3@t@ *result = _vector3@t@_alloc ();

  vsg_vector3@t@_set(result, x, y, z);

  return result;
}

/**
 * vsg_vector3@t@_free:
 * @vec: instance to deallocate.
 *
 * Frees a #VsgVector3@t@ instance previously allocated with
 * vsg_vector3@t@_new().
 */
void vsg_vector3@t@_free (VsgVector3@t@ *vec)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  g_chunk_free (vec, vsg_vector3@t@_mem_chunk);

  vsg_vector3@t@_instances_count --;

  if (vsg_vector3@t@_instances_count == 0)
    vsg_vector3@t@_finalize ();
}

/**
 * vsg_vector3@t@_set:
 * @vec: a #VsgVector3@t@
 * @x: abscissa
 * @y: ordinate
 * @z: Z coordinate
 *
 * Sets @vec to coordinates: @x, @y.
 */
void vsg_vector3@t@_set(VsgVector3@t@ *vec,
                        @type@ x, @type@ y, @type@ z)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  vec->x = x;
  vec->y = y;
  vec->z = z;
}


/**
 * vsg_vector3@t@_copy:
 * @dst: a #VsgVector3@t@
 * @src: a #VsgVector3@t@
 *
 * Copies @src to @dst.
 */
void vsg_vector3@t@_copy(const VsgVector3@t@ *src, VsgVector3@t@ *dst)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (dst != NULL);
  g_return_if_fail (src != NULL);
#endif

  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;
}

/**
 * vsg_vector3@t@_clone:
 * @src: a #VsgVector2@t@
 *
 * Duplicates @src.
 *
 * Returns: a newly allocated copy of @src.
 */
VsgVector3@t@ *vsg_vector3@t@_clone (const VsgVector3@t@ *src)
{
  VsgVector3@t@ *dst;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (src != NULL, NULL);
#endif

  dst = _vector3@t@_alloc ();

  vsg_vector3@t@_copy (src, dst);

  return dst;
}

/**
 * vsg_vector3@t@_square_norm:
 * @vec: a #VsgVector3@t@
 *
 * Computes square of Euclidean norm of @vec: x*x+y*y+z*z.
 *
 * Returns square of @vec norm.
 */
@type@ vsg_vector3@t@_square_norm(const VsgVector3@t@ *vec)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, 0);
#endif

  return vsg_vector3@t@_dotp(vec,vec);
}

/**
 * vsg_vector3@t@_norm:
 * @vec: a #VsgVector3@t@
 *
 * Computes Euclidean norm of @vec sqrt(x*x+y*y+z*z).
 *
 * Returns square of @vec norm.
 */
@type@ vsg_vector3@t@_norm(const VsgVector3@t@ *vec)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, 0);
#endif

  return sqrt(vsg_vector3@t@_dotp(vec,vec));
}

/**
 * vsg_vector3@t@_dist:
 * @vec: a #VsgVector3@t@
 * @other: a #VsgVector3@t@
 *
 * Computes Euclidean norm of the substraction between @vec and @other.
 *
 * Returns square of @vec-@other norm.
 */
@type@ vsg_vector3@t@_dist (const VsgVector3@t@ *vec,
                            const VsgVector3@t@ *other)
{
  VsgVector3@t@ tmp;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, 0);
  g_return_val_if_fail (other != NULL, 0);
#endif

  vsg_vector3@t@_sub (vec, other, &tmp);

  return vsg_vector3@t@_norm (&tmp);
}

/**
 * vsg_vector3@t@_normalize:
 * @vec: a #VsgVector3@t@
 *
 * Modifies @vec so that its Euclidean norm becomes %1. Former direction
 * of @vec is unchanged.
 */
void vsg_vector3@t@_normalize(VsgVector3@t@ *vec)
{
  @type@ n;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  n = vsg_vector3@t@_norm(vec);

  if (n == 0.) return;

  vsg_vector3@t@_scalp(vec, 1./n, vec);
}

/**
 * vsg_vector3@t@_scalp:
 * @vec: a #VsgVector3@t@
 * @scal: a #@type@
 * @res: a #VsgVector3@t@ holding the result.
 *
 * Computes scalar product of @vec by @scal and stores the result in
 * @res. Argument aliasing is allowed.
 */
void vsg_vector3@t@_scalp(const VsgVector3@t@ *vec, @type@ scal,
                          VsgVector3@t@ *res)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
  g_return_if_fail (res != NULL);
#endif

  res->x = vec->x * scal;
  res->y = vec->y * scal;
  res->z = vec->z * scal;
}

/**
 * vsg_vector3@t@_dotp:
 * @vec: a #VsgVector3@t@
 * @other: a #VsgVector3@t@
 *
 * Computes the dot product of @vec by @other.
 *
 * Returns: result of the dot product.
 */
@type@ vsg_vector3@t@_dotp(const VsgVector3@t@ *vec,
                           const VsgVector3@t@ *other)
{
  @type@ result;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, 0);
  g_return_val_if_fail (other != NULL, 0);
#endif

  result = vec->x * other->x;
  result += vec->y * other->y;
  result += vec->z * other->z;

  return result;
}

/**
 * vsg_vector3@t@_vecp:
 * @vec: a #VsgVector3@t@
 * @other: a #VsgVector3@t@
 * @result: a #VsgVector3@t@
 *
 * Computes the cross product of @vec by @other. Result is stored in @result
 * and argument aliasing is allowed
 */
void vsg_vector3@t@_vecp(const VsgVector3@t@ *vec,
                         const VsgVector3@t@ *other,
                         VsgVector3@t@ *result)
{
  VsgVector3@t@ tmp; /* avoid argument aliasing */

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  tmp.x = vec->y * other->z - vec->z * other->y;
  tmp.y = vec->z * other->x - vec->x * other->z;
  tmp.z = vec->x * other->y - vec->y * other->x;

  vsg_vector3@t@_copy(&tmp, result);
}

/**
 * vsg_vector3@t@_add:
 * @vec: a #VsgVector3@t@
 * @other: a #VsgVector3@t@
 * @result: a #VsgVector3@t@
 *
 * Computes vectorial addition of @vec and @other in @result. Argument aliasing
 * is allowed.
 */
void vsg_vector3@t@_add(const VsgVector3@t@ *vec,
                        const VsgVector3@t@ *other,
                        VsgVector3@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  result->x = vec->x + other->x;
  result->y = vec->y + other->y;
  result->z = vec->z + other->z;
}

/**
 * vsg_vector3@t@_sub:
 * @vec: a #VsgVector3@t@
 * @other: a #VsgVector3@t@
 * @result: a #VsgVector3@t@
 *
 * Computes vectorial substraction of @vec and @other in @result. Argument
 * aliasing is allowed.
 */
void vsg_vector3@t@_sub(const VsgVector3@t@ *vec,
                        const VsgVector3@t@ *other,
                        VsgVector3@t@ *result)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  result->x = vec->x - other->x;
  result->y = vec->y - other->y;
  result->z = vec->z - other->z;
}

/**
 * vsg_vector3@t@_lerp:
 * @vec: a #VsgVector3@t@
 * @other: a #VsgVector3@t@
 * @factor: a #@type@
 * @result: a #VsgVector3@t@
 *
 * Computes linear interpolation between @vec and @other with a factor of
 * @factor. Formula is: @result = (1-@factor)*@vec + @factor*@other. Argument aliasing is
 * allowed.
 */
void vsg_vector3@t@_lerp (const VsgVector3@t@ *vec,
                          const VsgVector3@t@ *other,
                          @type@ factor,
                          VsgVector3@t@ *result)
{
  @type@ one_minus_factor = (1.-factor);

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
  g_return_if_fail (other != NULL);
  g_return_if_fail (result != NULL);
#endif

  result->x = one_minus_factor * vec->x + factor * other->x;
  result->y = one_minus_factor * vec->y + factor * other->y;
  result->z = one_minus_factor * vec->z + factor * other->z;
}


/**
 * vsg_vector3@t@_copy@alt_t@:
 * @dst: a #VsgVector3@alt_t@
 * @src: a #VsgVector3@t@
 *
 * Copies @src to @dst, taking care of type conversion.
 */
void vsg_vector3@t@_copy@alt_t@ (const VsgVector3@t@ *src,
                                 VsgVector3@alt_t@ *dst)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (dst != NULL);
  g_return_if_fail (src != NULL);
#endif

  dst->x = (@alt_type@) src->x;
  dst->y = (@alt_type@) src->y;
  dst->z = (@alt_type@) src->z;
}

/**
 * vsg_vector3@t@_is_zero:
 * @vec: a #VsgVector3@t@
 *
 * Tests if @vec is at the origin.
 *
 * Returns %true if @vec is origin, %false otherwise
 */
gboolean vsg_vector3@t@_is_zero(const VsgVector3@t@ *vec)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (vec != NULL, FALSE);
#endif

  return vsg_vector3@t@_square_norm(vec) == 0.;
}

/**
 * vsg_vector3@t@_write:
 * @vec: a #VsgVector3@t@
 * @file: a #FILE pointer
 *
 * Writes @vec coordinates to @file.
 */
void vsg_vector3@t@_write(const VsgVector3@t@ *vec, FILE *file)
{
  gchar bufx[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *x;
  gchar bufy[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *y;
  gchar bufz[G_ASCII_DTOSTR_BUF_SIZE];
  gchar *z;

  g_return_if_fail(vec != NULL);
  g_return_if_fail(file != NULL);

/*   fprintf(file, "[%@tcode@,%@tcode@,%@tcode@]", vec->x, vec->y, vec->z); */

  /* avoid locale problems */
  x = g_ascii_dtostr (bufx, G_ASCII_DTOSTR_BUF_SIZE, vec->x);
  y = g_ascii_dtostr (bufy, G_ASCII_DTOSTR_BUF_SIZE, vec->y);
  z = g_ascii_dtostr (bufz, G_ASCII_DTOSTR_BUF_SIZE, vec->z);
  fprintf(file, "[%s,%s,%s]", x, y, z);
}

/**
 * vsg_vector3@t@_print:
 * @vec: a #VsgVector3@t@
 *
 * Writes @vec coordinates to %stdout.
 */
void vsg_vector3@t@_print(const VsgVector3@t@ *vec)
{
  g_return_if_fail(vec != NULL);

  vsg_vector3@t@_write(vec, stdout);
}

/**
 * vsg_vector3@t@_to_spherical_internal:
 * @vec: a #VsgVector3@t@
 * @r: radius
 * @cost: cos (theta)
 * @sint: sin (theta)
 * @cosp: cos (phi)
 * @sinp: sin (phi)
 *
 * Computes @vec spherical preliminary coordinates (sine and cosine).
 */
void vsg_vector3@t@_to_spherical_internal (const VsgVector3@t@ *vec,
					   @type@ *r,
					   @type@ *cost, @type@ *sint,
					   @type@ *cosp, @type@ *sinp)
{
  @type@ r0;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

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

/**
 * vsg_vector3@t@_to_spherical:
 * @vec: a #VsgVector3@t@
 * @r: radius
 * @theta: polar (colatitudinal) angle.
 * @phi: azimuthal (longitudinal) angle.
 *
 * Computes @vec spherical preliminary coordinates.
 *
 * Note: This spherical angle convention is the one that is normally used in
 * physics.
 */
void vsg_vector3@t@_to_spherical (const VsgVector3@t@ *vec,
				  @type@ *r, @type@ *theta, @type@ *phi)
{
  @type@ cost, sint, cosp, sinp;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  vsg_vector3@t@_to_spherical_internal (vec, r, &cost, &sint, &cosp, &sinp);

  *theta = acos (cost); /* theta is always positive */

  *phi = acos (cosp);
  if (sinp < 0.) *phi = - *phi;
}


/**
 * vsg_vector3@t@_from_spherical_internal:
 * @vec: a #VsgVector3@t@
 * @r: radius
 * @cost: cos (theta).
 * @sint: sin (theta).
 * @cosp: cos (phi)
 * @sinp: sin (phi)
 *
 * Sets @vec to cartesian coordinates coorresponding to @r, @cost, @sint,
 * @cosp and @sinp spherical preliminary coordinates.
 */
void vsg_vector3@t@_from_spherical_internal (VsgVector3@t@ *vec,
					     @type@ r,
					     @type@ cost, @type@ sint,
					     @type@ cosp, @type@ sinp)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  vec->x = r*cosp*sint;
  vec->y = r*sinp*sint;
  vec->z = r*cost;
}

/**
 * vsg_vector3@t@_from_spherical:
 * @vec: a #VsgVector3@t@
 * @r: radius
 * @theta: polar (colatitudinal) angle.
 * @phi: azimuthal (longitudinal) angle.
 *
 * Sets @vec to cartesian coordinates coorresponding to @r, @theta and @phi
 * spherical coordinates.
 *
 * Note: This spherical angle convention is the one that is normally used in
 * physics.
 */
void vsg_vector3@t@_from_spherical (VsgVector3@t@ *vec,
				    @type@ r, @type@ theta, @type@ phi)
{
  @type@ cost, sint, cosp, sinp;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (vec != NULL);
#endif

  cost = cos (theta);
  sint = sin (theta);

  cosp = cos (phi);
  sinp = sin (phi);

  vsg_vector3@t@_from_spherical_internal (vec, r, cost, sint, cosp, sinp);
}

/**
 * vsg_vector3@t@_orient:
 * @pa: a #VsgVector3@t@
 * @pb: a #VsgVector3@t@
 * @pc: a #VsgVector3@t@
 * @pd: a #VsgVector3@t@
 *
 * Computes Orient3D predicate.
 *
 * Returns: a positive value if the points @pa, @pb , @pc and @pd occur in a
 * direct order (they form a directed trihedron), negative value otherwise.
 */
@type@ vsg_vector3@t@_orient (const VsgVector3@t@ *pa,
                              const VsgVector3@t@ *pb,
                              const VsgVector3@t@ *pc,
                              const VsgVector3@t@ *pd)
{
  @type@ a00, a01, a02, a10, a11, a12, a20, a21, a22;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (pa != NULL, 0);
  g_return_val_if_fail (pb != NULL, 0);
  g_return_val_if_fail (pc != NULL, 0);
  g_return_val_if_fail (pd != NULL, 0);
#endif

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

/**
 * vsg_vector3@t@_insphere:
 * @pa: a #VsgVector3@t@
 * @pb: a #VsgVector3@t@
 * @pc: a #VsgVector3@t@
 * @pd: a #VsgVector3@t@
 * @pe: a #VsgVector3@t@
 *
 * Computes wether @pe is in @pa,@pb,@pc,@pd sphere or not.
 *
 * Returns: a positive value if @pd is in the circle, a negative value
 * otherwise.
 */
@type@ vsg_vector3@t@_insphere (const VsgVector3@t@ *pa,
                                const VsgVector3@t@ *pb,
                                const VsgVector3@t@ *pc,
                                const VsgVector3@t@ *pd,
                                const VsgVector3@t@ *pe)
{
  @type@ a00, a01, a02, a03,
    a10, a11, a12, a13,
    a20, a21, a22, a23,
    a30, a31, a32, a33;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (pa != NULL, 0);
  g_return_val_if_fail (pb != NULL, 0);
  g_return_val_if_fail (pc != NULL, 0);
  g_return_val_if_fail (pd != NULL, 0);
  g_return_val_if_fail (pe != NULL, 0);
#endif

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

/**
 * vsg_vector3@t@_vector3@t@_locfunc:
 * @candidate: a #VsgVector3@t@
 * @center: a #VsgVector3@t@
 *
 * Computes location of @cadidate relatively to @center position. Return
 * value is a 3D generalization of 1D point comparison.
 *
 * Returns: a #vsgloc3 location
 */
vsgloc3 vsg_vector3@t@_vector3@t@_locfunc (const VsgVector3@t@ *candidate,
                                           const VsgVector3@t@ *center)
{
  vsgloc3 res = 0;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (candidate != NULL, 0);
  g_return_val_if_fail (center != NULL, 0);
#endif

  if (candidate->x >= center->x)
    res |= VSG_LOC3_X;

  if (candidate->y >= center->y)
    res |= VSG_LOC3_Y;

  if (candidate->z >= center->z)
    res |= VSG_LOC3_Z;

  return res;
}

/**
 * vsg_vector3@t@_vector3@alt_t@_locfunc:
 * @candidate: a #VsgVector3@t@
 * @center: a #VsgVector3@alt_t@
 *
 * Computes location of @cadidate relatively to @center position. Return
 * value is a 3D generalization of 1D point comparison. This function
 * takes care of floating point conversions between @type@ and @alt_type@.
 *
 * Returns: a #vsgloc3 location
 */
vsgloc3 vsg_vector3@t@_vector3@alt_t@_locfunc (const VsgVector3@t@ *candidate,
                                               const VsgVector3@alt_t@ *center)
{
  vsgloc3 res = 0;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (candidate != NULL, 0);
  g_return_val_if_fail (center != NULL, 0);
#endif

  if (candidate->x >= center->x)
    res |= VSG_LOC3_X;

  if (candidate->y >= center->y)
    res |= VSG_LOC3_Y;

  if (candidate->z >= center->z)
    res |= VSG_LOC3_Z;

  return res;
}

/**
 * VsgVector3@t@:
 * @x: abscissa.
 * @y: ordinate.
 * @z: Z coordinate.
 *
 * A 3D Point.
 */

/**
 * VSG_VECTOR3@T@_COMP:
 * @vec: a #VsgVector3@t@
 * @i : an integer
 *
 * Provides access to the @i th coordinate of @vec.
 *
 */
