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

#include "vsgprtree3@t@.h"

G_BEGIN_DECLS;

/* binary localization macros: */
#define VSG_LOC3_COMP(loc,axis) ( \
 (vsgloc3) VSG_LOC3_MASK & ((loc) & (axis)) \
)

#define VSG_LOC3_ALONG_X(loc) ( VSG_LOC3_COMP (loc, VSG_LOC3_X) )
#define VSG_LOC3_ALONG_Y(loc) ( VSG_LOC3_COMP (loc, VSG_LOC3_Y) )
#define VSG_LOC3_ALONG_Z(loc) ( VSG_LOC3_COMP (loc, VSG_LOC3_Z) )

/* 
 * These macros are used to decide if we have to call a user defined marshaller
 * or directly the provided function. If we tried to write a dummy marshaller
 * for the provided function, we would loose performance on this case. Instead,
 * we rely on compiler efficiency in prefetching to minimize the overhead in
 * the direct case. The marshaller case may be slow down a little but it will
 * hopefully not be noticeable in its natural uses (language wrappers and
 * other special cases).
 */
#define CALL_POINT3@T@_LOC(config, one, other) ( \
(G_UNLIKELY ((config)->point_loc_marshall != NULL)) ? \
(config)->point_loc_marshall ((one), (other), (config)->point_loc_data) : \
((VsgPoint3@t@LocFunc) (config)->point_loc_data) ((one), (other)) \
)

#define CALL_REGION3@T@_LOC(config, one, other) ( \
(G_UNLIKELY ((config)->region_loc_marshall != NULL)) ? \
(config)->region_loc_marshall ((one), (other), (config)->region_loc_data) : \
((VsgRegion3@t@LocFunc) (config)->region_loc_data) ((one), (other)) \
)

#define CALL_POINT3@T@_DIST(config, one, other) ( \
(G_UNLIKELY ((config)->point_dist_marshall != NULL)) ? \
(config)->point_dist_marshall ((one), (other), (config)->point_dist_data) : \
((VsgPoint3@t@DistFunc) (config)->point_dist_data) ((one), (other)) \
)

/* forward typedefs */
typedef struct _VsgPRTree3@t@Leaf VsgPRTree3@t@Leaf;
typedef struct _VsgPRTree3@t@Int VsgPRTree3@t@Int;
typedef struct _VsgPRTree3@t@Node VsgPRTree3@t@Node;

typedef struct _VsgPRTree3@t@Config VsgPRTree3@t@Config;

/* private structs */
struct _VsgPRTree3@t@Leaf {

  gpointer isint;
  GSList *point;
};

struct _VsgPRTree3@t@Int {

  VsgPRTree3@t@Node *children[8];
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

struct _VsgPRTree3@t@Config {

  /* localization methods */
  VsgPoint3@t@LocMarshall point_loc_marshall;
  gpointer point_loc_data;

  VsgRegion3@t@LocMarshall region_loc_marshall;
  gpointer region_loc_data;

  /* point distance func */
  VsgPoint3@t@DistMarshall point_dist_marshall;
  gpointer point_dist_data;

  /* config */
  guint max_point;

  /* spatial tolerance */
  @type@ tolerance;

  /* user node data */
  gpointer user_data_model;
  GType user_data_type;

  /* children order in traversals */
  VsgChildrenOrderDataFunc children_order;
  gpointer children_order_data;
  gpointer root_key;
};

struct _VsgPRTree3@t@ {

  /* node part */
  VsgPRTree3@t@Node *node;

  /* tree configuration */
  VsgPRTree3@t@Config config;
};

/* private functions */

void _vsg_prtree3@t@node_get_info (VsgPRTree3@t@Node *node,
                                   VsgPRTree3@t@NodeInfo *node_info,
                                   VsgPRTree3@t@NodeInfo *father_info)

G_END_DECLS;

#endif /* __VSGPRTREE3@T@_PRIVATE_H__ */
