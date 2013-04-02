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

#include "vsgprtree-parallel.h"

#include <string.h>

#ifdef VSG_HAVE_MPI

/**
 * vsg_parallel_migrate_vtable_set:
 * @vtable: a #VsgParallelMigrateVTable
 * @pack: a function to pack migrable data
 * @pack_data: user data to be passed to @pack
 * @unpack: a function to unpack migrable data
 * @unpack_data: user data to be passed to @unpack
 * @reduce: reduction operation on migrable data
 * @reduce_data: user data to be passed to @reduce
 * 
 * A convenience function that can be used to configure a
 * #VsgParallelMigrateVTable.
 *
 * Note: this function is available only when #VSG_HAVE_MPI is set.
 */
void vsg_parallel_migrate_vtable_set (VsgParallelMigrateVTable *vtable,
                                      VsgMigrablePackDataFunc pack,
                                      gpointer pack_data,
                                      VsgMigrableUnpackDataFunc unpack,
                                      gpointer unpack_data,
                                      VsgMigrableReductionDataFunc reduce,
                                      gpointer reduce_data)
{
  vtable->pack = pack;
  vtable->pack_data = pack_data;
  vtable->unpack = unpack;
  vtable->unpack_data = unpack_data;
  vtable->reduce = reduce;
  vtable->reduce_data = reduce_data;
}

/**
 * vsg_parallel_vtable_set_migrate:
 * @vtable: a #VsgParallelVTable
 * @pack: a function to pack migrable data
 * @pack_data: user data to be passed to @pack
 * @unpack: a function to unpack migrable data
 * @unpack_data: user data to be passed to @unpack
 * @reduce: reduction operation on migrable data
 * @reduce_data: user data to be passed to @reduce
 * 
 * A convenience function that can be used to configure a
 * #VsgParallelVTable.
 *
 * Note: this function is available only when #VSG_HAVE_MPI is set.
 */
void vsg_parallel_vtable_set_migrate (VsgParallelVTable *vtable,
                                      VsgMigrablePackDataFunc pack,
                                      gpointer pack_data,
                                      VsgMigrableUnpackDataFunc unpack,
                                      gpointer unpack_data,
                                      VsgMigrableReductionDataFunc reduce,
                                      gpointer reduce_data)
{
  vsg_parallel_migrate_vtable_set (&vtable->migrate, pack, pack_data,
                                   unpack, unpack_data,
                                   reduce, reduce_data);
}

/**
 * vsg_parallel_vtable_set_visit_forward:
 * @vtable: a #VsgParallelVTable
 * @pack: a function to pack migrable data
 * @pack_data: user data to be passed to @pack
 * @unpack: a function to unpack migrable data
 * @unpack_data: user data to be passed to @unpack
 * @reduce: reduction operation on migrable data
 * @reduce_data: user data to be passed to @reduce
 * 
 * A convenience function that can be used to configure a
 * #VsgParallelVTable.
 *
 * Note: this function is available only when #VSG_HAVE_MPI is set.
 */
void vsg_parallel_vtable_set_visit_forward (VsgParallelVTable *vtable,
                                            VsgMigrablePackDataFunc pack,
                                            gpointer pack_data,
                                            VsgMigrableUnpackDataFunc unpack,
                                            gpointer unpack_data,
                                            VsgMigrableReductionDataFunc reduce,
                                            gpointer reduce_data)
{
  vsg_parallel_migrate_vtable_set (&vtable->visit_forward, pack, pack_data,
                                   unpack, unpack_data,
                                   reduce, reduce_data);
}

/**
 * vsg_parallel_vtable_set_visit_backward:
 * @vtable: a #VsgParallelVTable
 * @pack: a function to pack migrable data
 * @pack_data: user data to be passed to @pack
 * @unpack: a function to unpack migrable data
 * @unpack_data: user data to be passed to @unpack
 * @reduce: reduction operation on migrable data
 * @reduce_data: user data to be passed to @reduce
 * 
 * A convenience function that can be used to configure a
 * #VsgParallelVTable.
 *
 * Note: this function is available only when #VSG_HAVE_MPI is set.
 */
