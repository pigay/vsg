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

#include "vsgprtree3@t@.h"

#include "vsgprtree3@t@-private.h"

#include <string.h>
#include <glib/gprintf.h>

#define PRTREE3@T@LEAF_MAXSIZE 5

#define _USE_G_SLICES GLIB_CHECK_VERSION (2, 10, 0)

#if ! _USE_G_SLICES

#define VSG_PRTREE3@T@_PREALLOC 32
#define PRTREE3@T@NODE_PREALLOC 256

/* Nodes allocation */

static GMemChunk *vsg_prtree3@t@_mem_chunk = 0;
static GMemChunk *vsg_prtree3@t@node_mem_chunk = 0;
static guint vsg_prtree3@t@_instances_count = 0;

static void _prtree3@t@_finalize ()
{
  if (vsg_prtree3@t@_mem_chunk)
    {
      g_mem_chunk_destroy (vsg_prtree3@t@_mem_chunk);
      vsg_prtree3@t@_mem_chunk = 0;
      vsg_prtree3@t@_instances_count = 0;
    }
  if (vsg_prtree3@t@node_mem_chunk)
    {
      g_mem_chunk_destroy (vsg_prtree3@t@node_mem_chunk);
      vsg_prtree3@t@node_mem_chunk = 0;
    }
}
#endif /* ! _USE_G_SLICES */

/* keep in sync with vsgprtreekey3@t@.c */
void vsgprtree_key3@t@_init ();

void vsg_prtree3@t@_init()
{
  vsgprtree_key3@t@_init ();
#if ! _USE_G_SLICES
  g_atexit (_prtree3@t@_finalize);
#endif
}

static VsgPRTree3@t@ *_prtree3@t@_alloc ()
{
  VsgPRTree3@t@ *ret;

#if _USE_G_SLICES
  ret = g_slice_new0 (VsgPRTree3@t@);
#else
  if (!vsg_prtree3@t@_mem_chunk)
    {
      vsg_prtree3@t@_mem_chunk = g_mem_chunk_create (VsgPRTree3@t@,
                                                     VSG_PRTREE3@T@_PREALLOC,
                                                     G_ALLOC_ONLY);

      if (!vsg_prtree3@t@node_mem_chunk)
        vsg_prtree3@t@node_mem_chunk =
          g_mem_chunk_create (VsgPRTree3@t@Node,
                              PRTREE3@T@NODE_PREALLOC,
                              G_ALLOC_ONLY);
    }

  vsg_prtree3@t@_instances_count ++;

  ret = g_chunk_new0 (VsgPRTree3@t@, vsg_prtree3@t@_mem_chunk);
#endif /* ! _USE_G_SLICES */

#ifdef VSG_HAVE_MPI
  ret->config.parallel_config.communicator = MPI_COMM_NULL;
#endif

  ret->pending_shared_regions = NULL;
  ret->pending_exterior_points = NULL;

  return ret;
}

static void _prtree3@t@_dealloc (VsgPRTree3@t@ *prtree3@t@)
{
  VsgParallelVTable vtable = {0,};

  /* set */
  vsg_prtree3@t@_set_node_data_vtable (prtree3@t@, &vtable);

  if (prtree3@t@->pending_shared_regions != NULL)
    g_slist_free (prtree3@t@->pending_shared_regions);

  if (prtree3@t@->pending_exterior_points != NULL)
    g_slist_free (prtree3@t@->pending_exterior_points);

#if _USE_G_SLICES
  g_slice_free (VsgPRTree3@t@, prtree3@t@);
#else
  g_chunk_free (prtree3@t@, vsg_prtree3@t@_mem_chunk);
  vsg_prtree3@t@_instances_count --;

  if (vsg_prtree3@t@_instances_count == 0)
    {
      _prtree3@t@_finalize ();
    }
#endif /* ! _USE_G_SLICES */
}


VsgPRTree3@t@Node *
vsg_prtree3@t@node_alloc_no_data (const VsgVector3@t@ *lbound,
                                  const VsgVector3@t@ *ubound)
{
  VsgPRTree3@t@Node *ret;

#if _USE_G_SLICES
  ret = g_slice_new (VsgPRTree3@t@Node);
#else
  ret = g_chunk_new (VsgPRTree3@t@Node, vsg_prtree3@t@node_mem_chunk);
#endif

  ret->region_list = NULL;

  ret->point_count = 0;
  ret->region_count = 0;

  vsg_vector3@t@_copy (lbound, &ret->lbound);
  vsg_vector3@t@_copy (ubound, &ret->ubound);

  vsg_vector3@t@_lerp (lbound, ubound, 0.5, &ret->center);

  ret->variable.leaf.isint = FALSE; /* default is an empty leaf */
  ret->variable.leaf.point = NULL;

  ret->user_data = NULL;

  ret->parallel_status.storage = VSG_PARALLEL_LOCAL;
  ret->parallel_status.proc = 0;

  return ret;
}

static void _node_alloc_data (VsgPRTree3@t@Node *node,
                              const VsgPRTreeParallelConfig *pc)
{
  if (pc->node_data.alloc != NULL)
    node->user_data = pc->node_data.alloc (TRUE, pc->node_data.alloc_data);

}

VsgPRTree3@t@Node *vsg_prtree3@t@node_alloc (const VsgVector3@t@ *lbound,
                                             const VsgVector3@t@ *ubound,
                                             const VsgPRTree3@t@Config *config)
{
  VsgPRTree3@t@Node *ret = vsg_prtree3@t@node_alloc_no_data (lbound, ubound);

  _node_alloc_data (ret, &config->parallel_config);

  return ret;
}

void vsg_prtree3@t@node_dealloc (VsgPRTree3@t@Node *prtree3@t@node)
{

#if _USE_G_SLICES
  g_slice_free (VsgPRTree3@t@Node, prtree3@t@node);
#else
  g_chunk_free (prtree3@t@node, vsg_prtree3@t@node_mem_chunk);
#endif
}

static VsgPRTree3@t@Node *_leaf_alloc (const VsgVector3@t@ *lbound,
                                       const VsgVector3@t@ *ubound,
                                       const VsgParallelStatus parallel_status,
                                       const VsgPRTree3@t@Config *config)
{
  VsgPRTree3@t@Node *node = vsg_prtree3@t@node_alloc_no_data (lbound, ubound);

#ifdef VSG_HAVE_MPI
  if (!VSG_PARALLEL_STATUS_IS_REMOTE (parallel_status))
#endif
    _node_alloc_data (node, &config->parallel_config);

  node->variable.isint = 0;

  PRTREE3@T@NODE_LEAF(node).point = 0;

  node->parallel_status = parallel_status;

  return node;
}

static void _prtree3@t@node_child_get_bounds (const VsgPRTree3@t@Node *node,
                                              vsgloc3 loc,
                                              VsgVector3@t@ *lbound,
                                              VsgVector3@t@ *ubound)
{
  vsgloc3 xcomp = VSG_LOC3_ALONG_X (loc);
  vsgloc3 ycomp = VSG_LOC3_ALONG_Y (loc);
  vsgloc3 zcomp = VSG_LOC3_ALONG_Z (loc);

  const VsgVector3@t@ *center = &node->center;

  if (xcomp == 0)
    {
      lbound->x = node->lbound.x;
      ubound->x = center->x;
    }
  else
    {
      lbound->x = center->x;
      ubound->x = node->ubound.x;
    }

  if (ycomp == 0)
    {
      lbound->y = node->lbound.y;
      ubound->y = center->y;
    }
  else
    {
      lbound->y = center->y;
      ubound->y = node->ubound.y;
    }

  if (zcomp == 0)
    {
      lbound->z = node->lbound.z;
      ubound->z = center->z;
    }
  else
    {
      lbound->z = center->z;
      ubound->z = node->ubound.z;
    }
}

static VsgPRTree3@t@Node *_int_alloc (const VsgVector3@t@ *lbound,
                                      const VsgVector3@t@ *ubound,
                                      VsgPRTree3@t@Node *child,
                                      vsgloc3 loc,
                                      const VsgPRTree3@t@Config *config)
{
  VsgPRTree3@t@Node *node = vsg_prtree3@t@node_alloc (lbound, ubound, config);
  vsgloc3 i;
  VsgPRTree3@t@Node *children[8];

  for (i=0; i<8; i++)
    {
      VsgVector3@t@ lbound, ubound;

      if ((child == NULL) || (i != loc))
        {
          _prtree3@t@node_child_get_bounds (node, i, &lbound, &ubound);
          children[i] = _leaf_alloc (&lbound, &ubound,
                                     vsg_parallel_status_local,
                                     config);
        }
      else
        {
          children[i] = child;

          node->point_count += child->point_count;
          node->region_count += child->region_count;
        }
    }

  if (! PRTREE3@T@NODE_IS_LOCAL (child))
    {
      node->parallel_status = vsg_parallel_status_shared;
    }

  /* It is very important that new leaves are inserted once all calls to
   * _leaf_alloc are done, leaving the tree state consistent, because
   * certain language bindings do weird things with the tree when node_data
   * are duplicated. For example, Python and its cyclic garbage collector.
   */
  memcpy (PRTREE3@T@NODE_INT (node).children, children,
          8*sizeof (VsgPRTree3@t@Node*));

  return node;
}

static guint _prtree3@t@node_point_count (const VsgPRTree3@t@Node *node)
{
  /*   guint result = 0; */
  /*   if (PRTREE3@T@NODE_ISLEAF(node)) */
  /*     { */
  /*       result = g_slist_length (PRTREE3@T@NODE_LEAF(node).point); */
  /*     } */
  /*   else */
  /*     { */
  /*       vsgloc3 i; */

  /*       for (i=0; i<8; i++) */
  /*         result += _prtree3@t@node_point_count (PRTREE3@T@NODE_CHILD (node, i)); */
  /*     } */

  /*   return result; */

  return node->point_count;
}

