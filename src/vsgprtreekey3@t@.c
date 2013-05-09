#include <vsg-config.h>

#include "vsgprtreekey3@t@.h"

#include <string.h>

#include <glib/gprintf.h>

/**
 * SECTION: vsgprtreekey3@t@
 * @short_description: 3D localization key for #VsgPRTree3@t@.
 */

/**
 * VsgPRTreeKey3@t@:
 * @x : A @key_type@ for storing x binary coordinate
 * @y : A @key_type@ for storing y binary coordinate
 * @z : A @key_type@ for storing y binary coordinate
 * @depth : the depth of the key. It's the number of bits to take in account
 * for @x, @y and @z.
 *
 * A structure for manupilating VsgPRTree3@t@ nodes coordinates.
 */

/**
 * VSG_PRTREE_KEY3@T@_BITS:
 *
 * number of bits in a #VsgPRTreeKey3@t@.
 */

/**
 * VSG_PRTREE_KEY3@T@_INDEX_MASK:
 * @index: an integer
 *
 * an integer mask with @index first bits set to 1.
 */

/**
 * VSG_PRTREE_KEY3@T@_SIZE:
 *
 * size of a #VsgPRTreeKey3@t@.
 */

typedef struct _BitMaskData BitMaskData;

struct _BitMaskData {
  @key_type@ mask;
  guint8 base;
};

/* a list of binary sieves used to analyze keys */
static guint8 _bitmasks_number = 0;
static BitMaskData *_bitmasks = NULL;

/* list of number of sieves for any  */
static guint8 _number_of_sieves[VSG_PRTREE_KEY3@T@_BITS+1];

static void _exitfunc ()
{
  if (_bitmasks != NULL)
    g_free (_bitmasks);
}

void vsgprtree_key3@t@_init ()
{
  if (G_UNLIKELY (_bitmasks_number == 0))
    {
      guint8 ks = VSG_PRTREE_KEY3@T@_BITS;
      gint8 i, j;
      @key_type@ mask = ~0;

      while (ks>>(_bitmasks_number+1) != 0) _bitmasks_number++;

      _bitmasks = g_malloc (_bitmasks_number * sizeof (BitMaskData));

      for (i=_bitmasks_number-1; i>=0; i--)
        {
          ks >>= 1; /* ks = 1 << i */
          mask ^= (mask << ks);
          _bitmasks[i].mask = ~ mask;
          _bitmasks[i].base = 1 << (i);
/*           g_printerr ("num=%u offset=%u mask mask=%#@kmod@x\n", */
/*                       i, _bitmasks[i].base, _bitmasks[i].mask); */
        }

      _number_of_sieves[0] = 0;
      _number_of_sieves[1] = 0;
      for (i=0; i<_bitmasks_number; i++)
        {
          for (j=_bitmasks[i].base/2+1; j<=_bitmasks[i].base; j ++)
            {
              _number_of_sieves[j] = i+1;
/*               g_printerr ("powers %d %d\n", j,  _number_of_sieves[j]); */
            }
        }

      g_atexit (_exitfunc);
   }
}

#ifdef VSG_HAVE_MPI
/**
 * VSG_MPI_TYPE_PRTREE_KEY3@T@:
 *
 * The #MPI_Datatype associated to #VsgPRTreeKey3@t@.
 */

MPI_Datatype vsg_prtree_key3@t@_get_mpi_type (void)
{
  static MPI_Datatype prtree_key3@t@_mpi_type = MPI_DATATYPE_NULL;

  if (prtree_key3@t@_mpi_type == MPI_DATATYPE_NULL)
    {
      gint blocks = 4;
      gint lens[] = {1, 1, 1, 1};
      MPI_Aint indices[] = {
        G_STRUCT_OFFSET (VsgPRTreeKey3@t@, x),
        G_STRUCT_OFFSET (VsgPRTreeKey3@t@, y),
        G_STRUCT_OFFSET (VsgPRTreeKey3@t@, z),
        G_STRUCT_OFFSET (VsgPRTreeKey3@t@, depth),
      };
      MPI_Datatype types[] = {
        @KEY_MPI_DATATYPE@,
        @KEY_MPI_DATATYPE@,
        @KEY_MPI_DATATYPE@,
        MPI_BYTE,
      };

      MPI_Type_struct (blocks, lens, indices, types, &prtree_key3@t@_mpi_type);
      MPI_Type_commit (&prtree_key3@t@_mpi_type);
    }

  return prtree_key3@t@_mpi_type;
}
#endif

static void _key_copy (VsgPRTreeKey3@t@ *dst, VsgPRTreeKey3@t@ *src)
{
  memcpy (dst, src, sizeof (VsgPRTreeKey3@t@));
}

