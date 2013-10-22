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

#ifndef __VSGPRTREE3@T@_PRIVATE_H__
#define __VSGPRTREE3@T@_PRIVATE_H__

#include "vsgprtree-parallel.h"
#include "vsgprtree3@t@.h"
#include "vsgprtree3@t@-extras.h"

G_BEGIN_DECLS;

/* binary localization macros: */
#define VSG_LOC3_COMP(loc,axis) ( \
 (vsgloc3) VSG_LOC3_MASK & ((loc) & (axis)) \
)

#define VSG_LOC3_ALONG_X(loc) ( VSG_LOC3_COMP (loc, VSG_LOC3_X) )
#define VSG_LOC3_ALONG_Y(loc) ( VSG_LOC3_COMP (loc, VSG_LOC3_Y) )
#define VSG_LOC3_ALONG_Z(loc) ( VSG_LOC3_COMP (loc, VSG_LOC3_Z) )

/* 
 * Convenience macros.
 */
#define CALL_POINT3@T@_LOC(config, one, other) ( \
(config)->point_loc_func ((one), (other), (config)->point_loc_data) \
)

#define CALL_REGION3@T@_LOC(config, one, other) ( \
(config)->region_loc_func ((one), (other), (config)->region_loc_data) \
)

#define CALL_POINT3@T@_DIST(config, one, other) ( \
(config)->point_dist_func ((one), (other), (config)->point_dist_data) \
)

/* forward typedefs */
typedef struct _VsgPRTree3@t@Leaf VsgPRTree3@t@Leaf;
typedef struct _VsgPRTree3@t@Int VsgPRTree3@t@Int;
typedef struct _VsgPRTree3@t@Node VsgPRTree3@t@Node;

typedef struct _VsgPRTree3@t@Config VsgPRTree3@t@Config;

typedef struct _VsgNFConfig3@t@ VsgNFConfig3@t@;

typedef void (*VsgPRTree3@t@InternalFunc) (VsgPRTree3@t@Node *node,
                                           VsgPRTree3@t@NodeInfo *info,
                                           gpointer user_data);

typedef vsgrloc3
(*VsgRegion3@t@InternalLocDataFunc) (const VsgRegion3 region,
                                     const VsgPRTree3@t@NodeInfo *node_info,
                                     gpointer data);


/* private structs */
struct _VsgPRTree3@t@Leaf {

  gpointer isint;
  GSList *point;
  guint8 remote_depth; /* depth of the subtree in case this node is
                         remote */
};

struct _VsgPRTree3@t@Int {

  VsgPRTree3@t@Node *children[VSG_PRTREE3_CN];
};

struct _VsgPRTree3@t@Node {

  union {
    gpointer isint; /* first pointer of struct is 0 if leaf, not 0 otherwise */
    VsgPRTree3@t@Int interior;
    VsgPRTree3@t@Leaf leaf;
  } variable;

  /* bounding box limits */
  VsgVector3@t@ lbound;
  VsgVector3@t@ ubound;

  /* center of the box */
  VsgVector3@t@ center;

  /* counts */
  guint point_count;
  guint region_count;

  /* VsgRegion3 list */
  GSList *region_list;

  /* user data */
  gpointer user_data;

  /* parallel status */
  VsgParallelStatus parallel_status;
};

#define PRTREE3@T@NODE_ISINT(node) ( \
((node) != NULL) && ((node)->variable.isint != 0) \
)
#define PRTREE3@T@NODE_ISLEAF(node) ( \
((node) != NULL) && ((node)->variable.isint == 0) \
)

#define PRTREE3@T@NODE_INT(node) ( \
(node)->variable.interior \
)

#define PRTREE3@T@NODE_CHILD(node, i) ( \
PRTREE3@T@NODE_INT(node).children[i] \
)

#define PRTREE3@T@NODE_LEAF(node) ( \
(node)->variable.leaf \
)

#define PRTREE3@T@NODE_IS_REMOTE(node) ( \
VSG_PARALLEL_STATUS_IS_REMOTE (node->parallel_status) \
)

#define PRTREE3@T@NODE_IS_LOCAL(node) ( \
VSG_PARALLEL_STATUS_IS_LOCAL (node->parallel_status) \
)

