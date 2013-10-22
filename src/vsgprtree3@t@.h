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

#ifndef __VSG_PRTREE3@T@_H__ /* __VSG_PRTREE3@T@_H__ */
#define __VSG_PRTREE3@T@_H__

#include <glib.h>
#include <glib-object.h>

#include <vsg/vsgvector3@t@.h>

#include <vsg/vsgprtree-common.h>

#include <vsg/vsgprtree-parallel.h>

#include <vsg/vsgprtree3-common.h>

#include <vsg/vsgprtreekey3@t@.h>

G_BEGIN_DECLS;

/* macros */
#define VSG_TYPE_PRTREE3@T@ (vsg_prtree3@t@_get_type ())
#define VSG_TYPE_PRTREE3@T@_NODE_INFO (vsg_prtree3@t@_node_info_get_type ())

#define VSG_PRTREE3@T@_NODE_INFO_IS_REMOTE(node_info) ( \
VSG_PARALLEL_STATUS_IS_REMOTE ((node_info)->parallel_status) \
)

#define VSG_PRTREE3@T@_NODE_INFO_IS_LOCAL(node_info) ( \
VSG_PARALLEL_STATUS_IS_LOCAL ((node_info)->parallel_status) \
)

#define VSG_PRTREE3@T@_NODE_INFO_IS_SHARED(node_info) ( \
VSG_PARALLEL_STATUS_IS_SHARED ((node_info)->parallel_status) \
)

#define VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE(node_info) ( \
VSG_PARALLEL_STATUS_IS_PRIVATE (node_info->parallel_status) \
)

#define VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_LOCAL(node_info) ( \
VSG_PARALLEL_STATUS_IS_PRIVATE_LOCAL (node_info->parallel_status) \
)

#define VSG_PRTREE3@T@_NODE_INFO_IS_SHARED_LOCAL(node_info) ( \
VSG_PARALLEL_STATUS_IS_SHARED_LOCAL (node_info->parallel_status) \
)

#define VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_REMOTE(node_info) ( \
VSG_PARALLEL_STATUS_IS_PRIVATE_REMOTE (node_info->parallel_status) \
)

#define VSG_PRTREE3@T@_NODE_INFO_IS_SHARED_REMOTE(node_info) ( \
VSG_PARALLEL_STATUS_IS_SHARED_REMOTE (node_info->parallel_status) \
)

#define VSG_PRTREE3@T@_NODE_INFO_PROC(node_info) ( \
VSG_PARALLEL_STATUS_PROC ((node_info)->parallel_status) \
)

/* typedefs */
typedef struct _VsgPRTree3@t@ VsgPRTree3@t@;

typedef struct _VsgPRTree3@t@NodeInfo VsgPRTree3@t@NodeInfo;

typedef vsgloc3 (*VsgPoint3@t@LocFunc) (const VsgPoint3 candidate,
                                        const VsgVector3@t@ *center);


typedef vsgrloc3 (*VsgRegion3@t@LocFunc) (const VsgRegion3 region,
                                          const VsgVector3@t@ *center);

typedef @type@ (*VsgPoint3@t@DistFunc) (const VsgPoint3 one,
                                        const VsgPoint3 other);

typedef vsgloc3 (*VsgPoint3@t@LocDataFunc) (const VsgPoint3 candidate,
                                            const VsgVector3@t@ *center,
                                            gpointer data);

typedef vsgrloc3 (*VsgRegion3@t@LocDataFunc) (const VsgRegion3 region,
                                              const VsgVector3@t@ *center,
                                              gpointer data);

typedef @type@ (*VsgPoint3@t@DistDataFunc) (const VsgPoint3 one,
                                            const VsgPoint3 other,
                                            gpointer data);

typedef void (*VsgPRTree3@t@Func) (const VsgPRTree3@t@NodeInfo *node_info,
                                   gpointer user_data);

typedef gboolean (*VsgPRTree3@t@NFIsleafFunc) (const VsgPRTree3@t@NodeInfo *node_info,
                                               gpointer user_data);

/* structures */
struct _VsgPRTree3@t@NodeInfo {

  VsgVector3@t@ center;
  VsgVector3@t@ lbound;
  VsgVector3@t@ ubound;

  VsgPRTree3@t@NodeInfo *father_info;

  GSList *point_list;
  GSList *region_list;

  guint point_count;
  guint region_count;

  guint depth;

  gpointer user_data;

  gboolean isleaf;

  VsgPRTreeKey3@t@ id;

  VsgParallelStatus parallel_status;
};

/* functions */

GType vsg_prtree3@t@_node_info_get_type (void) G_GNUC_CONST;
void vsg_prtree3@t@_node_info_free (VsgPRTree3@t@NodeInfo *node_info);
VsgPRTree3@t@NodeInfo *
vsg_prtree3@t@_node_info_clone (VsgPRTree3@t@NodeInfo *node_info);

GType vsg_prtree3@t@_get_type (void) G_GNUC_CONST;

VsgPRTree3@t@ *
vsg_prtree3@t@_new_full (const VsgVector3@t@ *lbound,
                         const VsgVector3@t@ *ubound,
                         const VsgPoint3@t@LocFunc point_locfunc,
                         const VsgPoint3@t@DistFunc point_distfunc,
                         const VsgRegion3@t@LocFunc region_locfunc,
                         guint max_point);

#define vsg_prtree3@t@_new(lbound, ubound, region_locfunc, maxpoint) \
vsg_prtree3@t@_new_full (lbound, ubound, \
(VsgPoint3@t@LocFunc) vsg_vector3@t@_vector3@t@_locfunc, \
(VsgPoint3@t@DistFunc) vsg_vector3@t@_dist, region_locfunc, (maxpoint))