static guint _prtree3@t@node_region_count (const VsgPRTree3@t@Node *node)
{
  /*   guint result = 0; */
  /*   if (PRTREE3@T@NODE_ISINT(node)) */
  /*     { */
  /*       vsgloc3 i; */

  /*       for (i=0; i<8; i++) */
  /*         result += _prtree3@t@node_region_count (PRTREE3@T@NODE_CHILD (node, i)); */
  /*     } */

  /*   result += g_slist_length (node->region_list); */

  /*   return result; */

  return node->region_count;
}


static GSList *_prtree3@t@node_steal_point (VsgPRTree3@t@Node *node)
{
  GSList *result = NULL;

  if (PRTREE3@T@NODE_ISLEAF(node))
    {
      VsgPRTree3@t@Leaf *leaf_ = &PRTREE3@T@NODE_LEAF(node);

      result = leaf_->point;
      leaf_->point = NULL;
      node->point_count = 0;
    }
  else
    {
      vsgloc3 i;

      for (i=0; i<8; i++)
        {
          GSList *stolen;

          node->point_count -= PRTREE3@T@NODE_CHILD (node, i)->point_count;

          stolen =
            _prtree3@t@node_steal_point (PRTREE3@T@NODE_CHILD (node, i));

          result = g_slist_concat (result, stolen);
        }
    }

  return result;
}

static GSList *_prtree3@t@node_steal_region (VsgPRTree3@t@Node *node)
{
  GSList *result = NULL;

  if (PRTREE3@T@NODE_ISLEAF(node))
    {
      result = node->region_list;
      node->region_list = NULL;
      node->region_count = 0;
    }
  else
    {
      vsgloc3 i;

      for (i=0; i<8; i++)
        {
          GSList *stolen;

          node->region_count -= PRTREE3@T@NODE_CHILD (node, i)->region_count;

          stolen =
            _prtree3@t@node_steal_region (PRTREE3@T@NODE_CHILD (node, i));

          result = g_slist_concat (result, stolen);
        }
    }

  return result;
}

static guint
_prtree3@t@node_insert_region_list (VsgPRTree3@t@Node *node,
                                    GSList *region_list,
                                    const VsgPRTree3@t@Config *config,
                                    GSList **shared_regions);

static gboolean _check_point_dist (VsgPRTree3@t@Leaf *node,
                                   GSList *point,
                                   const VsgPRTree3@t@Config *config)
{
  /*
   * We check the distance from inserted point to first of already stored
   * point. If the distance is zero, we may have problems in the future,
   * eventually leading to infinite recursion on leaf subdivision. We then
   * choose to abort insertion.
   */

  if (CALL_POINT3@T@_DIST (config,
                           (VsgPoint3) point->data,
                           (VsgPoint3) node->point->data) <=
      config->tolerance)
    {
      g_warning ("detected multiple insertion of a same Point3."
                 " Aborting insertion.\n");
      g_slist_free (point);
      return FALSE;
    }

  return TRUE;
}

static guint
_prtree3@t@leaf_insert_point_list(VsgPRTree3@t@Node *node,
                                  GSList *point,
                                  const VsgPRTree3@t@Config *config)
{
  g_assert (point != NULL);
  g_assert (point->next == NULL);

  if (node->point_count+1 > config->max_point)
    {
      if (_check_point_dist (&PRTREE3@T@NODE_LEAF (node),
                             point, config))
        {
          vsg_prtree3@t@node_make_int (node, config);
          vsg_prtree3@t@node_insert_point_list (node, point, config);

        }
      else
        return 0;
    }
  else
    {
      PRTREE3@T@NODE_LEAF (node).point =
        g_slist_concat (point, PRTREE3@T@NODE_LEAF (node).point);
      node->point_count ++;
    }

  return 1;
}

guint
vsg_prtree3@t@node_insert_point_list(VsgPRTree3@t@Node *node,
                                     GSList *point,
                                     const VsgPRTree3@t@Config *config)
{
  guint len = 0;

#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node))
    {
      /* store outgoing points into remote nodes as if they they were a leaf */
      PRTREE3@T@NODE_LEAF (node).point =
        g_slist_concat (point, PRTREE3@T@NODE_LEAF (node).point);

      return 0;
    }
#endif

  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      while (point)
        {
          GSList *current;

          current = point;
          point = g_slist_next (current);
          current->next = NULL;

          len += _prtree3@t@leaf_insert_point_list (node, current, config);
        }
    }
  else
    {
      while (point)
        {
          GSList *current;
          vsgloc3 i;

          current = point;
          point = g_slist_next (current);
          current->next = NULL;

          i = CALL_POINT3@T@_LOC (config, current->data, &node->center);

          len +=
            vsg_prtree3@t@node_insert_point_list(PRTREE3@T@NODE_CHILD (node, i),
                                                 current, config);
        }

      node->point_count += len;
    }

  return len;
}

static void _prtree3@t@node_insert_point(VsgPRTree3@t@Node *node,
                                         VsgPoint3 point,
                                         const VsgPRTree3@t@Config *config)
{
  GSList *point_list = g_slist_alloc();

  point_list->data = (gpointer) point;

  vsg_prtree3@t@node_insert_point_list(node, point_list, config);
}

static gint
_prtree3@t@node_insert_point_local (VsgPRTree3@t@Node *node,
                                    VsgPoint3 point,
                                    const VsgPRTree3@t@Config *config)
{
  gint len = 0;

#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node))
    {
      return -1;
    }
#endif

  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      GSList *point_list = g_slist_alloc();

      point_list->data = (gpointer) point;

      len += _prtree3@t@leaf_insert_point_list (node, point_list, config);
    }
  else
    {
      vsgloc3 i;

      i = CALL_POINT3@T@_LOC (config, point, &node->center);

      len += _prtree3@t@node_insert_point_local (PRTREE3@T@NODE_CHILD (node, i),
                                                 point, config);
      if (len > 0) node->point_count += len;
    }

  return len;
}

void vsg_prtree3@t@node_make_int (VsgPRTree3@t@Node *node,
                                  const VsgPRTree3@t@Config *config)
{
  GSList *stolen_point;
  GSList *stolen_region;
  vsgloc3 i;
  VsgPRTree3@t@Node *children[8];

  g_return_if_fail (PRTREE3@T@NODE_ISLEAF(node));

  stolen_point = _prtree3@t@node_steal_point (node);
  stolen_region = _prtree3@t@node_steal_region (node);

  for (i=0; i<8; i++)
    {
      VsgVector3@t@ lbound, ubound;

      _prtree3@t@node_child_get_bounds (node, i, &lbound, &ubound);

      children[i] = _leaf_alloc (&lbound, &ubound, node->parallel_status,
                                 config);

      children[i]->parallel_status = node->parallel_status;
    }

  /* It is very important that new leaves are inserted once all calls to
   * _leaf_alloc are done, leaving the tree state consistent, because
   * certain language bindings do weird things with the tree when node_data
   * are duplicated. For example, Python and its cyclic garbage collector.
   */
  memcpy (PRTREE3@T@NODE_INT (node).children, children,
          8*sizeof (VsgPRTree3@t@Node*));

  vsg_prtree3@t@node_insert_point_list(node, stolen_point, config);
  _prtree3@t@node_insert_region_list(node, stolen_region, config, NULL);
      
}

void vsg_prtree3@t@node_free (VsgPRTree3@t@Node *node,
                              const VsgPRTree3@t@Config *config)
{
  if (PRTREE3@T@NODE_ISLEAF (node) ||
      PRTREE3@T@NODE_IS_REMOTE (node))
    {
      g_slist_free (PRTREE3@T@NODE_LEAF (node).point);

    }
  else
    {
      vsgloc3 i;

      for (i=0; i<8; i++)
        vsg_prtree3@t@node_free (PRTREE3@T@NODE_CHILD (node, i), config);
    }

  g_slist_free (node->region_list);

  if (config->parallel_config.node_data.destroy != NULL &&
      ! PRTREE3@T@NODE_IS_REMOTE (node))
    config->parallel_config.node_data.destroy (node->user_data, TRUE,
                                               config->parallel_config.node_data.destroy_data);

  vsg_prtree3@t@node_dealloc (node);
}

static guint _prtree3@t@node_depth (const VsgPRTree3@t@Node *node)
{
  guint res = 0;
  vsgloc3 i;

  if (PRTREE3@T@NODE_ISLEAF (node) ||
      PRTREE3@T@NODE_IS_REMOTE (node)) return 0;

  for (i=0; i<8; i++)
    {
      guint tmp = _prtree3@t@node_depth (PRTREE3@T@NODE_CHILD (node, i));
      if (tmp > res) res = tmp;
    }

  return res + 1;
}

static void _prtree3@t@node_flatten (VsgPRTree3@t@Node *node,
                                     const VsgPRTree3@t@Config *config)
{
  GSList *point = NULL;
  GSList *region = NULL;
  vsgloc3 i;

  g_return_if_fail (PRTREE3@T@NODE_ISINT (node));

  point = _prtree3@t@node_steal_point (node);
  region = _prtree3@t@node_steal_region (node);

  for (i=0; i<8; i++)
    {
      vsg_prtree3@t@node_free (PRTREE3@T@NODE_CHILD (node, i), config);
      PRTREE3@T@NODE_CHILD (node, i) = NULL;
    }

  vsg_prtree3@t@node_insert_point_list (node, point, config);
  _prtree3@t@node_insert_region_list (node, region, config, NULL);
}

