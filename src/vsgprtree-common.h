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

#ifndef __VSG_PRTREE_COMMON_H__
#define __VSG_PRTREE_COMMON_H__

G_BEGIN_DECLS;

/**
 * VsgChildrenOrderDataFunc:
 * @key: input node identifier key.
 * @children: output ordered list of children numbers (must be at least of size=2**dim).
 * @children_keys: output ordered list of children identifier keys (must be at least of size=2**dim).
 * @user_data: user data.
 *
 * function type for modifying children ordering in VsgPRTree* traversal.
 * Each node is identified with a key. The function must provide
 * ordering and a list of corresponding identifier keys for the node's
 * children.
 *
 */
typedef void (*VsgChildrenOrderDataFunc) (gpointer key, gint *children,
                                          gpointer *children_keys,
                                          gpointer user_data);

/**
 * VsgChildrenOrderFunc:
 * @key: node identifier key.
 * @children: output ordered list of children numbers (must be at least of size=2**dim).
 * @children_keys: output ordered list of children identifier keys (must be at least of size=2**dim).
 *
 * function type for modifying children ordering in VsgPRTree* traversal.
 * Each node is identified with a key. The function must provide
 * ordering and a list of corresponding identifier keys for the node's
 * children.
 */
typedef void (*VsgChildrenOrderFunc) (gpointer key, gint *children,
                                      gpointer *children_keys);



G_END_DECLS;

#endif /* __VSG_PRTREE_COMMON_H__ */
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

#ifndef __VSG_PRTREE_COMMON_H__
#define __VSG_PRTREE_COMMON_H__

G_BEGIN_DECLS;

/**
 * VsgChildrenOrderDataFunc:
 * @key: input node identifier key.
 * @children: output ordered list of children numbers (must be at least of size=2**dim).
 * @children_keys: output ordered list of children identifier keys (must be at least of size=2**dim).
 * @user_data: user data.
 *
 * function type for modifying children ordering in VsgPRTree* traversal.
 * Each node is identified with a key. The function must provide
 * ordering and a list of corresponding identifier keys for the node's
 * children.
 *
 */
typedef void (*VsgChildrenOrderDataFunc) (gpointer key, gint *children,
                                          gpointer *children_keys,
                                          gpointer user_data);

/**
 * VsgChildrenOrderFunc:
 * @key: node identifier key.
 * @children: output ordered list of children numbers (must be at least of size=2**dim).
 * @children_keys: output ordered list of children identifier keys (must be at least of size=2**dim).
 *
 * function type for modifying children ordering in VsgPRTree* traversal.
 * Each node is identified with a key. The function must provide
 * ordering and a list of corresponding identifier keys for the node's
 * children.
 */
typedef void (*VsgChildrenOrderFunc) (gpointer key, gint *children,
                                      gpointer *children_keys);



G_END_DECLS;

#endif /* __VSG_PRTREE_COMMON_H__ */