void vsg_prtree3@t@_free (VsgPRTree3@t@ *prtree3@t@);

VsgPRTree3@t@ *vsg_prtree3@t@_clone (VsgPRTree3@t@ *prtree3@t@);

guint vsg_prtree3@t@_get_max_point (VsgPRTree3@t@ *prtree3@t@);

void
vsg_prtree3@t@_set_point_loc (VsgPRTree3@t@ *prtree3@t@,
                              VsgPoint3@t@LocFunc locfunc);

void
vsg_prtree3@t@_set_point_loc_with_data (VsgPRTree3@t@ *prtree3@t@,
                                       VsgPoint3@t@LocDataFunc locfunc,
                                       gpointer locdata);

void vsg_prtree3@t@_set_region_loc (VsgPRTree3@t@ *prtree3@t@,
                                    VsgRegion3@t@LocFunc locfunc);

void vsg_prtree3@t@_set_region_loc_with_data (VsgPRTree3@t@ *prtree3@t@,
                                              VsgRegion3@t@LocDataFunc locfunc,
                                              gpointer locdata);

void
vsg_prtree3@t@_set_point_dist (VsgPRTree3@t@ *prtree3@t@,
                               VsgPoint3@t@DistFunc distfunc);

void
vsg_prtree3@t@_set_point_dist_with_data (VsgPRTree3@t@ *prtree3@t@,
                                         VsgPoint3@t@DistDataFunc distfunc,
                                         gpointer distdata);

void
vsg_prtree3@t@_set_children_order_with_data (VsgPRTree3@t@ *prtree3@t@,
                                             VsgChildrenOrderDataFunc children_order,
                                             gpointer root_key,
                                             gpointer user_data);
void
vsg_prtree3@t@_set_children_order (VsgPRTree3@t@ *prtree3@t@,
                                   VsgChildrenOrderFunc children_order,
                                   gpointer root_key);

@type@ vsg_prtree3@t@_get_tolerance (VsgPRTree3@t@ *prtree3@t@);

void vsg_prtree3@t@_set_tolerance (VsgPRTree3@t@ *prtree3@t@,
                                   @type@ tolerance);

void vsg_prtree3@t@_set_node_data (VsgPRTree3@t@ *prtree3@t@,
                                   GType user_data_type,
                                   gpointer user_data_model);

void vsg_prtree3@t@_set_node_data_vtable (VsgPRTree3@t@ *prtree3@t@,
                                          VsgParallelVTable *vtable);

void vsg_prtree3@t@_get_bounds (VsgPRTree3@t@ *prtree3@t@,
                                VsgVector3@t@ *lbound,
                                VsgVector3@t@ *ubound);

guint vsg_prtree3@t@_depth (const VsgPRTree3@t@ *prtree3@t@);

guint vsg_prtree3@t@_point_count (const VsgPRTree3@t@ *prtree3@t@);

guint vsg_prtree3@t@_region_count (const VsgPRTree3@t@ *prtree3@t@);

void vsg_prtree3@t@_insert_point (VsgPRTree3@t@ *prtree3@t@,
                                  VsgPoint3 point);

gboolean vsg_prtree3@t@_insert_point_local (VsgPRTree3@t@ *prtree3@t@,
                                            VsgPoint3 point);

gboolean vsg_prtree3@t@_remove_point (VsgPRTree3@t@ *prtree3@t@,
                                      VsgPoint3 point);

gboolean vsg_prtree3@t@_move_point (VsgPRTree3@t@ *prtree3@t@,
				    VsgPoint3 point,
				    GFunc move_func,
				    gpointer move_data);

void vsg_prtree3@t@_insert_region (VsgPRTree3@t@ *prtree3@t@,
				   VsgRegion3 region);

gboolean vsg_prtree3@t@_remove_region (VsgPRTree3@t@ *prtree3@t@,
				       VsgRegion3 region);

void vsg_prtree3@t@_write (VsgPRTree3@t@ *prtree3@t@,
                           FILE *file);
void vsg_prtree3@t@_print (VsgPRTree3@t@ *prtree3@t@);

VsgPoint3 vsg_prtree3@t@_find_point (VsgPRTree3@t@ *prtree3@t@,
                                     VsgPoint3 selector);

void vsg_prtree3@t@_foreach_point (VsgPRTree3@t@ *prtree3@t@,
                                   GFunc func,
                                   gpointer user_data);

void
vsg_prtree3@t@_foreach_point_custom (VsgPRTree3@t@ *prtree3@t@,
                                     VsgRegion3 selector,
                                     VsgRegion3Point3LocFunc locfunc,
                                     GFunc func,
                                     gpointer user_data);

void vsg_prtree3@t@_foreach_region (VsgPRTree3@t@ *prtree3@t@,
				    GFunc func,
				    gpointer user_data);

void vsg_prtree3@t@_foreach_region_custom (VsgPRTree3@t@ *prtree3@t@,
					   VsgRegion3 selector,
					   GFunc func,
					   gpointer user_data);

VsgRegion3 vsg_prtree3@t@_find_deep_region (VsgPRTree3@t@ *prtree3@t@,
                                            VsgPoint3 point,
                                            VsgRegion3CheckFunc check,
                                            gpointer user_data);

void vsg_prtree3@t@_traverse (VsgPRTree3@t@ *prtree3@t@,
                              GTraverseType order,
                              VsgPRTree3@t@Func func,
                              gpointer user_data);

void vsg_prtree3@t@_traverse_custom (VsgPRTree3@t@ *prtree3@t@,
                                     GTraverseType order,
                                     VsgRegion3 selector,
                                     VsgPRTree3@t@Func func,
                                     gpointer user_data);

G_END_DECLS;

#endif /* __VSG_PRTREE3@T@_H__ */