void vsg_parallel_vtable_set_visit_backward (VsgParallelVTable *vtable,
                                             VsgMigrablePackDataFunc pack,
                                             gpointer pack_data,
                                             VsgMigrableUnpackDataFunc unpack,
                                             gpointer unpack_data,
                                             VsgMigrableReductionDataFunc reduce,
                                             gpointer reduce_data)
{
  vsg_parallel_migrate_vtable_set (&vtable->visit_backward, pack, pack_data,
                                   unpack, unpack_data,
                                   reduce, reduce_data);
}

/**
 * vsg_parallel_vtable_set_parallel:
 * @vtable: a #VsgParallelVTable
 * @pack: a function to pack migrable data
 * @pack_data: user data to be passed to @pack
 * @unpack: a function to unpack migrable data
 * @unpack_data: user data to be passed to @unpack
 * @reduce: reduction operation on migrable data
 * @reduce_data: user data to be passed to @reduce
 * @fwpack: a function to pack migrable data in a forward visit message
 * @fwpack_data: user data to be passed to @fwpack
 * @fwunpack: a function to unpack migrable data from a forward visit message
 * @fwunpack_data: user data to be passed to @fwunpack
 * @fwreduce: reduction operation on migrable data after unpacking a forward visit message
 * @fwreduce_data: user data to be passed to @fwreduce
 * @bwpack: a function to pack migrable data
 * @bwpack_data: user data to be passed to @bwpack in a backward visit message
 * @bwunpack: a function to unpack migrable data
 * @bwunpack_data: user data to be passed to @bwunpack from a backward visit message
 * @bwreduce: reduction operation on migrable data
 * @bwreduce_data: user data to be passed to @bwreduce after unpacking a backward visit message
 * 
 * A convenience function that can be used to configure a
 * #VsgParallelVTable.
 *
 * Note: this function is available only when #VSG_HAVE_MPI is set.
 */
void vsg_parallel_vtable_set_parallel (VsgParallelVTable *vtable,
                                       VsgMigrablePackDataFunc pack,
                                       gpointer pack_data,
                                       VsgMigrableUnpackDataFunc unpack,
                                       gpointer unpack_data,
                                       VsgMigrableReductionDataFunc reduce,
                                       gpointer reduce_data,
                                       VsgMigrablePackDataFunc fwpack,
                                       gpointer fwpack_data,
                                       VsgMigrableUnpackDataFunc fwunpack,
                                       gpointer fwunpack_data,
                                       VsgMigrableReductionDataFunc fwreduce,
                                       gpointer fwreduce_data,
                                       VsgMigrablePackDataFunc bwpack,
                                       gpointer bwpack_data,
                                       VsgMigrableUnpackDataFunc bwunpack,
                                       gpointer bwunpack_data,
                                       VsgMigrableReductionDataFunc bwreduce,
                                       gpointer bwreduce_data)
{
  vsg_parallel_vtable_set_migrate (vtable, pack, pack_data,
                                   unpack, unpack_data,
                                   reduce, reduce_data);

  vsg_parallel_vtable_set_visit_forward (vtable, fwpack, fwpack_data,
                                         fwunpack, fwunpack_data,
                                         fwreduce, fwreduce_data);

  vsg_parallel_vtable_set_visit_backward (vtable, bwpack, bwpack_data,
                                          bwunpack, bwunpack_data,
                                          bwreduce, bwreduce_data);
}

#endif

/**
 * vsg_parallel_vtable_set:
 * @vtable: a #VsgParallelVTable
 * @alloc: allocation function for a migrable data
 * @alloc_data: user data to be passed to @alloc
 * @destroy: destructor function for a migrable data
 * @destroy_data:  user data to be passed to @destroy
 * 
 * A convenience function that can be used to configure a
 * #VsgParallelVTable.
 */