static gboolean
_prtree3@t@node_remove_point (VsgPRTree3@t@Node *node,
                              VsgPoint3 point,
                              const VsgPRTree3@t@Config *config)
{
  gboolean ret = FALSE;

#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node))
    {
      /* unable to remove a node located on another processor */
      return FALSE;
    }
#endif

  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      ret = g_slist_find (PRTREE3@T@NODE_LEAF (node).point,
                          point) != NULL;
      if (ret)
        {
          PRTREE3@T@NODE_LEAF (node).point =
            g_slist_remove (PRTREE3@T@NODE_LEAF (node).point, point);
          node->point_count --;
        }
    }
  else
    {
      vsgloc3 i = CALL_POINT3@T@_LOC (config, point, &node->center);

      ret = _prtree3@t@node_remove_point (PRTREE3@T@NODE_CHILD (node, i),
                                          point, config);

      if (ret)
        {
          node->point_count --;

          /* flatten only if node is local. Shared nodes mean heterogenously
           * distributed children.
           */
          if (node->point_count <= config->max_point &&
              PRTREE3@T@NODE_IS_LOCAL (node))
            _prtree3@t@node_flatten (node, config);
        }
    }

  return ret;
}

static void _wtabs (FILE *file, guint tab)
{
  guint i;

  for (i=0; i<tab; i++)
    {
      fprintf(file, "  ");
    }
}

static void _prtree3@t@node_write (VsgPRTree3@t@Node *node,
                                   VsgPRTree3@t@NodeInfo *node_info,
                                   FILE *file)
{
  _wtabs (file, 2*node_info->depth);

#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node))
    {
      fprintf (file,
               "remote%d[(%@tcode@,%@tcode@,%@tcode@) (%@tcode@,%@tcode@,%@tcode@) id=(",
               node->parallel_status.proc,
               node->lbound.x, node->lbound.y, node->lbound.z,
               node->ubound.x, node->ubound.y, node->ubound.z);
      vsg_prtree_key3@t@_write (&node_info->id, file);
      fprintf (file, ")] point=%d region=%d remote_depth=%u\n",
               node->point_count,
               node->region_count,
               PRTREE3@T@NODE_LEAF (node).remote_depth);

      return;
    }
#endif

  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      fprintf (file,
               "leaf[(%@tcode@,%@tcode@,%@tcode@) (%@tcode@,%@tcode@,%@tcode@) "
               "id=(",
               node->lbound.x, node->lbound.y, node->lbound.z,
               node->ubound.x, node->ubound.y, node->ubound.z);
      vsg_prtree_key3@t@_write (&node_info->id, file);
      fprintf (file, ")] point=%d region=%d\n",
               node->point_count,
               node->region_count);
    }
  else
    {
      fprintf (file,
               "int[(%@tcode@,%@tcode@,%@tcode@) (%@tcode@,%@tcode@,%@tcode@) "
               "id=(",
               node->lbound.x, node->lbound.y, node->lbound.z,
               node->ubound.x, node->ubound.y, node->ubound.z);
      vsg_prtree_key3@t@_write (&node_info->id, file);
      fprintf (file, ")] point=%d region=%d local_region=%d depth=%u\n",
               node->point_count,
               node->region_count,
               g_slist_length (node->region_list),
               _prtree3@t@node_depth (node));
    }
}

static VsgPoint3
_prtree3@t@node_find_point (VsgPRTree3@t@Node *node,
                            VsgPoint3 selector,
                            const VsgPRTree3@t@Config *config)
{
#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node))
    {
      return NULL;
    }
#endif

  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      GSList *point = PRTREE3@T@NODE_LEAF (node).point;

      while (point)
        {
          if (CALL_POINT3@T@_DIST (config, selector,
                                   (VsgPoint3) point->data) <=
              config->tolerance)
            {
              return point->data;
            }
          point = g_slist_next (point);
        }
    }
  else
    {
      vsgloc3 i = CALL_POINT3@T@_LOC (config, selector, &node->center);

      return
        _prtree3@t@node_find_point (PRTREE3@T@NODE_CHILD (node, i),
                                    selector, config);
    }
  return NULL;
}

static vsgrloc3 _region3_point3_locfunc_always (const gpointer region,
                                                const VsgPoint3 point)
{
  return VSG_RLOC3_MASK;
}

static gboolean _share_region (vsgrloc3 locmask)
{
  vsgloc3 i;
  vsgrloc3 ipow;
  guint count = 0;

  for (i=0; i<8; i++)
    {
      ipow = VSG_RLOC3_COMP (i);
      if (ipow & locmask)
        count++;
    }

  return count != 1;
}

static guint
_prtree3@t@node_insert_region_list (VsgPRTree3@t@Node *node,
                                    GSList *region_list,
                                    const VsgPRTree3@t@Config *config,
                                    GSList **shared_regions)
{
  guint len = g_slist_length (region_list);

#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node))
    {
      /* store outgoing regions into remote nodes. */
      node->region_list = g_slist_concat (node->region_list, region_list);

      return 0;
    }
#endif

  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      node->region_list = g_slist_concat (region_list, node->region_list);
    }
  else
    {
      while (region_list)
        {
          GSList *current;
          vsgrloc3 locmask;

          current = region_list;
          region_list = g_slist_next (current);
          current->next = NULL;

          locmask = CALL_REGION3@T@_LOC (config, current->data, &node->center);

          if (_share_region(locmask))
            {
              node->region_list = 
                g_slist_concat (current, node->region_list);

#ifdef VSG_HAVE_MPI
              /* shared regions in shared nodes are stored to be notified
               * to *all* the processors.
               */
              if (shared_regions != NULL && PRTREE3@T@NODE_IS_SHARED (node))
                *shared_regions = g_slist_concat (g_slist_copy (current),
                                                  *shared_regions);
#endif
            }
          else
            {
              vsgloc3 i;
              vsgrloc3 ipow;
              
              for (i=0; i<8; i++)
                {
                  ipow =  VSG_RLOC3_COMP (i);
                  if (locmask & ipow)
                    _prtree3@t@node_insert_region_list (PRTREE3@T@NODE_CHILD (node, i),
                                                        current,
                                                        config,
                                                        shared_regions);
                }
            }
        }
    }

  node->region_count += len;
  return len;
}

static void
_prtree3@t@node_insert_region (VsgPRTree3@t@Node *node,
                               VsgRegion3 region,
                               const VsgPRTree3@t@Config *config,
                               GSList **shared_regions)
{
  GSList *region_list = g_slist_alloc ();

  region_list->data = (gpointer) region;
  region_list->next = NULL;

  _prtree3@t@node_insert_region_list (node, region_list, config,
                                      shared_regions);
}

static gboolean
_prtree3@t@node_remove_region (VsgPRTree3@t@Node *node,
                               VsgRegion3 region,
                               const VsgPRTree3@t@Config *config)
{
  gboolean ret = FALSE;

#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node))
    {
      /* unable to remove a region on a remote processor */
      return FALSE;
    }
#endif

  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      ret = g_slist_find (node->region_list, region) != NULL;
      if (ret)
        node->region_list = g_slist_remove (node->region_list, region);
    }
  else
    {
      vsgrloc3 locmask = CALL_REGION3@T@_LOC (config, region, &node->center);

      if (_share_region(locmask))
        {
          ret = g_slist_find (node->region_list, region) != NULL;

          if (ret)
            node->region_list = g_slist_remove (node->region_list, region);
        }
      else
        {
          vsgloc3 i;
          vsgrloc3 ipow;
              
          for (i=0; i<8; i++)
            {
              ipow = VSG_RLOC3_COMP (i);
              if (locmask & ipow)
                {
                  ret =
                    _prtree3@t@node_remove_region (PRTREE3@T@NODE_CHILD (node, i),
                                                   region,
                                                   config);
                  break;
                }
            }
        }
    }

  if (ret) node->region_count --;
  return ret;
}

static VsgRegion3
_prtree3@t@node_find_deep_region (VsgPRTree3@t@Node *node,
                                  VsgPoint3 point,
                                  VsgRegion3CheckFunc check,
                                  gpointer user_data,
                                  const VsgPRTree3@t@Config *config)
{
  VsgRegion3 result = NULL;

#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node))
    return NULL;
#endif

  if (PRTREE3@T@NODE_ISINT (node))
    {
      vsgloc3 i = 0;

      if (point)
        i = CALL_POINT3@T@_LOC (config, point, &node->center);

      result =
        _prtree3@t@node_find_deep_region (PRTREE3@T@NODE_CHILD (node, i),
                                          point, check, user_data,
                                          config);
    }

  if (result == NULL)
    {
      GSList *region_list = node->region_list;

      while (region_list)
        {
          if (check (region_list->data, user_data))
            {
              return region_list->data;
            }

          region_list = g_slist_next (region_list);
        }
    }

  return result;
}

static void
_prtree3@t@node_update_user_data_vtable (VsgPRTree3@t@Node *node,
                                         VsgParallelVTable *old_vtable,
                                         VsgParallelVTable *new_vtable)
{
  if (old_vtable->destroy != NULL && node->user_data != NULL)
    old_vtable->destroy (node->user_data, TRUE, old_vtable->destroy_data);

  if (new_vtable->alloc != NULL && ! PRTREE3@T@NODE_IS_REMOTE (node))
    node->user_data = new_vtable->alloc (TRUE, new_vtable->alloc_data);
  else
    node->user_data = NULL;


  if (PRTREE3@T@NODE_ISINT (node))
    {
      vsgloc3 i;

      for (i=0; i<8; i++)
        {
          _prtree3@t@node_update_user_data_vtable
            (PRTREE3@T@NODE_CHILD (node, i), old_vtable, new_vtable);
        }

    }
}

static void _copy_point (VsgPoint3 *point, VsgPRTree3@t@ *tree)
{
  vsg_prtree3@t@_insert_point (tree, point);
}

static void _copy_region (VsgRegion3 *region, VsgPRTree3@t@ *tree)
{
  vsg_prtree3@t@_insert_region (tree, region);
}

