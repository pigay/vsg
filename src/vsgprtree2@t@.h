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

#ifndef __VSG_PRTREE2@T@_H__ /* __VSG_PRTREE2@T@_H__ */
#define __VSG_PRTREE2@T@_H__

#include <glib.h>
#include <glib-object.h>

#include <vsg/vsgvector2@t@.h>

#include <vsg/vsgprtree-common.h>

#include <vsg/vsgprtree-parallel.h>

#include <vsg/vsgprtree2-common.h>

#include <vsg/vsgprtreekey2@t@.h>

G_BEGIN_DECLS;

/* macros */
#define VSG_TYPE_PRTREE2@T@ (vsg_prtree2@t@_get_type ())
#define VSG_TYPE_PRTREE2@T@_NODE_INFO (vsg_prtree2@t@_node_info_get_type ())

#define VSG_PRTREE2@T@_NODE_INFO_IS_REMOTE(node_info) ( \
VSG_PARALLEL_STATUS_IS_REMOTE ((node_info)->parallel_status) \
)

#define VSG_PRTREE2@T@_NODE_INFO_IS_LOCAL(node_info) ( \
VSG_PARALLEL_STATUS_IS_LOCAL ((node_info)->parallel_status) \
)

#define VSG_PRTREE2@T@_NODE_INFO_IS_SHARED(node_info) ( \
VSG_PARALLEL_STATUS_IS_SHARED ((node_info)->parallel_status) \
)

#define VSG_PRTREE2@T@_NODE_INFO_IS_PRIVATE(node_info) ( \
VSG_PARALLEL_STATUS_IS_PRIVATE (node_info->parallel_status) \
)

#define VSG_PRTREE2@T@_NODE_INFO_IS_PRIVATE_LOCAL(node_info) ( \
VSG_PARALLEL_STATUS_IS_PRIVATE_LOCAL (node_info->parallel_status) \
)

#define VSG_PRTREE2@T@_NODE_INFO_IS_SHARED_LOCAL(node_info) ( \
VSG_PARALLEL_STATUS_IS_SHARED_LOCAL (node_info->parallel_status) \
)

#define VSG_PRTREE2@T@_NODE_INFO_IS_PRIVATE_REMOTE(node_info) ( \
VSG_PARALLEL_STATUS_IS_PRIVATE_REMOTE (node_info->parallel_status) \
)

#define VSG_PRTREE2@T@_NODE_INFO_IS_SHARED_REMOTE(node_info) ( \
VSG_PARALLEL_STATUS_IS_SHARED_REMOTE (node_info->parallel_status) \
)

#define VSG_PRTREE2@T@_NODE_INFO_PROC(node_info) ( \
VSG_PARALLEL_STATUS_PROC ((node_info)->parallel_status) \
)

/* typedefs */
typedef struct _VsgPRTree2@t@ VsgPRTree2@t@;

typedef struct _VsgPRTree2@t@NodeInfo VsgPRTree2@t@NodeInfo;


typedef vsgloc2 (*VsgPoint2@t@LocFunc) (const VsgPoint2 candidate,
                                        const VsgVector2@t@ *center);


typedef vsgrloc2 (*VsgRegion2@t@LocFunc) (const VsgRegion2 region,
                                          const VsgVector2@t@ *center);

typedef @type@ (*VsgPoint2@t@DistFunc) (const VsgPoint2 one,
                                        const VsgPoint2 other);

typedef vsgloc2 (*VsgPoint2@t@LocDataFunc) (const VsgPoint2 candidate,
                                            const VsgVector2@t@ *center,
                                            gpointer data);

typedef vsgrloc2 (*VsgRegion2@t@LocDataFunc) (const VsgRegion2 region,
                                              const VsgVector2@t@ *center,
                                              gpointer data);

typedef @type@ (*VsgPoint2@t@DistDataFunc) (const VsgPoint2 one,
                                            const VsgPoint2 other,
                                            gpointer data);

