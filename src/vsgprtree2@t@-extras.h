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

#ifndef __VSGPRTREE2@T@_EXTRAS_H__
#define __VSGPRTREE2@T@_EXTRAS_H__

#include "vsgprtree2@t@.h"

G_BEGIN_DECLS;

/* forward typedefs */
typedef void (*VsgPRTree2@t@InteractionFunc) (VsgPRTree2@t@NodeInfo *one_info,
					      VsgPRTree2@t@NodeInfo *other_info,
					      gpointer user_data);

typedef void (*VsgPRTree2@t@FarInteractionFunc) (VsgPRTree2@t@NodeInfo *one_info,
                                                 VsgPRTree2@t@NodeInfo *other_info,
                                                 gpointer user_data);

/* public functions */
void
vsg_prtree2@t@_near_far_traversal (VsgPRTree2@t@ *prtree2@t@,
                                   VsgPRTree2@t@FarInteractionFunc far_func,
                                   VsgPRTree2@t@InteractionFunc near_func,
                                   gpointer user_data);

void vsg_prtree2@t@_set_nf_isleaf (VsgPRTree2@t@ *prtree2@t@,
                                   VsgPRTree2@t@NFIsleafFunc isleaf,
                                   gpointer user_data);

void vsg_prtree2@t@_set_children_order_hilbert (VsgPRTree2@t@ *prtree2@t@);
void vsg_prtree2@t@_set_children_order_default (VsgPRTree2@t@ *prtree2@t@);

G_END_DECLS;

#endif /* __VSGPRTREE2@T@_EXTRAS_H__ */