static void
_prtree3@t@node_traverse_custom_internal (VsgPRTree3@t@Node *node,
                                          VsgPRTree3@t@NodeInfo *father_info,
                                          vsgloc3 child_number,
                                          GTraverseType order,
                                          VsgRegion3@t@InternalLocDataFunc sel_func,
                                          VsgRegion3 selector,
                                          gpointer sel_data,
                                          VsgPRTree3@t@InternalFunc func,
                                          gpointer user_data,
                                          VsgPRTree3@t@Config *config,
                                          gpointer node_key)
{
  VsgPRTree3@t@NodeInfo node_info;
  guint8 i;
  vsgrloc3 ipow;
  vsgrloc3 locmask = VSG_RLOC3_MASK;

  gint children[8];
  gpointer children_keys[8];

  _vsg_prtree3@t@node_get_info (node, &node_info, father_info, child_number);

  if (sel_func != NULL) locmask = sel_func (selector, &node_info, sel_data);

#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node))
    {
      func (node, &node_info, user_data);
      return;
    }
#endif

  if (order == G_PRE_ORDER)
    func (node, &node_info, user_data);

  if (PRTREE3@T@NODE_ISINT (node))
    {
      config->children_order (node_key, children, children_keys,
                              config->children_order_data);

      for (i=0; i<4; i++)
	{
          gint ic = children[i];

          ipow = VSG_RLOC3_COMP (ic);

          if (ipow & locmask)
            _prtree3@t@node_traverse_custom_internal
              (PRTREE3@T@NODE_CHILD(node, ic), &node_info, ic,
               order, sel_func, selector, sel_data, func, user_data, config,
               children_keys[i]);
	}
    }

  if (order == G_IN_ORDER)
    func (node, &node_info, user_data);

  if (PRTREE3@T@NODE_ISINT (node))
    {
      for (i=4; i<8; i++)
	{
          gint ic = children[i];

          ipow = VSG_RLOC3_COMP (ic);

          if (ipow & locmask)
            _prtree3@t@node_traverse_custom_internal
              (PRTREE3@T@NODE_CHILD(node, ic), &node_info, ic,
               order, sel_func, selector, sel_data, func, user_data, config,
               children_keys[i]);
	}
    }

  if (order == G_POST_ORDER)
    func (node, &node_info, user_data);
}

static void
_prtree3@t@node_traverse_custom (VsgPRTree3@t@Node *node,
                                 VsgPRTree3@t@NodeInfo *father_info,
                                 vsgloc3 child_number,
                                 GTraverseType order,
                                 VsgRegion3 selector,
                                 VsgPRTree3@t@Func func,
                                 gpointer user_data,
                                 VsgPRTree3@t@Config *config,
                                 gpointer node_key)
{
  VsgPRTree3@t@NodeInfo node_info;
  guint8 i;
  vsgrloc3 ipow;
  vsgrloc3 locmask = (selector == NULL) ? VSG_RLOC3_MASK :
    CALL_REGION3@T@_LOC (config, selector, &node->center);

  gint children[8];
  gpointer children_keys[8];

  _vsg_prtree3@t@node_get_info (node, &node_info, father_info, child_number);

#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node))
    {
      func (&node_info, user_data);
      return;
    }
#endif

  if (order == G_PRE_ORDER)
    func (&node_info, user_data);

  if (PRTREE3@T@NODE_ISINT (node))
    {
      config->children_order (node_key, children, children_keys,
                              config->children_order_data);

      for (i=0; i<4; i++)
	{
          gint ic = children[i];

          ipow = VSG_RLOC3_COMP (ic);

          if (ipow & locmask)
            _prtree3@t@node_traverse_custom (PRTREE3@T@NODE_CHILD(node, ic),
                                             &node_info, ic,
                                             order, selector, func, user_data,
                                             config, children_keys[i]);
	}
    }

  if (order == G_IN_ORDER)
    func (&node_info, user_data);

  if (PRTREE3@T@NODE_ISINT (node))
    {
      for (i=4; i<8; i++)
	{
          gint ic = children[i];

          ipow = VSG_RLOC3_COMP (ic);

          if (ipow & locmask)
            _prtree3@t@node_traverse_custom (PRTREE3@T@NODE_CHILD(node, ic),
                                             &node_info, ic,
                                             order, selector, func, user_data,
                                             config, children_keys[i]);
	}
    }

  if (order == G_POST_ORDER)
    func (&node_info, user_data);
}

typedef struct _CustomForeach CustomForeach;

struct _CustomForeach {
  GFunc func;
  gpointer data;
  VsgRegion3 selector;
  VsgRegion3Point3LocFunc locfunc;
};

static void _foreach_point_custom (const VsgPRTree3@t@NodeInfo *node_info,
                                   CustomForeach *custom)
{
  GSList *point_list = node_info->point_list;

#ifdef VSG_HAVE_MPI
  if (VSG_PRTREE3@T@_NODE_INFO_IS_REMOTE (node_info))
    return;
#endif

  while (point_list)
    {
      VsgPoint3 point = point_list->data;

      if (custom->locfunc (custom->selector, point) == VSG_RLOC3_MASK)
        custom->func (point, custom->data);

      point_list = g_slist_next (point_list);
    }
}

static void _foreach_region_custom (const VsgPRTree3@t@NodeInfo *node_info,
                                    CustomForeach *custom)
{
  GSList *region_list = node_info->region_list;

#ifdef VSG_HAVE_MPI
  if (VSG_PRTREE3@T@_NODE_INFO_IS_REMOTE (node_info))
    return;
#endif

  while (region_list)
    {
      VsgRegion3 region = region_list->data;

      custom->func (region, custom->data);

      region_list = g_slist_next (region_list);
    }
}

typedef struct _Foreach Foreach;

struct _Foreach {
  GFunc func;
  gpointer data;
};

static void _foreach_point (const VsgPRTree3@t@NodeInfo *node_info,
                            Foreach *custom)
{
  GSList *point_list = node_info->point_list;

#ifdef VSG_HAVE_MPI
  if (VSG_PRTREE3@T@_NODE_INFO_IS_REMOTE (node_info))
    return;
#endif

  while (point_list)
    {
      VsgPoint3 point = point_list->data;

      custom->func (point, custom->data);

      point_list = g_slist_next (point_list);
    }
}

static void _foreach_region (const VsgPRTree3@t@NodeInfo *node_info,
                             Foreach *custom)
{
  GSList *region_list = node_info->region_list;

#ifdef VSG_HAVE_MPI
  if (VSG_PRTREE3@T@_NODE_INFO_IS_REMOTE (node_info))
    return;
#endif

  while (region_list)
    {
      VsgRegion3 region = region_list->data;

      custom->func (region, custom->data);

      region_list = g_slist_next (region_list);
    }
}

static void
_z_order_data (gpointer key, gint *children, gpointer *children_keys,
               gpointer user_data)
{
  static const gint zorder[8] = {0, 1, 2, 3, 4, 5, 6, 7};

  memcpy (children, zorder, 8 * sizeof (gint));
}

static void _clone_parallel_status (const VsgPRTree3@t@NodeInfo *node_info,
                                    VsgPRTree3@t@Node *cloned)
{
  if (! VSG_PRTREE3@T@_NODE_INFO_IS_LOCAL (node_info))
    {
      VsgPRTree3@t@Node *cloned_node;

      cloned_node = _vsg_prtree3@t@node_get_child_at (cloned,
                                                      &node_info->center,
                                                      node_info->depth);

      cloned_node->parallel_status = node_info->parallel_status;
    }
}

typedef struct _GTypeAndModel GTypeAndModel;
struct _GTypeAndModel {
  GType data_type;
  gpointer data_model;
};

static gpointer _gtype_node_data_alloc (gboolean resident, GTypeAndModel *gam)
{
  return g_boxed_copy (gam->data_type, gam->data_model);
}

static void _gtype_node_data_destroy (gpointer data, gboolean resident,
                                 GTypeAndModel *gam)
{
  g_boxed_free (gam->data_type, data);
}

gboolean _vsg_prtree3@t@_check_user_data_model (VsgPRTree3@t@ *tree)
{
  return tree->config.parallel_config.node_data.alloc == 
    (VsgMigrableAllocDataFunc) _gtype_node_data_alloc;
}

/*-------------------------------------------------------------------*/
/* typedefs and structure doc */
/*-------------------------------------------------------------------*/
/**
 * VsgPRTree3@t@:
 *
 * Opaque structure. It must be manipulated only by the following functions.
 */

/**
 * VsgPoint3@t@LocFunc:
 * @candidate: a #VsgPoint3
 * @center: a #VsgVector3@t@
 *
 * Type of functions that perform #VsgPoint3 localization.
 *
 * Returns: localization quadrant (in #vsgloc3 form).
 */

/**
 * VsgRegion3@t@LocFunc:
 * @region: a #VsgRegion3
 * @center: a #VsgVector3@t@
 *
 * Type of functions that perform #VsgRegion3 localization.
 *
 * Returns: localization quadrant(s) (in #vsgrloc3 form).
 */

/**
 * VsgPoint3@t@DistFunc:
 * @one: a #VsgPoint3
 * @other: a #VsgPoint3
 *
 * Type of functions that compute #VsgPoint3 distance
 *
 * Returns: the distance between @one and @other.
 */

/**
 * VSG_TYPE_PRTREE3@T@:
 *
 * The #GBoxed #GType associated to #VsgPRTree3@t@.
 */

/**
 * VSG_PRTREE3@T@_NODE_INFO_IS_REMOTE:
 * @node_info: a #VsgPRTree3@t@NodeInfo
 *
 * Convenience macro for questioning about @node_info storage type.
 *
 * Returns: #TRUE if @node_info has a storage type equal to #VSG_PARALLEL_REMOTE
 */

/**
 * VSG_PRTREE3@T@_NODE_INFO_IS_LOCAL:
 * @node_info: a #VsgPRTree3@t@NodeInfo
 *
 * Convenience macro for questioning about @node_info storage type.
 *
 * Returns: #TRUE if @node_info has a storage type equal to #VSG_PARALLEL_LOCAL
 */