static inline @key_type@ _key_coord_scale_up (@key_type@ kc, guint8 offset)
{
  return kc << offset;
}

static void _key_scale_up (VsgPRTreeKey3@t@ *key, guint8 offset,
                           VsgPRTreeKey3@t@ *result)
{
  result->x = _key_coord_scale_up (key->x, offset);
  result->y = _key_coord_scale_up (key->y, offset);
  result->z = _key_coord_scale_up (key->z, offset);
  result->depth = key->depth + offset;
}

static inline @key_type@ _key_coord_scale_down (@key_type@ kc, guint8 offset)
{
  return kc >> offset;
}

static void _key_scale_down (VsgPRTreeKey3@t@ *key, guint8 offset,
                             VsgPRTreeKey3@t@ *result)
{
  result->x = _key_coord_scale_down (key->x, offset);
  result->y = _key_coord_scale_down (key->y, offset);
  result->z = _key_coord_scale_down (key->z, offset);
  result->depth = (guint8) MAX (0, ((gint8) key->depth) - offset);
}

static guint8 _single_key_first_true_bit (@key_type@ key, guint8 maxdepth)
{
  gint8 sieves_number;
  gint8 i, ret;

  if (!key) return 0;

  sieves_number = _number_of_sieves[maxdepth]-1;
  ret = VSG_PRTREE_KEY3@T@_INDEX_MASK (sieves_number);

  for (i=sieves_number; i >= 0; i--)
    {
      @key_type@ masked = key & _bitmasks[i].mask;
      if (!masked) ret -= _bitmasks[i].base;
      else key = masked;
/*       g_printerr ("%d, %#@kmod@x, %#@kmod@x, %#@kmod@x, %u\n", i, _bitmasks[i].mask, key, masked, ret); */
/*       g_printerr ("%d, %u %u\n", i, _bitmasks[i].base, maxdepth); */
    }

  return ret + 1;
}

/**
 * vsg_prtree_key3@t@_write:
 * @key : the VsgPRTreeKey3@t@ to write.
 * @file : the file to write to.
 *
 * Displays @key into @file.
 */
void vsg_prtree_key3@t@_write (VsgPRTreeKey3@t@ *key, FILE *file)
{
  if (key->depth == 0)
    {
      g_fprintf (file, "x=0x0, y=0x0, z=0x0, d=0");
    }
  else
    {
      @key_type@ x = key->x/*  & VSG_PRTREE_KEY3@T@_INDEX_MASK (key->depth-1) */;
      @key_type@ y = key->y/*  & VSG_PRTREE_KEY3@T@_INDEX_MASK (key->depth-1) */;
      @key_type@ z = key->z/*  & VSG_PRTREE_KEY3@T@_INDEX_MASK (key->depth-1) */;

      g_fprintf (file, "x=%#@kmod@x, y=%#@kmod@x, z=%#@kmod@x, d=%d",
                 x, y, z, key->depth);
    }
}

/**
 * vsg_prtree_key3@t@_xor:
 * @one : a VsgPRTreeKey3@t@.
 * @other : a VsgPRTreeKey3@t@.
 * @result : resulting VsgPRTreeKey3@t@.
 *
 * Computes bitwise (@one XOR @other) and stores the result into @result.
 * Argument aliasing is allowed.
 */
void vsg_prtree_key3@t@_xor (VsgPRTreeKey3@t@ *one,
                             VsgPRTreeKey3@t@ *other,
                             VsgPRTreeKey3@t@ *result)
{

  guint8 d;

  if (one->depth >= other->depth)
    {
      d = one->depth - other->depth;
      result->x = one->x ^ (other->x << d);
      result->y = one->y ^ (other->y << d);
      result->z = one->z ^ (other->z << d);
      result->depth = one->depth;
    }
  else
    {
      d = other->depth - one->depth;
      result->x = (one->x << d) ^ other->x;
      result->y = (one->y << d) ^ other->y;
      result->z = (one->z << d) ^ other->z;
      result->depth = other->depth;
    }
}

/**
 * vsg_prtree_key3@t@_first_different_index:
 * @one : a VsgPRTreeKey3@t@.
 * @other : a VsgPRTreeKey3@t@.
 *
 * Computes the index of the more significant different bit between @one and
 * @other). 
 *
 * Returns: 1 + index of the heavier different bit. 0 means identical keys
 */