#define PRTREE3@T@NODE_IS_SHARED(node) ( \
VSG_PARALLEL_STATUS_IS_SHARED (node->parallel_status) \
)

#define PRTREE3@T@NODE_IS_PRIVATE(node) ( \
VSG_PARALLEL_STATUS_IS_PRIVATE (node->parallel_status) \
)

#define PRTREE3@T@NODE_IS_PRIVATE_LOCAL(node) ( \
VSG_PARALLEL_STATUS_IS_PRIVATE_LOCAL (node->parallel_status) \
)

#define PRTREE3@T@NODE_IS_SHARED_LOCAL(node) ( \
VSG_PARALLEL_STATUS_IS_SHARED_LOCAL (node->parallel_status) \
)

#define PRTREE3@T@NODE_IS_PRIVATE_REMOTE(node) ( \
VSG_PARALLEL_STATUS_IS_PRIVATE_REMOTE (node->parallel_status) \
)

#define PRTREE3@T@NODE_IS_SHARED_REMOTE(node) ( \
VSG_PARALLEL_STATUS_IS_SHARED_REMOTE (node->parallel_status) \
)

#define PRTREE3@T@NODE_PROC(node) ( \
VSG_PARALLEL_STATUS_PROC (node->parallel_status) \
)

#define PRTREE3@T@_NODE_INFO_CALL_NF_ISLEAF(node_info, config) ( \
  (config)->nf_isleaf ((node_info), (config)->nf_isleaf_data) \
)

#define PRTREE3@T@_NODE_INFO_NF_ISLEAF(node_info,config) ( \
  (node_info)->isleaf || \
  (! VSG_PRTREE3@T@_NODE_INFO_IS_SHARED (node_info) && \
   (config)->nf_isleaf ((node_info), (config)->nf_isleaf_data)) \
)

#define PRTREE3@T@_NODE_INFO_NF_ISINT(node_info,config) ( \
  ! PRTREE3@T@_NODE_INFO_NF_ISLEAF (node_info, config) \
)


struct _VsgPRTree3@t@Config {

  /* localization methods */
  VsgPoint3@t@LocDataFunc point_loc_func;
  gpointer point_loc_data;

  VsgRegion3@t@LocDataFunc region_loc_func;
  gpointer region_loc_data;

  /* point distance func */
  VsgPoint3@t@DistDataFunc point_dist_func;
  gpointer point_dist_data;

  /* config */
  guint max_point;

  VsgPRTree3@t@NFIsleafFunc nf_isleaf;
  gpointer nf_isleaf_data;

  /* spatial tolerance */
  @type@ tolerance;


  /* children order in traversals */
  VsgChildrenOrderDataFunc children_order;
  gpointer children_order_data;
  gpointer root_key;

  /* parallel tree configuration */
  VsgPRTreeParallelConfig parallel_config;

  gboolean remote_depth_dirty; /* flag indicating if remote depths
                                  are up-to-date */
};

struct _VsgPRTree3@t@ {

  /* node part */
  VsgPRTree3@t@Node *node;

  /* tree configuration */
  VsgPRTree3@t@Config config;

  /* place to store pending message of inter processor VsgRegion */
  GSList *pending_shared_regions;

  /* place to store pending message of exterior VsgPoint */
  GSList *pending_exterior_points;
};

struct _VsgNFConfig3@t@
{
  VsgPRTree3@t@ *tree;

  VsgPRTree3@t@FarInteractionFunc far_func;
  VsgPRTree3@t@InteractionFunc near_func;
  VsgPRTree3@t@SemifarInteractionFunc semifar_func;
  guint semifar_threshold;
  gpointer user_data;

  /* parallel data */
  gint rk, sz;

#ifdef VSG_HAVE_MPI
  VsgPackedMsg recv;
  gboolean recv_lock;
  GHashTable *procs_msgs;
  MPI_Request *procs_requests;
  gpointer tmp_node_data;
  gpointer tmp_point;
  gpointer tmp_region;
  gint forward_pending_nb;
  gint backward_pending_nb;
  gint pending_end_forward;
  gint pending_backward_msgs;

  gint all_fw_sends, all_fw_recvs;
  gint all_bw_sends, all_bw_recvs;

  gint shared_far_interaction_counter;
#endif
};

/* private functions */

