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

#ifndef __VSGPRTREE2@T@_PARALLEL_H__
#define __VSGPRTREE2@T@_PARALLEL_H__

#include "vsgprtree-parallel.h"
#include "vsgprtree2@t@.h"

G_BEGIN_DECLS;

typedef
gint (*VsgPRTree2@t@DistributionFunc) (VsgPRTree2@t@NodeInfo *node_info,
                                       gpointer user_data);

void vsg_prtree2@t@_set_parallel (VsgPRTree2@t@ *tree,
                                  VsgPRTreeParallelConfig *pconfig);

void vsg_prtree2@t@_get_parallel (VsgPRTree2@t@ *tree,
                                  VsgPRTreeParallelConfig *pconfig);

void vsg_prtree2@t@_migrate_flush (VsgPRTree2@t@ *tree);

void vsg_prtree2@t@_distribute_nodes (VsgPRTree2@t@ *tree,
                                      VsgPRTree2@t@DistributionFunc func,
                                      gpointer user_data);

void vsg_prtree2@t@_distribute_contiguous_leaves (VsgPRTree2@t@ *tree);

void vsg_prtree2@t@_distribute_scatter_leaves (VsgPRTree2@t@ *tree);

void vsg_prtree2@t@_distribute_concentrate (VsgPRTree2@t@ *tree, gint dst);

void
vsg_prtree2@t@_shared_nodes_allreduce (VsgPRTree2@t@ *tree,
                                       VsgParallelMigrateVTable *data_vtable);

G_END_DECLS;

#endif /* __VSGPRTREE2@T@_PARALLEL_H__ */
