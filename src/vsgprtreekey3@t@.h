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

#ifndef __VSG_PRTREE_KEY3@T@_H__ /* __VSG_PRTREE_KEY3@T@_H__ */
#define __VSG_PRTREE_KEY3@T@_H__

#include <stdio.h>
#include <glib.h>

#include <vsg/vsgprtree3-common.h>
#include <vsg/vsgmpi.h>

G_BEGIN_DECLS;

/* macros */
#ifdef VSG_HAVE_MPI
#define VSG_MPI_TYPE_PRTREE_KEY3@T@ vsg_prtree_key3@t@_get_mpi_type ()
#endif
#define VSG_PRTREE_KEY3@T@_SIZE (sizeof (@key_type@))
#define VSG_PRTREE_KEY3@T@_BITS (8*VSG_PRTREE_KEY3@T@_SIZE)

#define VSG_PRTREE_KEY3@T@_INDEX_MASK(index) ( \
(1<<((index)+1)) - 1 \
)


/* typedefs */
typedef struct _VsgPRTreeKey3@t@ VsgPRTreeKey3@t@;

/* structures */
struct _VsgPRTreeKey3@t@ {
  @key_type@ x;
  @key_type@ y;
  @key_type@ z;
  guint8 depth;
};

/* constants */
static const VsgPRTreeKey3@t@ vsg_prtree_key3@t@_root = {0, 0, 0, 0};

/* functions */
#ifdef VSG_HAVE_MPI
MPI_Datatype vsg_prtree_key3@t@_get_mpi_type (void) G_GNUC_CONST;
#endif

void vsg_prtree_key3@t@_write (VsgPRTreeKey3@t@ *key, FILE *file);

guint8 vsg_prtree_key3@t@_first_different_index (VsgPRTreeKey3@t@ *one,
                                                 VsgPRTreeKey3@t@ *other);

void vsg_prtree_key3@t@_deepest_common_ancestor (VsgPRTreeKey3@t@ *one,
                                                 VsgPRTreeKey3@t@ *other,
                                                 VsgPRTreeKey3@t@ *result);

void vsg_prtree_key3@t@_build_child (VsgPRTreeKey3@t@ *father,
                                     vsgloc3 child_num,
                                     VsgPRTreeKey3@t@ *result);

void vsg_prtree_key3@t@_build_father (VsgPRTreeKey3@t@ *child,
                                      vsgloc3 child_num,
                                      VsgPRTreeKey3@t@ *result);

vsgloc3 vsg_prtree_key3@t@_loc3 (VsgPRTreeKey3@t@ *key,
                                 VsgPRTreeKey3@t@ *center);

@key_type@ vsg_prtree_key3@t@_distance (VsgPRTreeKey3@t@ *one,
                                        VsgPRTreeKey3@t@ *other);

vsgloc3 vsg_prtree_key3@t@_child (VsgPRTreeKey3@t@ *key);

gboolean vsg_prtree_key3@t@_equals (const VsgPRTreeKey3@t@ *one,
                                    const VsgPRTreeKey3@t@ *other);

void vsg_prtree_key3@t@_copy (VsgPRTreeKey3@t@ *dst, VsgPRTreeKey3@t@ *src);

void vsg_prtree_key3@t@_get_father (VsgPRTreeKey3@t@ *key,
                                    VsgPRTreeKey3@t@ *father);

gboolean vsg_prtree_key3@t@_is_neighbour (VsgPRTreeKey3@t@ *one,
                                          VsgPRTreeKey3@t@ *other);

gboolean vsg_prtree_key3@t@_is_ancestor (VsgPRTreeKey3@t@ *ancestor,
                                         VsgPRTreeKey3@t@ *child);

static inline void vsg_prtree_key3@t@_truncate (VsgPRTreeKey3@t@ *key,
                                                guint8 offset,
                                                VsgPRTreeKey3@t@ *result)
{
  result->x = key->x >> offset;
  result->y = key->y >> offset;
  result->z = key->z >> offset;
  result->depth = (guint8) MAX (0, ((gint8) key->depth) - offset);
}

static inline void vsg_prtree_key3@t@_sever (VsgPRTreeKey3@t@ *key, guint8 size,
                                             VsgPRTreeKey3@t@ *result)
{
  @key_type@ mask = VSG_PRTREE_KEY3@T@_INDEX_MASK (size-1);

  result->x &= mask;
  result->y &= mask;
  result->z &= mask;
  result->depth = size;
}

G_END_DECLS;

#endif /* __VSG_PRTREE_KEY3@T@_H__ */