void vsg_prtree3@t@node_free (VsgPRTree3@t@Node *node,
                              const VsgPRTree3@t@Config *config);

VsgPRTree3@t@Node *
vsg_prtree3@t@node_alloc_no_data (const VsgVector3@t@ *lbound,
                                  const VsgVector3@t@ *ubound);

VsgPRTree3@t@Node *
vsg_prtree3@t@node_alloc (const VsgVector3@t@ *lbound,
                          const VsgVector3@t@ *ubound,
                          const VsgPRTree3@t@Config *config);

void vsg_prtree3@t@node_dealloc (VsgPRTree3@t@Node *prtree3@t@node);

void _vsg_prtree3@t@node_get_info (VsgPRTree3@t@Node *node,
                                   VsgPRTree3@t@NodeInfo *node_info,
                                   VsgPRTree3@t@NodeInfo *father_info,
                                   guint8 child_number);

VsgPRTree3@t@Node *_vsg_prtree3@t@node_get_child_at (VsgPRTree3@t@Node *node,
                                                     const VsgVector3@t@ *pos,
                                                     gint depth);

VsgPRTree3@t@Node *vsg_prtree3@t@node_key_lookup (VsgPRTree3@t@Node *node,
                                                  VsgPRTreeKey3@t@ key);

void
vsg_prtree3@t@_traverse_custom_internal (VsgPRTree3@t@ *prtree3@t@,
                                         GTraverseType order,
                                         VsgRegion3@t@InternalLocDataFunc sel_func,
                                         VsgRegion3 selector,
                                         gpointer sel_data,
                                         VsgPRTree3@t@InternalFunc func,
                                         gpointer user_data);

guint
vsg_prtree3@t@node_insert_point_list(VsgPRTree3@t@Node *node,
                                     GSList *point,
                                     const VsgPRTree3@t@Config *config);

void vsg_prtree3@t@_bounds_extend (VsgPRTree3@t@ *prtree3@t@,
                                   VsgPoint3 point, VsgPRTreeKey3@t@ *extk);

void vsg_prtree3@t@node_make_int (VsgPRTree3@t@Node *node,
                                  const VsgPRTree3@t@Config *config);

#ifdef VSG_HAVE_MPI
void vsg_nf_config3@t@_init (VsgNFConfig3@t@ *nfc,
                             VsgPRTree3@t@ *tree,
                             VsgPRTree3@t@FarInteractionFunc far_func,
                             VsgPRTree3@t@InteractionFunc near_func,
                             VsgPRTree3@t@SemifarInteractionFunc semifar_func,
                             guint semifar_threshold,
                             gpointer user_data);

void vsg_nf_config3@t@_tmp_alloc (VsgNFConfig3@t@ *nfc,
                                 VsgPRTree3@t@Config *config);

void vsg_nf_config3@t@_tmp_free (VsgNFConfig3@t@ *nfc,
                                 VsgPRTree3@t@Config *config);

#endif

void vsg_nf_config3@t@_clean (VsgNFConfig3@t@ *nfc);

gboolean
vsg_prtree3@t@_node_check_parallel_near_far (VsgNFConfig3@t@ *nfc,
                                             VsgPRTree3@t@Node *node,
                                             VsgPRTree3@t@NodeInfo *info,
                                             gboolean do_traversal);

gint vsg_prtree3@t@_nf_check_send (VsgNFConfig3@t@ *nfc);

gboolean vsg_prtree3@t@_nf_check_receive (VsgNFConfig3@t@ *nfc, gint tag,
                                          gboolean blocking);
void
vsg_prtree3@t@_nf_check_parallel_end (VsgNFConfig3@t@ *nfc);

void vsg_prtree3@t@_update_remote_depths (VsgPRTree3@t@ *tree);

void vsg_prtree3@t@node_recursive_near_func (VsgPRTree3@t@Node *one,
                                             VsgPRTree3@t@NodeInfo *one_info,
                                             VsgPRTree3@t@Node *other,
                                             VsgPRTree3@t@NodeInfo *other_info,
                                             VsgNFConfig3@t@ *nfc);

gboolean vsg_prtree3@t@_nf_isleaf_is_default (VsgPRTree3@t@ *tree);

G_END_DECLS;

#endif /* __VSGPRTREE3@T@_PRIVATE_H__ */
