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

#ifndef __VSG_PRTREE2@T@_H__ /* __VSG_PRTREE2@T@_H__ */
#define __VSG_PRTREE2@T@_H__

#include <glib.h>
#include <glib-object.h>

#include <vsg/vsgvector2@t@.h>

#include <vsg/vsgprtree2-common.h>

G_BEGIN_DECLS;

/* macros */
#define VSG_TYPE_PRTREE2@T@ (vsg_prtree2@t@_get_type ())
#define VSG_TYPE_PRTREE2@T@_NODE_INFO (vsg_prtree2@t@_node_info_get_type ())

/* typedefs */
typedef struct _VsgPRTree2@t@ VsgPRTree2@t@;

typedef struct _VsgPRTree2@t@NodeInfo VsgPRTree2@t@NodeInfo;


typedef vsgloc2 (*VsgPoint2@t@LocFunc) (const VsgPoint2 candidate,
                                        const VsgVector2@t@ *center);


typedef vsgrloc2 (*VsgRegion2@t@LocFunc) (const VsgRegion2 region,
                                          const VsgVector2@t@ *center);

typedef @type@ (*VsgPoint2@t@DistFunc) (const VsgPoint2 one,
                                        const VsgPoint2 other);

typedef vsgloc2 (*VsgPoint2@t@LocMarshall) (const VsgPoint2 candidate,
                                            const VsgVector2@t@ *center,
                                            gpointer data);

typedef vsgrloc2 (*VsgRegion2@t@LocMarshall) (const VsgRegion2 region,
                                              const VsgVector2@t@ *center,
                                              gpointer data);

typedef @type@ (*VsgPoint2@t@DistMarshall) (const VsgPoint2 one,
                                            const VsgPoint2 other,
                                            gpointer data);

typedef void (*VsgPRTree2@t@Func) (const VsgPRTree2@t@NodeInfo *node_info,
                                   gpointer user_data);

/* structures */
struct _VsgPRTree2@t@NodeInfo {

  VsgVector2@t@ lbound;
  VsgVector2@t@ ubound;
  VsgVector2@t@ center;

  VsgPRTree2@t@NodeInfo *father_info;

  GSList *point_list;
  GSList *region_list;

  guint point_count;
  guint region_count;

  guint depth;

  gpointer user_data;

  gboolean isleaf;
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

#define vsg_prtree2@t@_new(lbound, ubound, region_locfunc) \
vsg_prtree2@t@_new_full (lbound, ubound, \
(VsgPoint2@t@LocFunc) vsg_vector2@t@_vector2@t@_locfunc, \
(VsgPoint2@t@DistFunc) vsg_vector2@t@_dist, region_locfunc, 0)

void vsg_prtree2@t@_free (VsgPRTree2@t@ *prtree2@t@);

VsgPRTree2@t@ *vsg_prtree2@t@_clone (VsgPRTree2@t@ *prtree2@t@);

void
vsg_prtree2@t@_set_point_loc_marshall (VsgPRTree2@t@ *prtree2@t@,
                                       VsgPoint2@t@LocMarshall marshall,
                                       gpointer locdata);

void vsg_prtree2@t@_set_region_loc_marshall (VsgPRTree2@t@ *prtree2@t@,
					     VsgRegion2@t@LocMarshall marshall,
					     gpointer locdata);

void
vsg_prtree2@t@_set_point_dist_marshall (VsgPRTree2@t@ *prtree2@t@,
                                        VsgPoint2@t@DistMarshall marshall,
                                        gpointer distdata);

@type@ vsg_prtree2@t@_get_tolerance (VsgPRTree2@t@ *prtree2@t@);

void vsg_prtree2@t@_set_tolerance (VsgPRTree2@t@ *prtree2@t@,
                                   @type@ tolerance);

void vsg_prtree2@t@_set_node_data (VsgPRTree2@t@ *prtree2@t@,
                                   GType user_data_type,
                                   gpointer user_data_model);

void vsg_prtree2@t@_get_bounds (VsgPRTree2@t@ *prtree2@t@,
                                VsgVector2@t@ *lbound,
                                VsgVector2@t@ *ubound);

guint vsg_prtree2@t@_depth (const VsgPRTree2@t@ *prtree2@t@);

guint vsg_prtree2@t@_point_count (const VsgPRTree2@t@ *prtree2@t@);

guint vsg_prtree2@t@_region_count (const VsgPRTree2@t@ *prtree2@t@);

void vsg_prtree2@t@_insert_point (VsgPRTree2@t@ *prtree2@t@,
                                  VsgPoint2 point);

gboolean vsg_prtree2@t@_remove_point (VsgPRTree2@t@ *prtree2@t@,
                                      VsgPoint2 point);

void vsg_prtree2@t@_insert_region (VsgPRTree2@t@ *prtree2@t@,
				   VsgRegion2 region);

gboolean vsg_prtree2@t@_remove_region (VsgPRTree2@t@ *prtree2@t@,
				       VsgRegion2 region);

void vsg_prtree2@t@_write (const VsgPRTree2@t@ *prtree2@t@,
                           FILE *file);
void vsg_prtree2@t@_print (const VsgPRTree2@t@ *prtree2@t@);

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