void vsg_parallel_vtable_set (VsgParallelVTable *vtable,
                              VsgMigrableAllocDataFunc alloc,
                              gpointer alloc_data,
                              VsgMigrableDestroyDataFunc destroy,
                              gpointer destroy_data)
{
  vtable->alloc = alloc;
  vtable->alloc_data = alloc_data;
  vtable->destroy = destroy;
  vtable->destroy_data = destroy_data;
}

/**
 * vsg_prtree_parallel_config_set:
 * @config: a #VsgPRTreeParallelConfig
 * @point: VTable config for point data
 * @region: VTable config for region data
 * @node_data: VTable config for node_data
 * 
 * A convenience function that can be used to configure a
 * #VsgPRTreeParallelConfig.
 */
void vsg_prtree_parallel_config_set (VsgPRTreeParallelConfig *config,
                                     VsgParallelVTable *point,
                                     VsgParallelVTable *region,
                                     VsgParallelVTable *node_data)
{
  vsg_prtree_parallel_config_set_point (config, point);
  vsg_prtree_parallel_config_set_region (config, region);
  vsg_prtree_parallel_config_set_node_data (config, node_data);
}

/**
 * vsg_prtree_parallel_config_set_point:
 * @config: a #VsgPRTreeParallelConfig
 * @point: VTable config for point data
 * 
 * A convenience function that can be used to configure a
 * #VsgPRTreeParallelConfig.
 */
void vsg_prtree_parallel_config_set_point (VsgPRTreeParallelConfig *config,
                                           VsgParallelVTable *point)
{
  if (config != NULL && point != NULL)
    memcpy (&config->point, point, sizeof (VsgParallelVTable));
}

/**
 * vsg_prtree_parallel_config_set_region:
 * @config: a #VsgPRTreeParallelConfig
 * @region:  VTable config for region data
 * 
 * A convenience function that can be used to configure a
 * #VsgPRTreeParallelConfig.
 */
void vsg_prtree_parallel_config_set_region (VsgPRTreeParallelConfig *config,
                                            VsgParallelVTable *region)
{
  if (config != NULL && region != NULL)
    memcpy (&config->region,region , sizeof (VsgParallelVTable));
}

/**
 * vsg_prtree_parallel_config_set_node_data:
 * @config: a #VsgPRTreeParallelConfig
 * @node_data: VTable config for node_data
 * 
 * A convenience function that can be used to configure a
 * #VsgPRTreeParallelConfig.
 */
void vsg_prtree_parallel_config_set_node_data (VsgPRTreeParallelConfig *config,
                                               VsgParallelVTable *node_data)
{
  if (config != NULL && node_data != NULL)
    memcpy (&config->node_data, node_data, sizeof (VsgParallelVTable));
}

#ifdef VSG_HAVE_MPI
/**
 * vsg_prtree_parallel_config_set_communicator:
 * @config: a #VsgPRTreeParallelConfig
 * @communicator: the #MPI_Communicator that shares this particular PRTree
 * 
 * A convenience function that can be used to configure a
 * #VsgPRTreeParallelConfig.
 *
 * Note: this function is available only when #VSG_HAVE_MPI is set.
 */
void
vsg_prtree_parallel_config_set_communicator (VsgPRTreeParallelConfig *config,
                                             MPI_Comm communicator)
{
  if (config != NULL)
    config->communicator = communicator;
}

/**
 * vsg_prtree_parallel_config_get_communicator:
 * @config: a #VsgPRTreeParallelConfig
 * 
 *
 * Returns the #MPI_Communicator that shares this particular PRTree
 */
MPI_Comm
vsg_prtree_parallel_config_get_communicator (VsgPRTreeParallelConfig *config)
{
  if (config != NULL)
    return config->communicator;

  return NULL;
}

#endif

