#include <vsg-config.h>

#include "vsgprtreekey2@t@.h"

#include <string.h>

#include <glib/gprintf.h>

/**
 * SECTION: vsgprtreekey2@t@
 * @short_description: 2D localization key for #VsgPRTree2@t@.
 */

/**
 * VsgPRTreeKey2@t@:
 * @x : A @key_type@ for storing x binary coordinate
 * @y : A @key_type@ for storing y binary coordinate
 * @depth : the depth of the key. It's the number of bits to take in account
 * for @x and @y.
 *
 * A structure for manupilating VsgPRTree2@t@ nodes coordinates.
 */

/**
 * VSG_PRTREE_KEY2@T@_BITS:
 *
 * number of bits in a #VsgPRTreeKey2@t@.
 */

/**
 * VSG_PRTREE_KEY2@T@_INDEX_MASK:
 * @index: an integer
 *
 * an integer mask with @index first bits set to 1.
 */

/**
 * VSG_PRTREE_KEY2@T@_SIZE:
 *
 * size of a #VsgPRTreeKey2@t@.
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
static guint8 _number_of_sieves[VSG_PRTREE_KEY2@T@_BITS+1];

static void _exitfunc ()
{
  if (_bitmasks != NULL)
    g_free (_bitmasks);
}

void vsgprtree_key2@t@_init ()
{
  if (G_UNLIKELY (_bitmasks_number == 0))
    {
      guint8 ks = VSG_PRTREE_KEY2@T@_BITS;
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
 * VSG_MPI_TYPE_PRTREE_KEY2@T@:
 *
 * The #MPI_Datatype associated to #VsgPrtreeKey2@t@.
 */

MPI_Datatype vsg_prtree_key2@t@_get_mpi_type (void)
{
  static MPI_Datatype prtree_key2@t@_mpi_type = MPI_DATATYPE_NULL;

  if (prtree_key2@t@_mpi_type == MPI_DATATYPE_NULL)
    {
      gint blocks = 3;
      gint lens[] = {1, 1, 1};
      MPI_Aint indices[] = {
        G_STRUCT_OFFSET (VsgPRTreeKey2@t@, x),
        G_STRUCT_OFFSET (VsgPRTreeKey2@t@, y),
        G_STRUCT_OFFSET (VsgPRTreeKey2@t@, depth),
      };
      MPI_Datatype types[] = {
        @KEY_MPI_DATATYPE@,
        @KEY_MPI_DATATYPE@,
        MPI_BYTE,
      };

      MPI_Type_struct (blocks, lens, indices, types, &prtree_key2@t@_mpi_type);
      MPI_Type_commit (&prtree_key2@t@_mpi_type);
    }

  return prtree_key2@t@_mpi_type;
}
#endif

static void _key_copy (VsgPRTreeKey2@t@ *dst, VsgPRTreeKey2@t@ *src)
{
  memcpy (dst, src, sizeof (VsgPRTreeKey2@t@));
}

static void _key_scale_up (VsgPRTreeKey2@t@ *key, guint8 offset,
                           VsgPRTreeKey2@t@ *result)
{
  result->x = key->x << offset;
  result->y = key->y << offset;
  result->depth = key->depth + offset;
}

static void _key_scale_down (VsgPRTreeKey2@t@ *key, guint8 offset,
                             VsgPRTreeKey2@t@ *result)
{
  result->x = key->x >> offset;
  result->y = key->y >> offset;
  result->depth = (guint8) MAX (0, ((gint8) key->depth) - offset);
}