guint8 vsg_prtree_key3@t@_first_different_index (VsgPRTreeKey3@t@ *one,
                                                 VsgPRTreeKey3@t@ *other)
{
  VsgPRTreeKey3@t@ xor;
  guint8 x, y, z;

  vsg_prtree_key3@t@_xor (one, other, &xor);

  x = _single_key_first_true_bit (xor.x, xor.depth);
  y = _single_key_first_true_bit (xor.y, xor.depth);
  z = _single_key_first_true_bit (xor.z, xor.depth);

  return MAX (MAX (x, y), z);

}

/**
 * vsg_prtree_key3@t@_deepest_common_ancestor:
 * @one : a VsgPRTreeKey3@t@.
 * @other : a VsgPRTreeKey3@t@.
 * @result : resulting VsgPRTreeKey3@t@.
 *
 * Computes the longest VsgPRTreeKey3@t@ that can be considered as an
 * ancestor of both @one and @other.
 */
void vsg_prtree_key3@t@_deepest_common_ancestor (VsgPRTreeKey3@t@ *one,
                                                 VsgPRTreeKey3@t@ *other,
                                                 VsgPRTreeKey3@t@ *result)
{
  VsgPRTreeKey3@t@ *max = one;
  guint8 index;

  if (one->depth < other->depth) max = other;

  index = vsg_prtree_key3@t@_first_different_index (one, other);

  result->x = max->x >> index;
  result->y = max->y >> index;
  result->z = max->z >> index;

  result->depth = max->depth - index;
}

/**
 * vsg_prtree_key3@t@_build_child:
 * @father : a VsgPRTreeKey3@t@.
 * @child_num : a vsgloc3.
 * @result : resulting VsgPRTreeKey3@t@.
 *
 * Builds a new key from @father and the child number @child_num.
 */
void vsg_prtree_key3@t@_build_child (VsgPRTreeKey3@t@ *father,
                                     vsgloc3 child_num,
                                     VsgPRTreeKey3@t@ *result)
{
  if (father == NULL)
    {
      result->x = 0;
      result->y = 0;
      result->z = 0;
      result->depth = 0;
    }
  else
    {
      result->x = (father->x << 1) | (child_num & VSG_LOC3_X);
      result->y = (father->y << 1) | ((child_num & VSG_LOC3_Y)>>1);
      result->z = (father->z << 1) | ((child_num & VSG_LOC3_Z)>>2);

      result->depth = father->depth+1;
    }
}

/**
 * vsg_prtree_key3@t@_build_father:
 * @child : a VsgPRTreeKey3@t@.
 * @child_num : a vsgloc3.
 * @result : resulting VsgPRTreeKey3@t@.
 *
 * Builds a new key from @child and the child number @child_num. @result is
 * returned such that @child is its direct child of number @child_num.
 */
void vsg_prtree_key3@t@_build_father (VsgPRTreeKey3@t@ *child,
                                      vsgloc3 child_num,
                                      VsgPRTreeKey3@t@ *result)
{
  if (child == NULL)
    {
      result->depth = 1;
      result->x = (child_num & VSG_LOC3_X);
      result->y = (child_num & VSG_LOC3_Y)>>1;
      result->z = (child_num & VSG_LOC3_Z)>>2;

      return;
    }

  result->x = child->x | ((child_num & VSG_LOC3_X)<<child->depth);
  result->y = child->y | (((child_num & VSG_LOC3_Y)>>1)<<child->depth);
  result->z = child->z | (((child_num & VSG_LOC3_Z)>>2)<<child->depth);
  
  result->depth = child->depth+1;
}

/**
 * vsg_prtree_key3@t@_loc3:
 * @key : a VsgPRTreeKey3@t@.
 * @center : a VsgPRTreeKey3@t@.
 *
 * Computes the localization index of @key from the point of view of
 * @center.
 *
 * Returns: the vsgloc3 of the position og @key.
 */
vsgloc3 vsg_prtree_key3@t@_loc3 (VsgPRTreeKey3@t@ *key,
                                 VsgPRTreeKey3@t@ *center)
{
  VsgPRTreeKey3@t@ tmp;
  vsgloc3 loc = 0;

  if (key->depth > center->depth)
    {
      _key_scale_up (center, key->depth - center->depth, &tmp);
      center = &tmp;
    }
  else if (key->depth < center->depth)
    {
      _key_scale_up (key, center->depth - key->depth, &tmp);
      key = &tmp;
    }
  if (key->x > center->x) loc |= VSG_LOC3_X;
  if (key->y > center->y) loc |= VSG_LOC3_Y;
  if (key->z > center->z) loc |= VSG_LOC3_Z;

  return loc;
}