/**
 * VSG_PRTREE3@T@_NODE_INFO_IS_SHARED:
 * @node_info: a #VsgPRTree3@t@NodeInfo
 *
 * Convenience macro for questioning about @node_info storage type.
 *
 * Returns: #TRUE if @node_info has a storage type equal to #VSG_PARALLEL_SHARED
 */

/**
 * VSG_PRTREE3@T@_NODE_INFO_PROC:
 * @node_info: a #VsgPRTree3@t@NodeInfo
 *
 * Convenience macro for questioning about @node_info processor number.
 *
 * Returns: node_info processor number
 */

/*-------------------------------------------------------------------*/
/* public functions */
/*-------------------------------------------------------------------*/

GType vsg_prtree3@t@_get_type (void)
{
  static GType prtree3@t@_type = 0;

  if (!prtree3@t@_type)
    {
      prtree3@t@_type =
        g_boxed_type_register_static ("VsgPRTree3@t@",
                                      (GBoxedCopyFunc) vsg_prtree3@t@_clone,
                                      (GBoxedFreeFunc) vsg_prtree3@t@_free);

    }

  return prtree3@t@_type;
}

/**
 * vsg_prtree3@t@_new_full:
 * @lbound: lower limit for coordinates of #VsgPoint3 to be stored
 * @ubound: upper limit for coordinates of #VsgPoint3 to be stored
 * @point_locfunc: the #VsgPoint3 localization function
 * @point_distfunc: the #VsgPoint3 distance function
 * @region_locfunc: the #VsgRegion3 localization function
 * @max_point: maximum number of #VsgPoint to be stored in a VsgPRTree3@t@ Leaf
 *
 * Allocates a new instance of #VsgPRTree3@t@. Specifying a @lax_point value of 0
 * means taking library default.
 *
 * Returns: newly allocated #VsgPRTree3@t@
 */
VsgPRTree3@t@ *
vsg_prtree3@t@_new_full (const VsgVector3@t@ *lbound,
                         const VsgVector3@t@ *ubound,
                         const VsgPoint3@t@LocFunc point_locfunc,
                         const VsgPoint3@t@DistFunc point_distfunc,
                         const VsgRegion3@t@LocFunc region_locfunc,
                         guint max_point)
{
  VsgPRTree3@t@ *prtree3@t@;
  VsgParallelStatus parallel_status = {VSG_PARALLEL_LOCAL, 0};

  g_return_val_if_fail (lbound != NULL, NULL);
  g_return_val_if_fail (ubound != NULL, NULL);
  g_return_val_if_fail (point_locfunc != NULL, NULL);

  prtree3@t@ = _prtree3@t@_alloc ();

  prtree3@t@->node = _leaf_alloc(lbound, ubound, parallel_status,
                                 &prtree3@t@->config);

  vsg_prtree3@t@_set_point_loc (prtree3@t@, point_locfunc);

  vsg_prtree3@t@_set_point_dist (prtree3@t@, point_distfunc);

  vsg_prtree3@t@_set_region_loc (prtree3@t@, region_locfunc);

  prtree3@t@->config.tolerance = @epsilon@;

  prtree3@t@->config.children_order = _z_order_data;
  prtree3@t@->config.children_order_data = NULL;
  prtree3@t@->config.root_key = NULL;

  if (max_point == 0)
    max_point = PRTREE3@T@LEAF_MAXSIZE;
  prtree3@t@->config.max_point = max_point;

  prtree3@t@->config.remote_depth_dirty = TRUE;

  return prtree3@t@;
}

/**
 * vsg_prtree3@t@_new:
 * @lbound: lower limit for coordinates of #VsgPoint3 to be stored
 * @ubound: upper limit for coordinates of #VsgPoint3 to be stored
 * @region_locfunc: the #VsgRegion3 localization function
 * @maxpoint: maximum number of #VsgPoint to be stored in a VsgPRTree3@t@ Leaf
 *
 * Convenience macro that calls #vsg_prtree3@t@_new_full ().
 */

/**
 * vsg_prtree3@t@_free:
 * @prtree3@t@: a #VsgPRTree3@t@
 *
 * Frees @prtree3@t@.
 */
void vsg_prtree3@t@_free (VsgPRTree3@t@ *prtree3@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
#endif

  vsg_prtree3@t@node_free (prtree3@t@->node, &prtree3@t@->config);
  prtree3@t@->node = NULL;

  _prtree3@t@_dealloc (prtree3@t@);
}

/**
 * vsg_prtree3@t@_clone:
 * @prtree3@t@: a #VsgPRTree3@t@
 *
 * Copies @prtree3@t@ into a newly allocated #VsgPRTree3@t@. All references
 * to #VsgPoint@3@ and #VsgRegion3 from @prtree3@t@ are copied into the
 * new tree.
 *
 * Returns: a copy of @prtree3@t@.
 */
VsgPRTree3@t@ *vsg_prtree3@t@_clone (VsgPRTree3@t@ *prtree3@t@)
{
  VsgPRTree3@t@ *res;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, NULL);
#endif

  res = _prtree3@t@_alloc ();

  res->config.point_loc_func = prtree3@t@->config.point_loc_func;
  res->config.point_loc_data = prtree3@t@->config.point_loc_data;

  res->config.point_dist_func =
    prtree3@t@->config.point_dist_func;
  res->config.point_dist_data = prtree3@t@->config.point_dist_data;

  res->config.region_loc_func = prtree3@t@->config.region_loc_func;
  res->config.region_loc_data = prtree3@t@->config.region_loc_data;
  res->config.tolerance = prtree3@t@->config.tolerance;

  res->config.max_point = prtree3@t@->config.max_point;

  res->config.children_order = prtree3@t@->config.children_order;
  res->config.children_order_data = prtree3@t@->config.children_order_data;
  res->config.root_key = prtree3@t@->config.root_key;

  memcpy (&res->config.parallel_config, &prtree3@t@->config.parallel_config,
          sizeof (VsgPRTreeParallelConfig));
  if (prtree3@t@->config.parallel_config.node_data.alloc ==
      (VsgMigrableAllocDataFunc) _gtype_node_data_alloc)
    {
      GTypeAndModel *src =
        (GTypeAndModel *) prtree3@t@->config.parallel_config.node_data.alloc_data;
      GTypeAndModel *gam = g_malloc (sizeof (GTypeAndModel));

      gam->data_type = src->data_type;
      gam->data_model = g_boxed_copy (src->data_type, src->data_model);

      res->config.parallel_config.node_data.alloc_data = gam;
      res->config.parallel_config.node_data.destroy_data = gam;
    }

  res->node = _leaf_alloc(&prtree3@t@->node->lbound,
                          &prtree3@t@->node->ubound,
                          prtree3@t@->node->parallel_status,
                          &res->config);

  vsg_prtree3@t@_foreach_point (prtree3@t@, (GFunc) _copy_point, res);

  vsg_prtree3@t@_foreach_region (prtree3@t@, (GFunc) _copy_region, res);

  /* copy parallel_status between shared or remote nodes */
  vsg_prtree3@t@_traverse (prtree3@t@, G_PRE_ORDER,
                           (VsgPRTree3@t@Func) _clone_parallel_status,
                           res);
  return res;
}

/**
 * vsg_prtree3@t@_set_point_loc:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @locfunc: the localization function.
 *
 * Configure @prtree3@t@ for marshalling its VsgPoint3@t@ localization.
 */
void
vsg_prtree3@t@_set_point_loc (VsgPRTree3@t@ *prtree3@t@,
                              VsgPoint3@t@LocFunc locfunc)
{
  /*
   * We force locfunc as a VsgPoint3@t@LocDataFunc as adding NULL as a
   * dummy argument in the call is (should be?) harmless.
   */
  vsg_prtree3@t@_set_point_loc_with_data (prtree3@t@,
                                          (VsgPoint3@t@LocDataFunc) locfunc,
                                          NULL);
}

/**
 * vsg_prtree3@t@_set_point_loc_with_data:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @locfunc: the localization function.
 * @locdata: the data passed to @locfunc.
 *
 * Configure @prtree3@t@ for marshalling its VsgPoint3@t@ localization.
 */
void
vsg_prtree3@t@_set_point_loc_with_data (VsgPRTree3@t@ *prtree3@t@,
                                        VsgPoint3@t@LocDataFunc locfunc,
                                        gpointer locdata)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (locfunc != NULL);
#endif

  prtree3@t@->config.point_loc_func = locfunc;

  prtree3@t@->config.point_loc_data = locdata;
}

/**
 * vsg_prtree3@t@_set_region_loc:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @locfunc: the localization function.
 *
 * Configure @prtree3@t@ for marshalling its VsgRegion3@t@ localization.
 */
void vsg_prtree3@t@_set_region_loc (VsgPRTree3@t@ *prtree3@t@,
                                    VsgRegion3@t@LocFunc locfunc)
{
  /*
   * We force locfunc as a VsgRegion3@t@LocDataFunc as adding NULL as a
   * dummy argument in the call is (should be?) harmless.
   */
  vsg_prtree3@t@_set_region_loc_with_data (prtree3@t@,
                                           (VsgRegion3@t@LocDataFunc) locfunc,
                                           NULL);
}

/**
 * vsg_prtree3@t@_set_region_loc_with_data:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @locfunc: the localization function.
 * @locdata: the data passed to @locfunc.
 *
 * Configure @prtree3@t@ for marshalling its VsgRegion3@t@ localization.
 */
void vsg_prtree3@t@_set_region_loc_with_data (VsgPRTree3@t@ *prtree3@t@,
                                              VsgRegion3@t@LocDataFunc locfunc,
                                              gpointer locdata)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (locfunc != NULL);
#endif

  prtree3@t@->config.region_loc_func = locfunc;

  prtree3@t@->config.region_loc_data = locdata;
}

