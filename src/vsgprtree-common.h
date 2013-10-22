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


/**
 * VsgParallelStorage:
 * @VSG_PARALLEL_SHARED_LOCAL: data is shared among several processors,
 * including current one.
 * @VSG_PARALLEL_SHARED_REMOTE: data is shared among several processors,
 * xcluding current one.
 * @VSG_PARALLEL_PRIVATE_LOCAL: data is local to current processor (the default in sequential programs).
 * @VSG_PARALLEL_PRIVATE_REMOTE: data is stored on some remote processor.
 *
 * Parallel storage of some data: shared between all processors, local
 * to only one processor or stored on some remote processor.
 *
 * In a non parallel program, data is supposed to be %VSG_PARALLEL_LOCAL.
 */
typedef enum {
  VSG_PARALLEL_PRIVATE_LOCAL = 0,
  VSG_PARALLEL_SHARED_MASK = 1 << 0,
  VSG_PARALLEL_SHARED_LOCAL = VSG_PARALLEL_SHARED_MASK,
  VSG_PARALLEL_REMOTE_MASK = 1 << 1,
  VSG_PARALLEL_PRIVATE_REMOTE = VSG_PARALLEL_REMOTE_MASK,
  VSG_PARALLEL_SHARED_REMOTE = VSG_PARALLEL_SHARED_MASK | VSG_PARALLEL_REMOTE_MASK,
} VsgParallelStorage;

/**
 * VsgParallelStatus:
 * @storage: type of storage.
 * @proc: processor number of data in case of a %VSG_PARALLEL_REMOTE
 * storage.
 *
 * Parallel status associated with some piece of data.
 */
typedef struct _VsgParallelStatus VsgParallelStatus;

struct _VsgParallelStatus {
  VsgParallelStorage storage:2;
  guint32 proc:30;
};

/**
 * vsg_parallel_status_private_local:
 *
 * #VsgParallelStatus constant representing private-local data.
 */
static const VsgParallelStatus vsg_parallel_status_private_local = {
  VSG_PARALLEL_PRIVATE_LOCAL,
  0,
};

/**
 * vsg_parallel_status_shared_local:
 *
 * #VsgParallelStatus constant representing shared-local data.
 */
static const VsgParallelStatus vsg_parallel_status_shared_local = {
  VSG_PARALLEL_SHARED_LOCAL,
  -1,
};

/**
 * VSG_PARALLEL_STATUS_IS_REMOTE:
 * @status: a #VsgParallelStatus
 *
 * Convenience macro for questioning about @status storage type.
 *
 * Returns: #TRUE if @status is equal to #VSG_PARALLEL_REMOTE
 */
#define VSG_PARALLEL_STORAGE_IS_REMOTE(storage) ( \
(storage) & VSG_PARALLEL_REMOTE_MASK \
)

#define VSG_PARALLEL_STORAGE_IS_LOCAL(storage) ( \
! VSG_PARALLEL_STORAGE_IS_REMOTE (storage) \
)

#define VSG_PARALLEL_STORAGE_IS_SHARED(storage) ( \
(storage) & VSG_PARALLEL_SHARED_MASK \
)
#define VSG_PARALLEL_STORAGE_IS_PRIVATE(storage) ( \
! VSG_PARALLEL_STORAGE_IS_SHARED (storage) \
)

#define VSG_PARALLEL_STORAGE_IS_PRIVATE_LOCAL(storage) ( \
(storage) == VSG_PARALLEL_PRIVATE_LOCAL \
)

#define VSG_PARALLEL_STORAGE_IS_SHARED_LOCAL(storage) ( \
(storage) == VSG_PARALLEL_SHARED_LOCAL \
)

#define VSG_PARALLEL_STORAGE_IS_PRIVATE_REMOTE(storage) ( \
(storage) == VSG_PARALLEL_PRIVATE_REMOTE \
)

#define VSG_PARALLEL_STORAGE_IS_SHARED_REMOTE(storage) ( \
(storage) == VSG_PARALLEL_SHARED_REMOTE \
)

/**
 * VSG_PARALLEL_STATUS_IS_LOCAL:
 * @status: a #VsgParallelStatus
 *
 * Convenience macro for questioning about @status storage type.
 *
 * Returns: #TRUE if @status is equal to #VSG_PARALLEL_SHARED_LOCAL or
 * #VSG_PARALLEL_PRIVATE_LOCAL.
 */
#define VSG_PARALLEL_STATUS_IS_LOCAL(status) ( \
VSG_PARALLEL_STORAGE_IS_LOCAL ((status).storage) \
)

/**
 * VSG_PARALLEL_STATUS_IS_REMOTE:
 * @status: a #VsgParallelStatus
 *
 * Convenience macro for questioning about @status storage type.
 *
 * Returns: #TRUE if @status is equal to #VSG_PARALLEL_SHARED_REMOTE or
 * #VSG_PARALLEL_PRIVATE_REMOTE.
 */
#define VSG_PARALLEL_STATUS_IS_REMOTE(status) ( \
VSG_PARALLEL_STORAGE_IS_REMOTE ((status).storage) \
)

/**
 * VSG_PARALLEL_STATUS_IS_SHARED:
 * @status: a #VsgParallelStatus
 *
 * Convenience macro for questioning about @status storage type.
 *
 * Returns: #TRUE if @status is equal to #VSG_PARALLEL_SHARED
 */
#define VSG_PARALLEL_STATUS_IS_SHARED(status) ( \
VSG_PARALLEL_STORAGE_IS_SHARED ((status).storage) \
)
#define VSG_PARALLEL_STATUS_IS_PRIVATE(status) ( \
VSG_PARALLEL_STORAGE_IS_PRIVATE ((status).storage) \
)

#define VSG_PARALLEL_STATUS_IS_PRIVATE_LOCAL(status) ( \
VSG_PARALLEL_STORAGE_IS_PRIVATE_LOCAL ((status).storage) \
)

#define VSG_PARALLEL_STATUS_IS_SHARED_LOCAL(status) ( \
VSG_PARALLEL_STORAGE_IS_SHARED_LOCAL ((status).storage) \
)

#define VSG_PARALLEL_STATUS_IS_PRIVATE_REMOTE(status) ( \
VSG_PARALLEL_STORAGE_IS_PRIVATE_REMOTE ((status).storage) \
)

#define VSG_PARALLEL_STATUS_IS_SHARED_REMOTE(status) ( \
VSG_PARALLEL_STORAGE_IS_SHARED_REMOTE ((status).storage) \
)

/**
 * VSG_PARALLEL_STATUS_PROC:
 * @status: a #VsgParallelStatus
 *
 * Convenience macro for questioning about @status processor number.
 *
 * Returns: @status.proc
 */
#define VSG_PARALLEL_STATUS_PROC(status) ( \
(status).proc \
)

G_END_DECLS;


#endif /* __VSG_PRTREE_COMMON_H__ */
