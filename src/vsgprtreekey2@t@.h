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

#ifndef __VSG_PRTREE_KEY2@T@_H__ /* __VSG_PRTREE_KEY2@T@_H__ */
#define __VSG_PRTREE_KEY2@T@_H__

#include <stdio.h>
#include <glib.h>

#include <vsg/vsgprtree2-common.h>
#include <vsg/vsgmpi.h>

G_BEGIN_DECLS;

/* macros */
#ifdef VSG_HAVE_MPI
#define VSG_MPI_TYPE_PRTREE_KEY2@T@ vsg_prtree_key2@t@_get_mpi_type ()
#endif
#define VSG_PRTREE_KEY2@T@_SIZE (sizeof (@key_type@))
#define VSG_PRTREE_KEY2@T@_BITS (8*VSG_PRTREE_KEY2@T@_SIZE)

#define VSG_PRTREE_KEY2@T@_INDEX_MASK(index) ( \
(1<<((index)+1)) - 1 \
)


/* typedefs */
typedef struct _VsgPRTreeKey2@t@ VsgPRTreeKey2@t@;

/* structures */
struct _VsgPRTreeKey2@t@ {
  @key_type@ x;
  @key_type@ y;
  guint8 depth;
};

/* constants */
static const VsgPRTreeKey2@t@ vsg_prtree_key2@t@_root = {0, 0, 0};

/* functions */
#ifdef VSG_HAVE_MPI
MPI_Datatype vsg_prtree_key2@t@_get_mpi_type (void) G_GNUC_CONST;
#endif

void vsg_prtree_key2@t@_write (VsgPRTreeKey2@t@ *key, FILE *file);

guint8 vsg_prtree_key2@t@_first_different_index (VsgPRTreeKey2@t@ *one,
                                                 VsgPRTreeKey2@t@ *other);

void vsg_prtree_key2@t@_deepest_common_ancestor (VsgPRTreeKey2@t@ *one,
                                                 VsgPRTreeKey2@t@ *other,
                                                 VsgPRTreeKey2@t@ *result);

void vsg_prtree_key2@t@_build_child (VsgPRTreeKey2@t@ *father,
                                     vsgloc2 child_num,
                                     VsgPRTreeKey2@t@ *result);

void vsg_prtree_key2@t@_build_father (VsgPRTreeKey2@t@ *child,
                                      vsgloc2 child_num,
                                      VsgPRTreeKey2@t@ *result);

vsgloc2 vsg_prtree_key2@t@_loc2 (VsgPRTreeKey2@t@ *key,
                                 VsgPRTreeKey2@t@ *center);

@key_type@ vsg_prtree_key2@t@_distance (VsgPRTreeKey2@t@ *one,
                                        VsgPRTreeKey2@t@ *other);

vsgloc2 vsg_prtree_key2@t@_child (VsgPRTreeKey2@t@ *key);

gboolean vsg_prtree_key2@t@_equals (VsgPRTreeKey2@t@ *one,
                                    VsgPRTreeKey2@t@ *other);

void vsg_prtree_key2@t@_copy (VsgPRTreeKey2@t@ *dst, VsgPRTreeKey2@t@ *src);

void vsg_prtree_key2@t@_get_father (VsgPRTreeKey2@t@ *key,
                                    VsgPRTreeKey2@t@ *father);

gboolean vsg_prtree_key2@t@_is_neighbour (VsgPRTreeKey2@t@ *one,
                                          VsgPRTreeKey2@t@ *other);

gboolean vsg_prtree_key2@t@_is_ancestor (VsgPRTreeKey2@t@ *ancestor,
                                         VsgPRTreeKey2@t@ *child);

static inline void vsg_prtree_key2@t@_truncate (VsgPRTreeKey2@t@ *key,
                                                guint8 offset,
                                                VsgPRTreeKey2@t@ *result)
{
  result->x = key->x >> offset;
  result->y = key->y >> offset;
  result->depth = (guint8) MAX (0, ((gint8) key->depth) - offset);
}

static inline void vsg_prtree_key2@t@_sever (VsgPRTreeKey2@t@ *key, guint8 size,
                                             VsgPRTreeKey2@t@ *result)
{
  @key_type@ mask = VSG_PRTREE_KEY2@T@_INDEX_MASK (size-1);

  result->x &= mask;
  result->y &= mask;
  result->depth = size;
}

G_END_DECLS;

#endif /* __VSG_PRTREE_KEY2@T@_H__ */