/**
 * vsg_prtree3@t@_set_point_dist:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @distfunc: the distance function.
 *
 * Configure @prtree3@t@ for marshalling its VsgRegion3@t@ distance evaluation.
 */
void
vsg_prtree3@t@_set_point_dist (VsgPRTree3@t@ *prtree3@t@,
                               VsgPoint3@t@DistFunc distfunc)
{
  /*
   * We force distfunc as a VsgPoint3@t@DistDataFunc as adding NULL as a
   * dummy argument in the call is (should be?) harmless.
   */
  vsg_prtree3@t@_set_point_dist_with_data (prtree3@t@,
                                           (VsgPoint3@t@DistDataFunc) distfunc,
                                           NULL);
}

/**
 * vsg_prtree3@t@_set_point_dist_with_data:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @distfunc: the distance function.
 * @distdata: the data passed to @distfunc.
 *
 * Configure @prtree3@t@ for marshalling its VsgRegion3@t@ distance evaluation.
 */
void
vsg_prtree3@t@_set_point_dist_with_data (VsgPRTree3@t@ *prtree3@t@,
                                         VsgPoint3@t@DistDataFunc distfunc,
                                         gpointer distdata)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (distfunc != NULL || distdata != NULL);
#endif

  prtree3@t@->config.point_dist_func = distfunc;

  prtree3@t@->config.point_dist_data = distdata;
}

/**
 * vsg_prtree3@t@_set_children_order_with_data :
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @children_order: a function for defining children order in traversals.
 * @root_key: the key to pass to @children_order for the first (root) node.
 * @user_data: user provided data that will be passed to @children_order at each call.
 *
 * Configures @prtree3@t@ to use @children_order for determining children order
 * in traversals. If @children_order is set to %NULL, children ordering returns
 * to the default Z-order.
 *
 * Applies only to single traversal operations (currently not to near/far
 * traversals).
 */
void
vsg_prtree3@t@_set_children_order_with_data (VsgPRTree3@t@ *prtree3@t@,
                                             VsgChildrenOrderDataFunc children_order,
                                             gpointer root_key,
                                             gpointer user_data)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, -1.);
#endif

  if (children_order == NULL)
    {
      children_order = _z_order_data;
      root_key = NULL;
      user_data = NULL;
    }

  prtree3@t@->config.children_order = children_order;

  prtree3@t@->config.root_key = root_key;
  prtree3@t@->config.children_order_data = user_data;
}

/**
 * vsg_prtree3@t@_set_children_order :
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @children_order: a function for defining children order in traversals.
 * @root_key: the key to pass to @children_order for the first (root) node.
 *
 * Configures @prtree3@t@ to use @children_order for determining children order
 * in traversals. If @children_order is set to %NULL, children ordering returns
 * to the default Z-order.
 *
 * Applies only to single traversal operations (currently not to near/far
 * traversals).
 */
void
vsg_prtree3@t@_set_children_order (VsgPRTree3@t@ *prtree3@t@,
                                   VsgChildrenOrderFunc children_order,
                                   gpointer root_key)
{
  vsg_prtree3@t@_set_children_order_with_data (prtree3@t@,
                                               (VsgChildrenOrderDataFunc) children_order,
                                               root_key, NULL);
}

/**
 * vsg_prtree3@t@_get_tolerance:
 * @prtree3@t@: a #VsgPRTree3@t@
 *
 * Read access to current @prtree3@t@ spatial tolerance.
 *
 * Returns: @prtree3@t@ spatial tolerance.
 */
@type@ vsg_prtree3@t@_get_tolerance (VsgPRTree3@t@ *prtree3@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, -1.);
#endif

  return prtree3@t@->config.tolerance;
}

/**
 * vsg_prtree3@t@_set_tolerance:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @tolerance: a #@type@
 *
 * Sets @prtree3@t@ spatial tolerance to the value of @tolerance.
 * Any distance below @tolerance will be considered as %0. by
 * @prtree3@t@.
 */
void vsg_prtree3@t@_set_tolerance (VsgPRTree3@t@ *prtree3@t@,
                                   @type@ tolerance)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
#endif

  g_return_if_fail (tolerance >= 0.);

  prtree3@t@->config.tolerance = tolerance;
}

/**
 * vsg_prtree3@t@_set_node_data:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @user_data_type: a #GType derived from %G_BOXED_TYPE.
 * @user_data_model: a #GBoxed instance of type @user_data_type.
 *
 * Sets the user data type associated with each nodes of @prtree3@t@ to
 * @user_data_type. If a data type was already present in @prtree3@t@, it
 * will be deleted.
 */
void vsg_prtree3@t@_set_node_data (VsgPRTree3@t@ *prtree3@t@,
                                   GType user_data_type,
                                   gpointer user_data_model)
{
  VsgParallelVTable vtable= {0,};
  GTypeAndModel *gam;
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
#endif

  if (user_data_type == G_TYPE_NONE)
    {
      vsg_prtree3@t@_set_node_data_vtable (prtree3@t@, &vtable);
      return;
    }

  g_return_if_fail (g_type_is_a (user_data_type, G_TYPE_BOXED));
  g_return_if_fail (user_data_model != NULL);

  user_data_model = g_boxed_copy (user_data_type, user_data_model);

  gam = g_malloc (sizeof (GTypeAndModel));

  gam->data_type = user_data_type;
  gam->data_model = user_data_model;

  vtable.alloc = (VsgMigrableAllocDataFunc) _gtype_node_data_alloc;
  vtable.alloc_data = gam;
  vtable.destroy = (VsgMigrableDestroyDataFunc) _gtype_node_data_destroy;
  vtable.destroy_data = gam;

  vsg_prtree3@t@_set_node_data_vtable (prtree3@t@, &vtable);
}

/**
 * vsg_prtree3@t@_set_node_data_vtable:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @vtable: VsgParallelVTable.
 *
 * Sets the user data vtable for allocating and destroying node_data at every
 * node of prtree3@t@.
 */
void vsg_prtree3@t@_set_node_data_vtable (VsgPRTree3@t@ *prtree3@t@,
                                          VsgParallelVTable *vtable)
{
  VsgParallelVTable *node_data_vtable;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (vtable != NULL);
#endif

  node_data_vtable = &prtree3@t@->config.parallel_config.node_data;

  if (node_data_vtable->alloc != vtable->alloc ||
      node_data_vtable->alloc_data != vtable->alloc_data)
    {
      if (node_data_vtable->alloc_data != NULL &&
          node_data_vtable->alloc == (VsgMigrableAllocDataFunc)
          _gtype_node_data_alloc)
        {
          GTypeAndModel *gam = node_data_vtable->alloc_data;

          _gtype_node_data_destroy (gam->data_model, FALSE, gam);

          g_free (gam);

          node_data_vtable->alloc = NULL;
          node_data_vtable->destroy = NULL;
        }

      if (prtree3@t@->node)
        _prtree3@t@node_update_user_data_vtable (prtree3@t@->node,
                                                 node_data_vtable,
                                                 vtable);
    }

  memcpy (node_data_vtable, vtable, sizeof (VsgParallelVTable));

}

/**
 * vsg_prtree3@t@_get_bounds:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @lbound: a #VsgVector3@t@.
 * @ubound: a #VsgVector3@t@.
 *
 * Get the current lower and upper corners of @prtree3@t@ bounding box.
 */
void vsg_prtree3@t@_get_bounds (VsgPRTree3@t@ *prtree3@t@,
                                VsgVector3@t@ *lbound,
                                VsgVector3@t@ *ubound)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (lbound != NULL);
  g_return_if_fail (ubound != NULL);
#endif

  vsg_vector3@t@_copy (&prtree3@t@->node->lbound, lbound);
  vsg_vector3@t@_copy (&prtree3@t@->node->ubound, ubound);
}

/**
 * vsg_prtree3@t@_depth:
 * @prtree3@t@: a #VsgPRTree3@t@
 *
 * Computes the depth of @prtree3@t@.
 *
 * Returns: Depth of @prtree3@t@.
 */
guint vsg_prtree3@t@_depth (const VsgPRTree3@t@ *prtree3@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, 0);
#endif

  return _prtree3@t@node_depth (prtree3@t@->node);
}

/**
 * vsg_prtree3@t@_point_count:
 * @prtree3@t@: a #VsgPRTree3@t@
 *
 * Computes the number of #VsgPoint3 currently stored in @prtree3@t@.
 *
 * Returns: Number of #VsgPoint3 currently stored in @prtree3@t@.
 */
guint vsg_prtree3@t@_point_count (const VsgPRTree3@t@ *prtree3@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, 0);
#endif

  return _prtree3@t@node_point_count (prtree3@t@->node);
}

/**
 * vsg_prtree3@t@_region_count:
 * @prtree3@t@: a #VsgPRTree3@t@
 *
 * Computes the number of #VsgRegion3 currently stored in @prtree3@t@.
 *
 * Returns: Number of #VsgRegion3 currently stored in @prtree3@t@.
 */
guint vsg_prtree3@t@_region_count (const VsgPRTree3@t@ *prtree3@t@)
{
  g_return_val_if_fail (prtree3@t@ != NULL, 0);

  return _prtree3@t@node_region_count (prtree3@t@->node);
}

/**
 * Extends @prtree3@t@ bounds in order to allow @point to be inside the box.
 * @extk is the key the old root node in the new tree.
 */