typedef void (*VsgPRTree2@t@Func) (const VsgPRTree2@t@NodeInfo *node_info,
                                   gpointer user_data);

typedef gboolean (*VsgPRTree2@t@NFIsleafFunc) (const VsgPRTree2@t@NodeInfo *node_info,
                                               gpointer user_data);

/* structures */
struct _VsgPRTree2@t@NodeInfo {

  VsgVector2@t@ center;
  VsgVector2@t@ lbound;
  VsgVector2@t@ ubound;

  VsgPRTree2@t@NodeInfo *father_info;

  GSList *point_list;
  GSList *region_list;

  guint point_count;
  guint region_count;

  guint depth;

  gpointer user_data;

  gboolean isleaf;

  VsgPRTreeKey2@t@ id;

  VsgParallelStatus parallel_status;
};

/* functions */

GType vsg_prtree2@t@_node_info_get_type (void) G_GNUC_CONST;
void vsg_prtree2@t@_node_info_free (VsgPRTree2@t@NodeInfo *node_info);
VsgPRTree2@t@NodeInfo *
vsg_prtree2@t@_node_info_clone (VsgPRTree2@t@NodeInfo *node_info);

GType vsg_prtree2@t@_get_type (void) G_GNUC_CONST;

VsgPRTree2@t@ *
vsg_prtree2@t@_new_full (const VsgVector2@t@ *lbound,
                         const VsgVector2@t@ *ubound,
                         const VsgPoint2@t@LocFunc point_locfunc,
                         const VsgPoint2@t@DistFunc point_distfunc,
                         const VsgRegion2@t@LocFunc region_locfunc,
                         guint max_point);

#define vsg_prtree2@t@_new(lbound, ubound, region_locfunc, maxpoint) \
vsg_prtree2@t@_new_full (lbound, ubound, \
(VsgPoint2@t@LocFunc) vsg_vector2@t@_vector2@t@_locfunc, \
(VsgPoint2@t@DistFunc) vsg_vector2@t@_dist, region_locfunc, (maxpoint))

void vsg_prtree2@t@_free (VsgPRTree2@t@ *prtree2@t@);

VsgPRTree2@t@ *vsg_prtree2@t@_clone (VsgPRTree2@t@ *prtree2@t@);

guint vsg_prtree2@t@_get_max_point (VsgPRTree2@t@ *prtree2@t@);

void
vsg_prtree2@t@_set_point_loc (VsgPRTree2@t@ *prtree2@t@,
                              VsgPoint2@t@LocFunc locfunc);

void
vsg_prtree2@t@_set_point_loc_with_data (VsgPRTree2@t@ *prtree2@t@,
                                        VsgPoint2@t@LocDataFunc locfunc,
                                        gpointer locdata);

void
vsg_prtree2@t@_set_region_loc (VsgPRTree2@t@ *prtree2@t@,
                               VsgRegion2@t@LocFunc locfunc);

void
vsg_prtree2@t@_set_region_loc_with_data (VsgPRTree2@t@ *prtree2@t@,
                                         VsgRegion2@t@LocDataFunc locfunc,
                                         gpointer locdata);

void
vsg_prtree2@t@_set_point_dist (VsgPRTree2@t@ *prtree2@t@,
                               VsgPoint2@t@DistFunc distfunc);

void
vsg_prtree2@t@_set_point_dist_with_data (VsgPRTree2@t@ *prtree2@t@,
                                         VsgPoint2@t@DistDataFunc distfunc,
                                         gpointer distdata);

void
vsg_prtree2@t@_set_children_order_with_data (VsgPRTree2@t@ *prtree2@t@,
                                             VsgChildrenOrderDataFunc children_order,
                                             gpointer root_key,
                                             gpointer user_data);

void
vsg_prtree2@t@_set_children_order (VsgPRTree2@t@ *prtree2@t@,
                                   VsgChildrenOrderFunc children_order,
                                   gpointer root_key);