static inline @key_type@ _key3@t@_distance (VsgPRTreeKey3@t@ *one,
                                            VsgPRTreeKey3@t@ *other)
{
  @key_type@ dx, dy, dz;

  if (one->x < other->x) dx = other->x - one->x;
  else dx = one->x - other->x;

  if (one->y < other->y) dy = other->y - one->y;
  else dy = one->y - other->y;

  if (one->z < other->z) dz = other->z - one->z;
  else dz = one->z - other->z;

  return MAX (MAX (dx, dy), dz);
}

/**
 * vsg_prtree_key3@t@_distance:
 * @one : a VsgPRTreeKey3@t@.
 * @other : a VsgPRTreeKey3@t@.
 *
 * Computes the maximum distance (on x, y and z coordinates) between two
 * keys @one and other. The result is set in the scale of the shallowest key.
 * For example if @one is shallower than @other and
 * vsg_prtree_key3@t@_distance() returns "d", the two nodes are separated by "d"
 * nodes of the level of @one in one direction in a tree.
 *
 * Returns: the distance between @one and @other.
 */
@key_type@ vsg_prtree_key3@t@_distance (VsgPRTreeKey3@t@ *one,
                                        VsgPRTreeKey3@t@ *other)
{
  VsgPRTreeKey3@t@ tmp;

  if (one->depth < other->depth)
    {
      _key_scale_down (other, other->depth - one->depth, &tmp);
      other = &tmp;
    }
  else if (one->depth > other->depth)
    {
      _key_scale_down (one, one->depth - other->depth, &tmp);
      one = &tmp;
    }

  return _key3@t@_distance (one, other);
}

/**
 * vsg_prtree_key3@t@_child:
 * @key : a VsgPRTreeKey3@t@.
 *
 * Computes the child index (#vsgloc3) of the first level of @key.
 *
 * Returns: the first child index of @key or 0 if @key->depth == 0.
 */
vsgloc3 vsg_prtree_key3@t@_child (VsgPRTreeKey3@t@ *key)
{
  gint8 locx, locy, locz;
  vsgloc3 loc;

  g_return_val_if_fail (key->depth > 0, 0);

  locx = (key->x >> (key->depth-1)) & 1;
  locy = (key->y >> (key->depth-1)) & 1;
  locz = (key->z >> (key->depth-1)) & 1;
  loc = locx | (locy << 1) | (locz << 2);

  return loc;
}

/**
 * vsg_prtree_key3@t@_ancestor_order_unsafe:
 * @key : a VsgPRTreeKey3@t@.
 * @center : a VsgPRTreeKey3@t@.
 *
 * Compares @key and @center for determining their order in a default
 * children order traversal.
 *
 * Returns: wether @key would precede @center in a normal traversal.
 */
gboolean vsg_prtree_key3@t@_ancestor_order_unsafe (VsgPRTreeKey3@t@ *key,
                                                   VsgPRTreeKey3@t@ *center)
{
  guint8 index;
  @key_type@ indexmask;
  @key_type@ kloc, cloc;

  index = vsg_prtree_key3@t@_first_different_index (key, center);

  indexmask = 1 << (index-1);

  /* use @key_type@ like we should do with vsgloc3 */
  kloc = (key->x & indexmask) | ((key->y & indexmask) << 1) |
    ((key->z & indexmask) << 2);
  cloc = (center->x & indexmask) | ((center->y & indexmask) << 1) |
    ((center->z & indexmask) << 2);

  return kloc <= cloc;
}

/**
 * vsg_prtree_key3@t@_equals:
 * @one: a #VsgPRTreeKey3@t@.
 * @other: a #VsgPRTreeKey3@t@.
 *
 * compares @one with @other.
 *
 * Returns: #TRUE when @one and @other are identical
 */
gboolean vsg_prtree_key3@t@_equals (const VsgPRTreeKey3@t@ *one,
                                    const VsgPRTreeKey3@t@ *other)
{
/*   return memcmp (one, other, sizeof (VsgPRTreeKey3@t@)) == 0; */
  return one->depth == other->depth && one->x == other->x &&
    one->y == other->y && one->z == other->z;
}

/**
 * vsg_prtree_key3@t@_copy:
 * @dst: a #VsgPRTreeKey3@t@.
 * @src: a #VsgPRTreeKey3@t@.
 *
 * Copies @src to @dst.
 */
void vsg_prtree_key3@t@_copy (VsgPRTreeKey3@t@ *dst, VsgPRTreeKey3@t@ *src)
{
  _key_copy (dst, src);
}

/**
 * vsg_prtree_key3@t@_get_father:
 * @key: a #VsgPRTreeKey3@t@.
 * @father: a #VsgPRTreeKey3@t@.
 *
 * sets @father to the value of @key's direct ancestor (ie: the same
 * as @key shortened by 1 level).
 */