void vsg_prtree3@t@_bounds_extend (VsgPRTree3@t@ *prtree3@t@,
                                   VsgPoint3 point, VsgPRTreeKey3@t@ *extk)
{
  const VsgVector3@t@ *lbound;
  const VsgVector3@t@ *ubound;
  const VsgPRTree3@t@Config *config;

  config = &prtree3@t@->config;

  lbound = &prtree3@t@->node->lbound;
  ubound = &prtree3@t@->node->ubound;

  while (CALL_POINT3@T@_LOC (config, point, lbound) != VSG_LOC3_UNE ||
         CALL_POINT3@T@_LOC (config, point, ubound) != VSG_LOC3_BSW)
    {
      VsgVector3@t@ center = {0.,};
      VsgVector3@t@ diff, new_lbound, new_ubound;
      vsgloc3 loc;

      vsg_vector3@t@_lerp (lbound, ubound, 0.5, &center);

      loc = CALL_POINT3@T@_LOC (config, point, &center);

      vsg_vector3@t@_sub (ubound, lbound, &diff);

      vsg_vector3@t@_copy (lbound, &new_lbound);
      vsg_vector3@t@_copy (ubound, &new_ubound);

      if (loc & VSG_LOC3_X)
        new_ubound.x += diff.x;
      else
        new_lbound.x -= diff.x;

      if (loc & VSG_LOC3_Y)
        new_ubound.y += diff.y;
      else
        new_lbound.y -= diff.y;

      if (loc & VSG_LOC3_Z)
        new_ubound.z += diff.z;
      else
        new_lbound.z -= diff.z;

      loc = VSG_LOC3_MASK & ~(loc);
      prtree3@t@->node = _int_alloc (&new_lbound,
                                     &new_ubound,
                                     prtree3@t@->node,
                                     loc,
                                     config);

      vsg_prtree_key3@t@_build_father (extk, loc, extk);

      lbound = &prtree3@t@->node->lbound;
      ubound = &prtree3@t@->node->ubound;
    }
}

/**
 * vsg_prtree3@t@_insert_point:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @point: a #VsgPoint3
 *
 * Stores @point reference in prtree3@t@. (#GObject reference support not
 * provided)
 */
void vsg_prtree3@t@_insert_point (VsgPRTree3@t@ *prtree3@t@,
                                  VsgPoint3 point)
{
  VsgPRTreeKey3@t@ extk = vsg_prtree_key3@t@_root;
  const VsgVector3@t@ *lbound;
  const VsgVector3@t@ *ubound;
  const VsgPRTree3@t@Config *config;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (point != NULL);
#endif

  config = &prtree3@t@->config;

  lbound = &prtree3@t@->node->lbound;
  ubound = &prtree3@t@->node->ubound;

  if (CALL_POINT3@T@_LOC (config, point, lbound) != VSG_LOC3_UNE ||
      CALL_POINT3@T@_LOC (config, point, ubound) != VSG_LOC3_BSW)
    {
#ifdef VSG_HAVE_MPI
      if (config->parallel_config.communicator != MPI_COMM_NULL)
        {
          gint sz;

          MPI_Comm_size (config->parallel_config.communicator, &sz);

          if (sz > 1)
            {
              /* store exterior point in appropriate place and return */
              GSList *point_list = g_slist_alloc ();

              point_list->data = (gpointer) point;
              point_list->next = NULL;

              prtree3@t@->pending_exterior_points =
                g_slist_concat (point_list,
                                prtree3@t@->pending_exterior_points);

/*               gint rk; */
/*               MPI_Comm_rank (MPI_COMM_WORLD, &rk); */
/*               g_printerr ("%d: store exterior ", rk); */
/*               vsg_vector3@t@_write (point, stderr); */
/*               g_printerr ("\n"); */

              return;
            }
        }
#endif

      vsg_prtree3@t@_bounds_extend (prtree3@t@, point, &extk);
    }

  _prtree3@t@node_insert_point (prtree3@t@->node, point, config);
}

/**
 * vsg_prtree3@t@_insert_point_local:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @point: a #VsgPoint3
 *
 * tries to locally store @point reference in prtree3@t@. If
 * @prtree3@t@ is in a parallel state and @point falls in a remote
 * area, then, @point will not be stored and #FALSE will be returned.
 *
 * Returns: #TRUE when @point is stored locally in the tree. #FALSE
 * when @point could not be stored.
 */
gboolean vsg_prtree3@t@_insert_point_local (VsgPRTree3@t@ *prtree3@t@,
                                            VsgPoint3 point)
{
  VsgPRTreeKey3@t@ extk = vsg_prtree_key3@t@_root;
  const VsgVector3@t@ *lbound;
  const VsgVector3@t@ *ubound;
  const VsgPRTree3@t@Config *config;

#ifndef VSG_HAVE_MPI
  vsg_prtree3@t@_insert_point (prtree3@t@, point);
  return TRUE;
#else

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, FALSE);
  g_return_val_if_fail (point != NULL, FALSE);
#endif

  config = &prtree3@t@->config;

  lbound = &prtree3@t@->node->lbound;
  ubound = &prtree3@t@->node->ubound;

  if (CALL_POINT3@T@_LOC (config, point, lbound) != VSG_LOC3_UNE ||
      CALL_POINT3@T@_LOC (config, point, ubound) != VSG_LOC3_BSW)
    {
      if (config->parallel_config.communicator != MPI_COMM_NULL)
        {
          gint rk;

          MPI_Comm_rank (config->parallel_config.communicator, &rk);

          /* exterior points arbitrary belong to proc 0 */
          if (rk == 0)
            {
              /* store exterior point in appropriate place and return */
              GSList *point_list = g_slist_alloc ();

              point_list->data = (gpointer) point;
              point_list->next = NULL;

              prtree3@t@->pending_exterior_points =
                g_slist_concat (point_list,
                                prtree3@t@->pending_exterior_points);

              return TRUE;
            }
          else
            {
              return FALSE;
            }
        }

      vsg_prtree3@t@_bounds_extend (prtree3@t@, point, &extk);
    }

  return
    _prtree3@t@node_insert_point_local (prtree3@t@->node, point, config) >= 0;

#endif
}

/**
 * vsg_prtree3@t@_remove_point:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @point: a #VsgPoint3
 *
 * Removes specified #VsgPoint3 reference from @prtree3@t@.
 *
 * Returns: %TRUE if @point was found and removed, %FALSE otherwise.
 */
gboolean vsg_prtree3@t@_remove_point (VsgPRTree3@t@ *prtree3@t@,
                                      VsgPoint3 point)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, FALSE);
  g_return_val_if_fail (point != NULL, FALSE);
#endif

  return
    _prtree3@t@node_remove_point (prtree3@t@->node, point,
                                  &prtree3@t@->config);
}

/**
 * vsg_prtree3@t@_write:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @file: a #FILE
 * 
 * Writes a summary of @prtree3@t@ in @file. Mainly used for debugging.
 */
void vsg_prtree3@t@_write (VsgPRTree3@t@ *prtree3@t@, FILE *file)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (file != NULL);
#endif

  vsg_prtree3@t@_traverse_custom_internal (prtree3@t@, G_PRE_ORDER, NULL,
                                           NULL, NULL,
                                           (VsgPRTree3@t@InternalFunc) _prtree3@t@node_write,
                                           file);
}

/**
 * vsg_prtree3@t@_print:
 * @prtree3@t@: a #VsgPRTree3@t@
 *
 * Prints a summary of @prtree3@t@ on standard output. Mainly used for debugging.
 */
void vsg_prtree3@t@_print (VsgPRTree3@t@ *prtree3@t@)
{
  vsg_prtree3@t@_write (prtree3@t@, stdout);
}

/**
 * vsg_prtree3@t@_find_point:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @selector: a #VsgPoint3
 *
 * Search @prtree3@t@ for a #VsgPoint3 which distance from @selector
 * is zero.
 *
 * Returns: the first #VsgPoint3 found or NULL.
 */
VsgPoint3 vsg_prtree3@t@_find_point (VsgPRTree3@t@ *prtree3@t@,
                                     VsgPoint3 selector)
{
  const VsgVector3@t@ *lbound;
  const VsgVector3@t@ *ubound;
  const VsgPRTree3@t@Config *config;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, NULL);
  g_return_val_if_fail (selector != NULL, NULL);
#endif

  config = &prtree3@t@->config;

  lbound = &prtree3@t@->node->lbound;
  ubound = &prtree3@t@->node->ubound;

  if (CALL_POINT3@T@_LOC (config, selector, lbound) != VSG_LOC3_BNE ||
      CALL_POINT3@T@_LOC (config, selector, ubound) != VSG_LOC3_USW)
    return NULL;

  return _prtree3@t@node_find_point (prtree3@t@->node, selector, config);
}

/**
 * vsg_prtree3@t@_foreach_point:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @func: a #GFunc
 * @user_data: a #gpointer associated to func
 *
 * Performs a "@func (point, @user_data)" call on every #VsgPoint3
 * found in @prtree3@t@.
 */
void vsg_prtree3@t@_foreach_point (VsgPRTree3@t@ *prtree3@t@, GFunc func,
                                   gpointer user_data)
{
  Foreach foreach = {func, user_data};

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (func != NULL);
#endif

  vsg_prtree3@t@_traverse (prtree3@t@, G_POST_ORDER,
                           (VsgPRTree3@t@Func) _foreach_point,
                           &foreach);
}

/**
 * vsg_prtree3@t@_foreach_point_custom:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @selector: a #VsgRegion3
 * @locfunc: #VsgRegion3Point3LocFunc associated to @selector
 * @func: a #GFunc
 * @user_data: a #gpointer associated to func
 *
 * Performs a "@func (point, @user_data)" call on every #VsgPoint3
 * conforming to @selector/@locfunc test found in @prtree3@t@.
 */
void
vsg_prtree3@t@_foreach_point_custom (VsgPRTree3@t@ *prtree3@t@,
                                     VsgRegion3 selector,
                                     VsgRegion3Point3LocFunc locfunc,
                                     GFunc func,
                                     gpointer user_data)
{
  CustomForeach custom = {func, user_data, selector, locfunc};

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (func != NULL);
  g_return_if_fail (prtree3@t@->config.region_loc_data != NULL);
#endif

  if (custom.locfunc == NULL)
    custom.locfunc = _region3_point3_locfunc_always;

  vsg_prtree3@t@_traverse_custom (prtree3@t@, G_POST_ORDER, selector,
                                  (VsgPRTree3@t@Func) _foreach_point_custom,
                                  &custom);
}