@type@ vsg_prtree2@t@_get_tolerance (VsgPRTree2@t@ *prtree2@t@);

void vsg_prtree2@t@_set_tolerance (VsgPRTree2@t@ *prtree2@t@,
                                   @type@ tolerance);

void vsg_prtree2@t@_set_node_data (VsgPRTree2@t@ *prtree2@t@,
                                   GType user_data_type,
                                   gpointer user_data_model);

void vsg_prtree2@t@_set_node_data_vtable (VsgPRTree2@t@ *prtree2@t@,
                                          VsgParallelVTable *vtable);

void vsg_prtree2@t@_get_bounds (VsgPRTree2@t@ *prtree2@t@,
                                VsgVector2@t@ *lbound,
                                VsgVector2@t@ *ubound);

guint vsg_prtree2@t@_depth (const VsgPRTree2@t@ *prtree2@t@);

guint vsg_prtree2@t@_point_count (const VsgPRTree2@t@ *prtree2@t@);

guint vsg_prtree2@t@_region_count (const VsgPRTree2@t@ *prtree2@t@);

void vsg_prtree2@t@_insert_point (VsgPRTree2@t@ *prtree2@t@,
                                  VsgPoint2 point);

gboolean vsg_prtree2@t@_insert_point_local (VsgPRTree2@t@ *prtree2@t@,
                                            VsgPoint2 point);

gboolean vsg_prtree2@t@_remove_point (VsgPRTree2@t@ *prtree2@t@,
                                      VsgPoint2 point);

gboolean vsg_prtree2@t@_move_point (VsgPRTree2@t@ *prtree2@t@,
				    VsgPoint2 point,
				    GFunc move_func,
				    gpointer move_data);

void vsg_prtree2@t@_insert_region (VsgPRTree2@t@ *prtree2@t@,
				   VsgRegion2 region);

gboolean vsg_prtree2@t@_remove_region (VsgPRTree2@t@ *prtree2@t@,
				       VsgRegion2 region);

void vsg_prtree2@t@_write (VsgPRTree2@t@ *prtree2@t@,
                           FILE *file);
void vsg_prtree2@t@_print (VsgPRTree2@t@ *prtree2@t@);

VsgPoint2 vsg_prtree2@t@_find_point (VsgPRTree2@t@ *prtree2@t@,
                                     VsgPoint2 selector);

void vsg_prtree2@t@_foreach_point (VsgPRTree2@t@ *prtree2@t@,
                                   GFunc func,
                                   gpointer user_data);

void
vsg_prtree2@t@_foreach_point_custom (VsgPRTree2@t@ *prtree2@t@,
                                     VsgRegion2 selector,
                                     VsgRegion2Point2LocFunc locfunc,
                                     GFunc func,
                                     gpointer user_data);

void vsg_prtree2@t@_foreach_region (VsgPRTree2@t@ *prtree2@t@,
				    GFunc func,
				    gpointer user_data);

void vsg_prtree2@t@_foreach_region_custom (VsgPRTree2@t@ *prtree2@t@,
					   VsgRegion2 selector,
					   GFunc func,
					   gpointer user_data);

VsgRegion2 vsg_prtree2@t@_find_deep_region (VsgPRTree2@t@ *prtree2@t@,
                                            VsgPoint2 point,
                                            VsgRegion2CheckFunc check,
                                            gpointer user_data);

void vsg_prtree2@t@_traverse (VsgPRTree2@t@ *prtree2@t@,
                              GTraverseType order,
                              VsgPRTree2@t@Func func,
                              gpointer user_data);

void vsg_prtree2@t@_traverse_custom (VsgPRTree2@t@ *prtree2@t@,
                                     GTraverseType order,
                                     VsgRegion2 selector,
                                     VsgPRTree2@t@Func func,
                                     gpointer user_data);

G_END_DECLS;

#endif /* __VSG_PRTREE2@T@_H__ */