static guint8 _single_key_first_true_bit (@key_type@ key, guint8 maxdepth)
{
  gint8 sieves_number;
  gint8 i, ret;

  if (!key) return 0;

  sieves_number = _number_of_sieves[maxdepth]-1;
  ret = VSG_PRTREE_KEY2@T@_INDEX_MASK (sieves_number);

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
 * vsg_prtree_key2@t@_write:
 * @key : the VsgPRTreeKey2@t@ to write.
 * @file : the file to write to.
 *
 * Displays @key into @file.
 */
void vsg_prtree_key2@t@_write (VsgPRTreeKey2@t@ *key, FILE *file)
{
  if (key->depth == 0)
    {
      g_fprintf (file, "x=0x0, y=0x0, d=0");
    }
  else
    {
      @key_type@ x = key->x/*  & VSG_PRTREE_KEY2@T@_INDEX_MASK (key->depth-1) */;
      @key_type@ y = key->y/*  & VSG_PRTREE_KEY2@T@_INDEX_MASK (key->depth-1) */;

      g_fprintf (file, "x=%#@kmod@x, y=%#@kmod@x, d=%d",
                 x, y, key->depth);
    }
}

/**
 * vsg_prtree_key2@t@_xor:
 * @one : a VsgPRTreeKey2@t@.
 * @other : a VsgPRTreeKey2@t@.
 * @result : resulting VsgPRTreeKey2@t@.
 *
 * Computes bitwise (@one XOR @other) and stores the result into @result.
 * Argument aliasing is allowed.
 */
void vsg_prtree_key2@t@_xor (VsgPRTreeKey2@t@ *one,
                             VsgPRTreeKey2@t@ *other,
                             VsgPRTreeKey2@t@ *result)
{

  guint8 d;

  if (one->depth >= other->depth)
    {
      d = one->depth - other->depth;
      result->x = one->x ^ (other->x << d);
      result->y = one->y ^ (other->y << d);
      result->depth = one->depth;
    }
  else
    {
      d = other->depth - one->depth;
      result->x = (one->x << d) ^ other->x;
      result->y = (one->y << d) ^ other->y;
      result->depth = other->depth;
    }
}

/**
 * vsg_prtree_key2@t@_first_different_index:
 * @one : a VsgPRTreeKey2@t@.
 * @other : a VsgPRTreeKey2@t@.
 *
 * Computes the index of the more significant different bit between @one and
 * @other). 
 *
 * Returns: 1 + index of the heavier different bit. 0 means identical keys
 */
guint8 vsg_prtree_key2@t@_first_different_index (VsgPRTreeKey2@t@ *one,
                                                 VsgPRTreeKey2@t@ *other)
{
  VsgPRTreeKey2@t@ xor;
  guint8 x, y;

  vsg_prtree_key2@t@_xor (one, other, &xor);

  x = _single_key_first_true_bit (xor.x, xor.depth);
  y = _single_key_first_true_bit (xor.y, xor.depth);

  return MAX (x, y);

}

/**
 * vsg_prtree_key2@t@_deepest_common_ancestor:
 * @one : a VsgPRTreeKey2@t@.
 * @other : a VsgPRTreeKey2@t@.
 * @result : resulting VsgPRTreeKey2@t@.
 *
 * Computes the longest VsgPRTreeKey2@t@ that can be considered as an
 * ancestor of both @one and @other.
 */
void vsg_prtree_key2@t@_deepest_common_ancestor (VsgPRTreeKey2@t@ *one,
                                                 VsgPRTreeKey2@t@ *other,
                                                 VsgPRTreeKey2@t@ *result)
{
  VsgPRTreeKey2@t@ *max = one;
  guint8 index;

  if (one->depth < other->depth) max = other;

  index = vsg_prtree_key2@t@_first_different_index (one, other);

  result->x = max->x >> index;
  result->y = max->y >> index;

  result->depth = max->depth - index;
}

/**
 * vsg_prtree_key2@t@_build_child:
 * @father : a VsgPRTreeKey2@t@.
 * @child_num : a vsgloc2.
 * @result : resulting VsgPRTreeKey2@t@.
 *
 * Builds a new key from @father and the child number @child_num.
 */
void vsg_prtree_key2@t@_build_child (VsgPRTreeKey2@t@ *father,
                                     vsgloc2 child_num,
                                     VsgPRTreeKey2@t@ *result)
{
  if (father == NULL)
    {
      result->x = 0;
      result->y = 0;
      result->depth = 0;
    }
  else
    {
      result->x = (father->x << 1) | (child_num & VSG_LOC2_X);
      result->y = (father->y << 1) | ((child_num & VSG_LOC2_Y)>>1);

      result->depth = father->depth+1;
    }
}

/**
 * vsg_prtree_key2@t@_build_father:
 * @child : a VsgPRTreeKey2@t@.
 * @child_num : a vsgloc2.
 * @result : resulting VsgPRTreeKey2@t@.
 *
 * Builds a new key from @child and the child number @child_num. @result is
 * returned such that @child is its direct child of number @child_num.
 */
void vsg_prtree_key2@t@_build_father (VsgPRTreeKey2@t@ *child,
                                      vsgloc2 child_num,
                                      VsgPRTreeKey2@t@ *result)
{
  if (child == NULL)
    {
      result->depth = 1;
      result->x = (child_num & VSG_LOC2_X);
      result->y = (child_num & VSG_LOC2_Y)>>1;

      return;
    }

  result->x = child->x | ((child_num & VSG_LOC2_X)<<child->depth);
  result->y = child->y | (((child_num & VSG_LOC2_Y)>>1)<<child->depth);
  
  result->depth = child->depth+1;
}

/**
 * vsg_prtree_key2@t@_loc2:
 * @key : a VsgPRTreeKey2@t@.
 * @center : a VsgPRTreeKey2@t@.
 *
 * Computes the localization index of @key from the point of view of
 * @center.
 *
 * Returns: the vsgloc2 of the position og @key.
 */
vsgloc2 vsg_prtree_key2@t@_loc2 (VsgPRTreeKey2@t@ *key,
                                 VsgPRTreeKey2@t@ *center)
{
  VsgPRTreeKey2@t@ tmp;
  vsgloc2 loc = 0;

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
  if (key->x > center->x) loc |= VSG_LOC2_X;
  if (key->y > center->y) loc |= VSG_LOC2_Y;

  return loc;
}

static inline @key_type@ _key2@t@_distance (VsgPRTreeKey2@t@ *one,
                                            VsgPRTreeKey2@t@ *other)
{
  @key_type@ dx, dy;

  if (one->x < other->x) dx = other->x - one->x;
  else dx = one->x - other->x;

  if (one->y < other->y) dy = other->y - one->y;
  else dy = one->y - other->y;

  return MAX (dx, dy);
}

/**
 * vsg_prtree_key2@t@_distance:
 * @one : a VsgPRTreeKey2@t@.
 * @other : a VsgPRTreeKey2@t@.
 *
 * Computes the maximum distance (on both x and y coordinates) between two
 * keys @one and other. The result is set in the scale of the shallowest key.
 * For example if @one is shallower than @other and
 * vsg_prtree_key2@t@_distance() returns "d", the two nodes are separated by "d"
 * nodes of the level of @one in one direction in a tree.
 *
 * Returns: the distance between @one and @other.
 */
@key_type@ vsg_prtree_key2@t@_distance (VsgPRTreeKey2@t@ *one,
                                        VsgPRTreeKey2@t@ *other)
{
  VsgPRTreeKey2@t@ tmp;

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

  return _key2@t@_distance (one, other);
}

/**
 * vsg_prtree_key2@t@_child:
 * @key : a VsgPRTreeKey2@t@.
 *
 * Computes the child index (#vsgloc2) of the first level of @key.
 *
 * Returns: the first child index of @key or 0 if @key->depth == 0.
 */
vsgloc2 vsg_prtree_key2@t@_child (VsgPRTreeKey2@t@ *key)
{
  gint8 locx, locy;
  vsgloc2 loc;

  g_return_val_if_fail (key->depth > 0, 0);

  locx = (key->x >> (key->depth-1)) & 1;
  locy = (key->y >> (key->depth-1)) & 1;
  loc = locx | (locy << 1);

  return loc;
}

/**
 * vsg_prtree_key2@t@_ancestor_order_unsafe:
 * @key : a VsgPRTreeKey2@t@.
 * @center : a VsgPRTreeKey2@t@.
 *
 * Compares @key and @center for determining their order in a default
 * children order traversal.
 *
 * Returns: wether @key would precede @center in a normal traversal.
 */
gboolean vsg_prtree_key2@t@_ancestor_order_unsafe (VsgPRTreeKey2@t@ *key,
                                                   VsgPRTreeKey2@t@ *center)
{
  guint8 index;
  @key_type@ indexmask;
  @key_type@ kloc, cloc;

  index = vsg_prtree_key2@t@_first_different_index (key, center);

  indexmask = 1 << (index-1);

  /* use @key_type@ like we should do with vsgloc2 */
  kloc = (key->x & indexmask) | ((key->y & indexmask) << 1);
  cloc = (center->x & indexmask) | ((center->y & indexmask) << 1);

  return kloc <= cloc;
}

/**
 * vsg_prtree_key2@t@_equals:
 * @one: a #VsgPRTreeKey2@t@.
 * @other: a #VsgPRTreeKey2@t@.
 *
 * compares @one with @other.
 *
 * Returns: #TRUE when @one and @other are identical
 */
gboolean vsg_prtree_key2@t@_equals (const VsgPRTreeKey2@t@ *one,
                                    const VsgPRTreeKey2@t@ *other)
{
/*   return memcmp (one, other, sizeof (VsgPRTreeKey2@t@)) == 0; */
  return one->depth == other->depth && one->x == other->x && one->y == other->y;
}

/**
 * vsg_prtree_key2@t@_copy:
 * @dst: a #VsgPRTreeKey2@t@.
 * @src: a #VsgPRTreeKey2@t@.
 *
 * Copies @src to @dst.
 */
void vsg_prtree_key2@t@_copy (VsgPRTreeKey2@t@ *dst, VsgPRTreeKey2@t@ *src)
{
  _key_copy (dst, src);
}

/**
 * vsg_prtree_key2@t@_get_father:
 * @key: a #VsgPRTreeKey2@t@.
 * @father: a #VsgPRTreeKey2@t@.
 *
 * sets @father to the value of @key's direct ancestor (ie: the same
 * as @key shortened by 1 level).
 */
void vsg_prtree_key2@t@_get_father (VsgPRTreeKey2@t@ *key,
                                    VsgPRTreeKey2@t@ *father)
{
  _key_scale_down (key, 1, father);
}

/**
 * vsg_prtree_key2@t@_is_neighbour:
 * @one: a #VsgPRTreeKey2@t@.
 * @other: a #VsgPRTreeKey2@t@.
 *
 * evaluates proximity of @one and @other.
 * WARNING: if the keys are not of the same depth, behaviour is undefined.
 *
 * Returns: #TRUE if @one and @other are neighbours
 */
gboolean vsg_prtree_key2@t@_is_neighbour (VsgPRTreeKey2@t@ *one,
                                          VsgPRTreeKey2@t@ *other)
{
  return ((one->x - other->x) <= 1 || (other->x - one->x) <= 1) &&
    ((one->y - other->y) <= 1 || (other->y - one->y) <= 1);
}

/**
 * vsg_prtree_key2@t@_is_ancestor:
 * @ancestor: a #VsgPRTreeKey2@t@.
 * @child: a #VsgPRTreeKey2@t@.
 *
 * ancetry query on two keys.
 *
 * Returns: if @child is a descendant of @ancestor.
 */
gboolean vsg_prtree_key2@t@_is_ancestor (VsgPRTreeKey2@t@ *ancestor,
                                         VsgPRTreeKey2@t@ *child)
{
  VsgPRTreeKey2@t@ tmp;
  VsgPRTreeKey2@t@ *_child;

  if (child->depth < ancestor->depth) return FALSE;

  if (child->depth > ancestor->depth)
    {
      _child = &tmp;
      _key_scale_down (child, child->depth - ancestor->depth, _child);
    }
  else
    _child = child;

  return ancestor->x == _child->x && ancestor->y == _child->y;
}