/**
 * vsg_prtree3@t@_insert_region:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @region: a #VsgRegion
 *
 *  Inserts @region in @prtree3@t@ lists.
 */
void vsg_prtree3@t@_insert_region (VsgPRTree3@t@ *prtree3@t@,
                                   VsgRegion3 region)
{
  GSList **shared_regions = NULL;
  VsgVector3@t@ *lbound;
  VsgVector3@t@ *ubound;
  const VsgPRTree3@t@Config *config;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (region != NULL);
#endif

  config = &prtree3@t@->config;

#ifdef VSG_HAVE_MPI
  if (prtree3@t@->config.parallel_config.communicator != MPI_COMM_NULL)
    shared_regions = &prtree3@t@->pending_shared_regions;
#endif


  lbound = &prtree3@t@->node->lbound;
  ubound = &prtree3@t@->node->ubound;

  if (CALL_REGION3@T@_LOC (config, region, lbound) != VSG_RLOC3_UNE ||
      CALL_REGION3@T@_LOC (config, region, ubound) != VSG_RLOC3_BSW)
    {
      prtree3@t@->node->region_list =
        g_slist_append (prtree3@t@->node->region_list, region);

#ifdef VSG_HAVE_MPI
      /* shared regions in shared nodes are stored to be notified
       * to *all* the processors.
       */
      if (PRTREE3@T@NODE_IS_SHARED (prtree3@t@->node))
        *shared_regions = g_slist_append (*shared_regions, region);
#endif

      prtree3@t@->node->region_count ++;

      return;
    }

  _prtree3@t@node_insert_region (prtree3@t@->node, region,
                                 &prtree3@t@->config, shared_regions);
}

/**
 * vsg_prtree3@t@_remove_region:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @region: a #VsgRegion
 *
 * Removes specified @region from @prtree3@t@ lists.
 *
 * Returns: %TRUE if @region was in @prtree3@t@, %FALSE if not.
 */
gboolean vsg_prtree3@t@_remove_region (VsgPRTree3@t@ *prtree3@t@,
                                       VsgRegion3 region)
{
  VsgVector3@t@ *lbound;
  VsgVector3@t@ *ubound;
  const VsgPRTree3@t@Config *config;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, FALSE);
  g_return_val_if_fail (region != NULL, FALSE);
#endif

  config = &prtree3@t@->config;

  lbound = &prtree3@t@->node->lbound;
  ubound = &prtree3@t@->node->ubound;

  if (CALL_REGION3@T@_LOC (config, region, lbound) != VSG_RLOC3_UNE ||
      CALL_REGION3@T@_LOC (config, region, ubound) != VSG_RLOC3_BSW)
    {
      if (g_slist_find (prtree3@t@->node->region_list, region) != NULL)
        {
          prtree3@t@->node->region_list =
            g_slist_remove (prtree3@t@->node->region_list, region);

          prtree3@t@->node->region_count --;

          return TRUE;
        }

      return FALSE;
    }

  return
    _prtree3@t@node_remove_region (prtree3@t@->node, region,
                                   &prtree3@t@->config);
}

/**
 * vsg_prtree3@t@_foreach_region:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @func: function to perform on found #VsgRegion3
 * @user_data: user provided pointer
 *
 * Performs "@func (region, @user_data)" call on every #VsgRegion3 in @prtree3@t@.
 */
void vsg_prtree3@t@_foreach_region (VsgPRTree3@t@ *prtree3@t@, GFunc func,
                                    gpointer user_data)
{
  Foreach foreach = {func, user_data};

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (func != NULL);
#endif

  vsg_prtree3@t@_traverse (prtree3@t@, G_POST_ORDER,
                           (VsgPRTree3@t@Func) _foreach_region,
                           &foreach);
}

/**
 * vsg_prtree3@t@_foreach_region_custom:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @selector: the #VsgRegion3 used to specify search
 * @func: function to perform on found #VsgRegion3
 * @user_data: user provided pointer
 *
 * Performs "@func (region, @user_data)" call on found #VsgRegion3 in 
 * @prtree3@t@. Warning: call to @func will be performed on every #VsgRegion3 that
 * is in a @prtree3@t@ sub-node that matches @selector/@locfunc localzation. This
 * means @locfunc must provide its own #VsgRegion3/#VsgRegion3 intersection
 * tests if it needs to.
 */
void vsg_prtree3@t@_foreach_region_custom (VsgPRTree3@t@ *prtree3@t@,
                                           VsgRegion3 selector,
                                           GFunc func,
                                           gpointer user_data)
{
  CustomForeach custom = {func, user_data, selector, NULL};

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (func != NULL);
#endif

  vsg_prtree3@t@_traverse_custom (prtree3@t@, G_POST_ORDER, selector,
                                  (VsgPRTree3@t@Func) _foreach_region_custom,
                                  &custom);
}

static gboolean _checkregion3_true (const VsgRegion3 region,
                                    gpointer user_data)
{
  return TRUE;
}
/**
 * vsg_prtree3@t@_find_deep_region:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @point: a #VsgPoint3 or %NULL
 * @check: a #VsgRegion3CheckFunc
 * @user_data: a pointer to be passed to @check
 *
 * Performs a depth first search of a #VsgRegion3 in the vicinity of
 * @point. If @check is not %NULL, the returned #VsgRegion3 is the
 * first one that return %TRUE to a call of @check().
 *
 * Returns: the deepest #VsgRegion3 found.
 */
VsgRegion3 vsg_prtree3@t@_find_deep_region (VsgPRTree3@t@ *prtree3@t@,
                                            VsgPoint3 point,
                                            VsgRegion3CheckFunc check,
                                            gpointer user_data)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, NULL);
#endif

  if (check == NULL) check = _checkregion3_true;

  return _prtree3@t@node_find_deep_region (prtree3@t@->node,
                                           point,
                                           check,
                                           user_data,
                                           &prtree3@t@->config);
}

/**
 * vsg_prtree3@t@_traverse:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @order: traverse order.
 * @func: a #VsgPRTree3@t@Func.
 * @user_data: a #gpointer data to be apssed to @func.
 *
 * Performs a traversal of @prtree3@t@, executing @func on each of its nodes
 * in @order order.
 *
 * WARNING: Please note that %G_LEVEL_ORDER is not currenlty implemented.
 */
void vsg_prtree3@t@_traverse (VsgPRTree3@t@ *prtree3@t@,
                              GTraverseType order,
                              VsgPRTree3@t@Func func,
                              gpointer user_data)
{
  vsg_prtree3@t@_traverse_custom (prtree3@t@, order, NULL, func, user_data);
}

/**
 * vsg_prtree3@t@_traverse_custom:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @order: traverse order.
 * @selector: a #VsgRegion3
 * @func: a #VsgPRTree3@t@Func.
 * @user_data: a #gpointer data to be apssed to @func.
 *
 * Performs a traversal of @prtree3@t@, executing @func on each of its nodes
 * that intersects @selector (as defined by provided region_locfunc in
 * vsg_prtree3@t@_new_full()). The @order parameter refers to the order between
 * @func call and children traversal for each node. If you need to set a
 * specific order between node's children, please refer to
 * vsg_prtree3@t@_set_children_order() function.
 *
 * WARNING: Please note that %G_LEVEL_ORDER is not currenlty implemented.
 */
void vsg_prtree3@t@_traverse_custom (VsgPRTree3@t@ *prtree3@t@,
                                     GTraverseType order,
                                     VsgRegion3 selector,
                                     VsgPRTree3@t@Func func,
                                     gpointer user_data)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (order != G_LEVEL_ORDER);
  g_return_if_fail (func != NULL);
#endif

  _prtree3@t@node_traverse_custom (prtree3@t@->node, NULL, 0,
                                   order, selector, func,
                                   user_data, &prtree3@t@->config,
                                   prtree3@t@->config.root_key);
}

void
vsg_prtree3@t@_traverse_custom_internal (VsgPRTree3@t@ *prtree3@t@,
                                         GTraverseType order,
                                         VsgRegion3@t@InternalLocDataFunc sel_func,
                                         VsgRegion3 selector,
                                         gpointer sel_data,
                                         VsgPRTree3@t@InternalFunc func,
                                         gpointer user_data)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (order != G_LEVEL_ORDER);
  g_return_if_fail (func != NULL);
#endif

  {
    VsgPRTree3@t@Config conf = prtree3@t@->config;

    _prtree3@t@node_traverse_custom_internal (prtree3@t@->node, NULL, 0,
                                              order, sel_func, selector,
                                              sel_data, func,
                                              user_data, &conf,
                                              prtree3@t@->config.root_key);
  }
}

GType vsg_prtree3@t@_node_info_get_type (void)
{
  static GType prtree3@t@_node_info_type = 0;

  if (!prtree3@t@_node_info_type)
    {
      prtree3@t@_node_info_type =
        g_boxed_type_register_static ("VsgPRTree3@t@NodeInfo",
                                      (GBoxedCopyFunc) vsg_prtree3@t@_node_info_clone,
                                      (GBoxedFreeFunc) vsg_prtree3@t@_node_info_free);

    }

  return prtree3@t@_node_info_type;
}

void vsg_prtree3@t@_node_info_free (VsgPRTree3@t@NodeInfo *node_info)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (node_info != NULL);
#endif

  g_free (node_info);
}

VsgPRTree3@t@NodeInfo *
vsg_prtree3@t@_node_info_clone (VsgPRTree3@t@NodeInfo *node_info)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (node_info != NULL, NULL);
#endif

  return g_memdup (node_info, sizeof (VsgPRTree3@t@NodeInfo));
}