void vsg_prtree_key3@t@_get_father (VsgPRTreeKey3@t@ *key,
                                    VsgPRTreeKey3@t@ *father)
{
  _key_scale_down (key, 1, father);
}

/**
 * vsg_prtree_key3@t@_is_neighbour:
 * @one: a #VsgPRTreeKey3@t@.
 * @other: a #VsgPRTreeKey3@t@.
 *
 * evaluates proximity of @one and @other.
 * WARNING: if the keys are not of the same depth, behaviour is undefined.
 *
 * Returns: #TRUE if @one and @other are neighbours
 */
gboolean vsg_prtree_key3@t@_is_neighbour (VsgPRTreeKey3@t@ *one,
                                          VsgPRTreeKey3@t@ *other)
{
  return ((one->x - other->x) <= 1 || (other->x - one->x) <= 1) &&
    ((one->y - other->y) <= 1 || (other->y - one->y) <= 1) &&
    ((one->z - other->z) <= 1 || (other->z - one->z) <= 1);
}

/**
 * vsg_prtree_key3@t@_is_ancestor:
 * @ancestor: a #VsgPRTreeKey3@t@.
 * @child: a #VsgPRTreeKey3@t@.
 *
 * ancetry query on two keys.
 *
 * Returns: if @child is a descendant of @ancestor.
 */
gboolean vsg_prtree_key3@t@_is_ancestor (VsgPRTreeKey3@t@ *ancestor,
                                         VsgPRTreeKey3@t@ *child)
{
  VsgPRTreeKey3@t@ tmp;
  VsgPRTreeKey3@t@ *_child;

  if (child->depth < ancestor->depth) return FALSE;

  if (child->depth > ancestor->depth)
    {
      _child = &tmp;
      _key_scale_down (child, child->depth - ancestor->depth, _child);
    }
  else
    _child = child;

  return ancestor->x == _child->x && ancestor->y == _child->y &&
    ancestor->z == _child->z;
}

static inline @key_type@ _key_clamped_coord_dist (@key_type@ one, @key_type@ other,
                                                  @key_type@ clamped, guint8 height)
{
  @key_type@ tmp = _key_coord_scale_down (one, height);

  if (other == tmp)
    {
      /* g_printerr ("tmp=other\n"); */
      return 0;
    }

  if (other > tmp)
    {
      /* g_printerr ("tmp<other h=%d tmp=%d cl=%d\n", height, tmp, clamped); */
      return ((other-tmp) << height) - clamped;
    }

  /* g_printerr ("tmp>other: h=%d tmp=%d cl=%d\n", height, tmp, clamped); */
  return ((tmp-other-1) << height) + 1 + clamped;
}

/**
 * vsg_prtree_key3@t@_deepest_distance:
 * @one : a VsgPRTreeKey3@t@.
 * @other : a VsgPRTreeKey3@t@.
 *
 * Computes the maximum distance (on any of x, y and z coordinates) between two
 * keys @one and other. As opposed to vsg_prtree_key3@t@_distance(), the result is set in the scale of the deepest key.
 *
 * Returns: the distance between @one and @other.
 */
@key_type@ vsg_prtree_key3@t@_deepest_distance (VsgPRTreeKey3@t@ *one,
                                                VsgPRTreeKey3@t@ *other)
{
  guint8 clamp_height;
  @key_type@ dx, dy, dz;
  VsgPRTreeKey3@t@ clamped;

  /* make sure @one holds the deepest of both keys */
  if (one->depth < other->depth)
    {
      VsgPRTreeKey3@t@ *tmp;

      tmp = one;
      one = other;
      other = tmp;
    }

  clamp_height = one->depth - other->depth;
  vsg_prtree_key3@t@_sever (one, clamp_height, &clamped);

  dx = _key_clamped_coord_dist (one->x, other->x, clamped.x, clamp_height);
  dy = _key_clamped_coord_dist (one->y, other->y, clamped.y, clamp_height);
  dz = _key_clamped_coord_dist (one->z, other->z, clamped.z, clamp_height);

  /* g_printerr ("%d %ld-%ld / %d %ld\n", clamp_height, one->x, other->x, clamped.x, dx); */
  /* g_printerr ("%d %ld-%ld / %d %ld\n", clamp_height, one->y, other->y, clamped.y, dy); */
  /* g_printerr ("%d %ld-%ld / %d %ld\n", clamp_height, one->z, other->z, clamped.z, dz); */

  return MAX (MAX (dx, dy), dz);
}
