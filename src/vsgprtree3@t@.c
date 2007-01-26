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

#include "vsg-config.h"

#include "vsgprtree3@t@.h"

#include "vsgprtree3@t@-private.h"

#include <string.h>

#define VSG_PRTREE3@T@_PREALLOC 32
#define PRTREE3@T@NODE_PREALLOC 256

#define PRTREE3@T@LEAF_MAXSIZE 5

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

void vsg_prtree3@t@_init()
{
  g_atexit (_prtree3@t@_finalize);
}

static VsgPRTree3@t@ *_prtree3@t@_alloc ()
{
  VsgPRTree3@t@ *ret;

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

  ret = g_chunk_new (VsgPRTree3@t@, vsg_prtree3@t@_mem_chunk);

  ret->config.user_data_type = G_TYPE_NONE;
  ret->config.user_data_model = NULL;

  return ret;
}

static void _prtree3@t@_dealloc (VsgPRTree3@t@ *prtree3@t@)
{
  if (prtree3@t@->config.user_data_type != G_TYPE_NONE)
    {
      g_boxed_free (prtree3@t@->config.user_data_type,
                    prtree3@t@->config.user_data_model);
    }

  g_chunk_free (prtree3@t@, vsg_prtree3@t@_mem_chunk);
  vsg_prtree3@t@_instances_count --;

  if (vsg_prtree3@t@_instances_count == 0)
    {
      _prtree3@t@_finalize ();
    }
}

static
VsgPRTree3@t@Node *_prtree3@t@node_alloc (const VsgVector3@t@ *lbound,
                                          const VsgVector3@t@ *ubound,
                                          const VsgPRTree3@t@Config *config)
{
  VsgPRTree3@t@Node *ret;
  ret = g_chunk_new (VsgPRTree3@t@Node, vsg_prtree3@t@node_mem_chunk);

  ret->region_list = NULL;

  ret->point_count = 0;
  ret->region_count = 0;

  vsg_vector3@t@_copy (lbound, &ret->lbound);
  vsg_vector3@t@_copy (ubound, &ret->ubound);

  vsg_vector3@t@_lerp (lbound, ubound, 0.5, &ret->center);

  if (config->user_data_type != G_TYPE_NONE)
    {
      ret->user_data = g_boxed_copy (config->user_data_type,
                                     config->user_data_model);
    }

  return ret;
}

static void _prtree3@t@node_dealloc (VsgPRTree3@t@Node *prtree3@t@node,
                                     const VsgPRTree3@t@Config *config)
{
  if (config->user_data_type != G_TYPE_NONE)
    {
      g_boxed_free (config->user_data_type,
                    prtree3@t@node->user_data);
    }

  g_chunk_free (prtree3@t@node, vsg_prtree3@t@node_mem_chunk);
}

static VsgPRTree3@t@Node *_leaf_alloc (const VsgVector3@t@ *lbound,
                                       const VsgVector3@t@ *ubound,
                                       const VsgPRTree3@t@Config *config)
{
  VsgPRTree3@t@Node *node = _prtree3@t@node_alloc (lbound, ubound, config);

  node->variable.isint = 0;

  PRTREE3@T@NODE_LEAF(node).point = 0;

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
  VsgPRTree3@t@Node *node = _prtree3@t@node_alloc (lbound, ubound, config);
  vsgloc3 i;
  VsgPRTree3@t@Node *children[8];

  for (i=0; i<8; i++)
    {
      VsgVector3@t@ lbound, ubound;

      if ((child == NULL) || (i != loc))
        {
          _prtree3@t@node_child_get_bounds (node, i, &lbound, &ubound);
          children[i] = _leaf_alloc (&lbound, &ubound, config);
        }
      else
        {
          children[i] = child;

          node->point_count += child->point_count;
          node->region_count += child->region_count;
        }
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

static void _prtree3@t@node_make_int (VsgPRTree3@t@Node *node,
                                      const VsgPRTree3@t@Config *config);

static guint
_prtree3@t@node_insert_point_list(VsgPRTree3@t@Node *node,
                                  GSList *point,
                                  const VsgPRTree3@t@Config *config);

static guint
_prtree3@t@node_insert_region_list(VsgPRTree3@t@Node *node,
                                   GSList *region_list,
                                   const VsgPRTree3@t@Config *config);

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
          _prtree3@t@node_make_int (node, config);
          _prtree3@t@node_insert_point_list (node, point, config);
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

static guint
_prtree3@t@node_insert_point_list(VsgPRTree3@t@Node *node,
                                  GSList *point,
                                  const VsgPRTree3@t@Config *config)
{
  guint len = 0;

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
            _prtree3@t@node_insert_point_list(PRTREE3@T@NODE_CHILD (node, i),
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

  _prtree3@t@node_insert_point_list(node, point_list, config);
}

static void _prtree3@t@node_make_int (VsgPRTree3@t@Node *node,
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

      children[i] = _leaf_alloc (&lbound, &ubound, config);
    }

  /* It is very important that new leaves are inserted once all calls to
   * _leaf_alloc are done, leaving the tree state consistent, because
   * certain language bindings do weird things with the tree when node_data
   * are duplicated. For example, Python and its cyclic garbage collector.
   */
  memcpy (PRTREE3@T@NODE_INT (node).children, children,
          8*sizeof (VsgPRTree3@t@Node*));

  _prtree3@t@node_insert_point_list(node, stolen_point, config);
  _prtree3@t@node_insert_region_list(node, stolen_region, config);
      
}

static void _prtree3@t@node_free (VsgPRTree3@t@Node *node,
                                  const VsgPRTree3@t@Config *config)
{
  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      g_slist_free (PRTREE3@T@NODE_LEAF (node).point);

    }
  else
    {
      vsgloc3 i;

      for (i=0; i<8; i++)
        _prtree3@t@node_free (PRTREE3@T@NODE_CHILD (node, i), config);
    }

  g_slist_free (node->region_list);

  _prtree3@t@node_dealloc (node, config);
}

static guint _prtree3@t@node_depth (const VsgPRTree3@t@Node *node)
{
  guint res = 0;
  vsgloc3 i;

  if (PRTREE3@T@NODE_ISLEAF (node)) return 0;

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
      _prtree3@t@node_free (PRTREE3@T@NODE_CHILD (node, i), config);
      PRTREE3@T@NODE_CHILD (node, i) = NULL;
    }

  _prtree3@t@node_insert_point_list (node, point, config);
  _prtree3@t@node_insert_region_list (node, region, config);
}

static gboolean
_prtree3@t@node_remove_point (VsgPRTree3@t@Node *node,
                              VsgPoint3 point,
                              const VsgPRTree3@t@Config *config)
{
  gboolean ret = FALSE;

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

      ret =
        _prtree3@t@node_remove_point (PRTREE3@T@NODE_CHILD (node, i),
                                      point, config);

      if (ret)
        {
          node->point_count --;
          if (node->point_count <= config->max_point)
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

static void _prtree3@t@node_write (const VsgPRTree3@t@Node *node, FILE *file,
                                   guint tab)
{
  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      _wtabs (file, tab);
      fprintf (file, "leaf[(%@tcode@,%@tcode@,%@tcode@)"
               " (%@tcode@,%@tcode@,%@tcode@)] point=%d region=%d\n",
               node->lbound.x, node->lbound.y, node->lbound.z,
               node->ubound.x, node->ubound.y, node->ubound.z,
               node->point_count,
               node->region_count);
    }
  else
    {
      vsgloc3 i;

      _wtabs (file, tab);
      fprintf (file, "int[(%@tcode@,%@tcode@,%@tcode@)"
               " (%@tcode@,%@tcode@,%@tcode@)] point=%d region=%d {\n",
               node->lbound.x, node->lbound.y, node->lbound.z,
               node->ubound.x, node->ubound.y, node->ubound.z,
               node->point_count,
               node->region_count);

      for (i=0; i<8; i++)
        {
          _prtree3@t@node_write (PRTREE3@T@NODE_CHILD (node, i), file,
                                 tab+1);
        }
      _wtabs (file, tab); fprintf (file, "} local region=%d depth=%u\n",
                                   g_slist_length (node->region_list),
                                   _prtree3@t@node_depth(node));
    }
}

static VsgPoint3
_prtree3@t@node_find_point (VsgPRTree3@t@Node *node,
                            VsgPoint3 selector,
                            const VsgPRTree3@t@Config *config)
{
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

static void _prtree3@t@node_foreach_point (VsgPRTree3@t@Node *node,
                                           GFunc func,
                                           gpointer user_data)
{
  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      g_slist_foreach (PRTREE3@T@NODE_LEAF (node).point, func,
                       user_data);
    }
  else
    {
      vsgloc3 i;
      
      for (i=0; i<8; i++)
        {
          _prtree3@t@node_foreach_point (PRTREE3@T@NODE_CHILD (node, i),
                                         func, user_data);
        }
    }
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
                                    const VsgPRTree3@t@Config *config)
{
  guint len = g_slist_length (region_list);

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
              node->region_list = g_slist_concat (current, node->region_list);
            }
          else
            {
              vsgloc3 i;
              vsgrloc3 ipow;
              
              for (i=0; i<8; i++)
                {
                  ipow =  VSG_RLOC3_COMP (i);
                  if (locmask & ipow)
                    _prtree3@t@node_insert_region_list (PRTREE3@T@NODE_CHILD (node,
                                                                              i),
                                                        current,
                                                        config);
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
                               const VsgPRTree3@t@Config *config)
{
  GSList *region_list = g_slist_alloc ();

  region_list->data = (gpointer) region;
  region_list->next = NULL;

  _prtree3@t@node_insert_region_list (node, region_list, config);
}

static gboolean
_prtree3@t@node_remove_region (VsgPRTree3@t@Node *node,
                               VsgRegion3 region,
                               const VsgPRTree3@t@Config *config)
{
  gboolean ret = FALSE;

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
              ipow =  VSG_RLOC3_COMP (i);
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

void _prtree3@t@node_update_user_data_type (VsgPRTree3@t@Node *node,
                                            GType new_type,
                                            gpointer new_model,
                                            const VsgPRTree3@t@Config *config)
{
  if (config->user_data_type != G_TYPE_NONE)
    {
      g_boxed_free (config->user_data_type, node->user_data);
    }

  if (new_type != G_TYPE_NONE)
    {
      node->user_data = g_boxed_copy (new_type, new_model);
    }
  else
    {
      node->user_data = NULL;
    }

  if (PRTREE3@T@NODE_ISINT (node))
    {
      vsgloc3 i;

      for (i=0; i<8; i++)
        {
          _prtree3@t@node_update_user_data_type
            (PRTREE3@T@NODE_CHILD (node, i),
             new_type, new_model, config);
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
_prtree3@t@node_traverse_custom (VsgPRTree3@t@Node *node,
                                 VsgPRTree3@t@NodeInfo *father_info,
                                 GTraverseType order,
                                 VsgRegion3 selector,
                                 VsgPRTree3@t@Func func,
                                 gpointer user_data,
                                 VsgPRTree3@t@Config *config)
{
  VsgPRTree3@t@NodeInfo node_info;
  guint8 i;
  vsgrloc3 ipow;
  vsgrloc3 locmask = (selector == NULL) ? VSG_RLOC3_MASK :
    CALL_REGION3@T@_LOC (config, selector, &node->center);

  _vsg_prtree3@t@node_get_info (node, &node_info, father_info);

  if (order == G_PRE_ORDER)
    func (&node_info, user_data);

  if (PRTREE3@T@NODE_ISINT (node))
    {
      for (i=0; i<4; i++)
	{
          ipow = VSG_RLOC3_COMP (i);

          if (ipow & locmask)
            _prtree3@t@node_traverse_custom (PRTREE3@T@NODE_CHILD(node, i),
                                             &node_info,
                                             order, selector, func, user_data,
                                             config);
	}
    }

  if (order == G_IN_ORDER)
    func (&node_info, user_data);

  if (PRTREE3@T@NODE_ISINT (node))
    {
      for (i=4; i<8; i++)
	{
          ipow = VSG_RLOC3_COMP (i);

          if (ipow & locmask)
            _prtree3@t@node_traverse_custom (PRTREE3@T@NODE_CHILD(node, i),
                                             &node_info,
                                             order, selector, func, user_data,
                                             config);
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

  while (region_list)
    {
      VsgRegion3 region = region_list->data;

      custom->func (region, custom->data);

      region_list = g_slist_next (region_list);
    }
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
 * The #GBoxed #GType associated to #VsgPrtree3@t@.
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
        g_boxed_type_register_static ("VsgPrtree3@t@",
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

  g_return_val_if_fail (lbound != NULL, NULL);
  g_return_val_if_fail (ubound != NULL, NULL);
  g_return_val_if_fail (point_locfunc != NULL, NULL);

  prtree3@t@ = _prtree3@t@_alloc ();

  prtree3@t@->node = _leaf_alloc(lbound, ubound, &prtree3@t@->config);

  prtree3@t@->config.point_loc_marshall = NULL;
  prtree3@t@->config.point_loc_data = point_locfunc;

  prtree3@t@->config.point_dist_marshall = NULL;
  prtree3@t@->config.point_dist_data = point_distfunc;

  prtree3@t@->config.region_loc_marshall = NULL;
  prtree3@t@->config.region_loc_data = region_locfunc;
  prtree3@t@->config.tolerance = @epsilon@;

  if (max_point == 0)
    max_point = PRTREE3@T@LEAF_MAXSIZE;
  prtree3@t@->config.max_point = max_point;

  return prtree3@t@;
}

/**
 * vsg_prtree3@t@_new:
 * @lbound: lower limit for coordinates of #VsgPoint3 to be stored
 * @ubound: upper limit for coordinates of #VsgPoint3 to be stored
 * @region_locfunc: the #VsgRegion3 localization function
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

  _prtree3@t@node_free (prtree3@t@->node, &prtree3@t@->config);

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

  res->config.user_data_type = prtree3@t@->config.user_data_type;

  if (res->config.user_data_type != G_TYPE_NONE)
    res->config.user_data_model =
      g_boxed_copy (res->config.user_data_type,
                    prtree3@t@->config.user_data_model);

  res->config.point_loc_marshall = prtree3@t@->config.point_loc_marshall;
  res->config.point_loc_data = prtree3@t@->config.point_loc_data;

  res->config.point_dist_marshall =
    prtree3@t@->config.point_dist_marshall;
  res->config.point_dist_data = prtree3@t@->config.point_dist_data;

  res->config.region_loc_marshall = prtree3@t@->config.region_loc_marshall;
  res->config.region_loc_data = prtree3@t@->config.region_loc_data;
  res->config.tolerance = prtree3@t@->config.tolerance;

  res->config.max_point = prtree3@t@->config.max_point;

  res->node = _leaf_alloc(&prtree3@t@->node->lbound,
                          &prtree3@t@->node->ubound,
                          &res->config);

  vsg_prtree3@t@_foreach_point (prtree3@t@, (GFunc) _copy_point, res);

  vsg_prtree3@t@_foreach_region (prtree3@t@, (GFunc) _copy_region, res);

  vsg_prtree3@t@_set_node_data (res,
                                prtree3@t@->config.user_data_type,
                                prtree3@t@->config.user_data_model);

  return res;
}

/**
 * vsg_prtree3@t@_set_point_loc_marshall:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @marshall: the function marshaller or %NULL.
 * @locdata: the data passed to @marshall or the function for direct loc test.
 *
 * Configure @prtree3@t@ for marshalling its VsgPoint3@t@ localization.
 */
void
vsg_prtree3@t@_set_point_loc_marshall (VsgPRTree3@t@ *prtree3@t@,
                                       VsgPoint3@t@LocMarshall marshall,
                                       gpointer locdata)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (marshall != NULL || locdata != NULL);
#endif

  prtree3@t@->config.point_loc_marshall = marshall;

  prtree3@t@->config.point_loc_data = locdata;
}

/**
 * vsg_prtree3@t@_set_point_loc_marshall:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @marshall: the function marshaller or %NULL.
 * @locdata: the data passed to @marshall or the function for direct loc test.
 *
 * Configure @prtree3@t@ for marshalling its VsgRegion3@t@ localization.
 */
void vsg_prtree3@t@_set_region_loc_marshall (VsgPRTree3@t@ *prtree3@t@,
                                             VsgRegion3@t@LocMarshall marshall,
                                             gpointer locdata)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (marshall != NULL || locdata != NULL);
#endif

  prtree3@t@->config.region_loc_marshall = marshall;

  prtree3@t@->config.region_loc_data = locdata;
}

/**
 * vsg_prtree3@t@_set_point_loc_marshall:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @marshall: the function marshaller or %NULL.
 * @locdata: the data passed to @marshall or the function for direct dist test.
 *
 * Configure @prtree3@t@ for marshalling its VsgRegion3@t@ distance evaluation.
 */
void
vsg_prtree3@t@_set_point_dist_marshall (VsgPRTree3@t@ *prtree3@t@,
                                        VsgPoint3@t@DistMarshall marshall,
                                        gpointer distdata)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (marshall != NULL || distdata != NULL);
#endif

  prtree3@t@->config.point_dist_marshall = marshall;

  prtree3@t@->config.point_dist_data = distdata;
}

/**
 * vsg_prtree3@t@_get_tolerance:
 * @prtree3@t@: a #VsgPRTree3@t@
 *
 * Read access to current @prtree3@t@ spatial tolerance.
 *
 * Returns @prtree3@t@ spatial tolerance.
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
  g_return_if_fail (tolerance >= 0.);
#endif

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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
#endif

  if (user_data_type != G_TYPE_NONE)
    {
      g_return_if_fail (g_type_is_a (user_data_type, G_TYPE_BOXED));
      g_return_if_fail (user_data_model != NULL);

      user_data_model = g_boxed_copy (user_data_type, user_data_model);
    }
  else
    {
      user_data_model = NULL;
    }

  _prtree3@t@node_update_user_data_type (prtree3@t@->node,
                                         user_data_type,
                                         user_data_model,
                                         &prtree3@t@->config);

  if (prtree3@t@->config.user_data_type != G_TYPE_NONE)
    {
      g_boxed_free (prtree3@t@->config.user_data_type,
                    prtree3@t@->config.user_data_model);

      prtree3@t@->config.user_data_type = G_TYPE_NONE;
    }

  prtree3@t@->config.user_data_type = user_data_type;
  prtree3@t@->config.user_data_model = user_data_model;
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
 * vsg_prtree3@t@_point_count
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
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, 0);
#endif

  return _prtree3@t@node_region_count (prtree3@t@->node);
}

/**
 * vsg_prtree3@t@_insert_point
 * @prtree3@t@: a #VsgPRTree3@t@
 * @point: a #VsgPoint3
 *
 * Stores @point reference in prtree3@t@. (#GObject reference support not
 * provided)
 */
void vsg_prtree3@t@_insert_point (VsgPRTree3@t@ *prtree3@t@,
                                  VsgPoint3 point)
{
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
      prtree3@t@->node = _int_alloc (&new_lbound, &new_ubound,
                                     prtree3@t@->node, loc, config);

      vsg_prtree3@t@_insert_point (prtree3@t@, point);
    }
  else
    _prtree3@t@node_insert_point (prtree3@t@->node, point, config);
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
void vsg_prtree3@t@_write (const VsgPRTree3@t@ *prtree3@t@, FILE *file)
{
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (file != NULL);

  _prtree3@t@node_write (prtree3@t@->node, file, 0);
}

/**
 * vsg_prtree3@t@_print:
 * @prtree3@t@: a #VsgPRTree3@t@
 *
 * Prints a summary of @prtree3@t@ on standard output. Mainly used for debugging.
 */
void vsg_prtree3@t@_print (const VsgPRTree3@t@ *prtree3@t@)
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
 * Returns the first #VsgPoint3 found or NULL.
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
  g_return_if_fail (region != NULL);
#endif

  _prtree3@t@node_insert_region (prtree3@t@->node, region,
                                 &prtree3@t@->config);
}

/**
 * vsg_prtree3@t@_remove_region:
 * @prtree3@t@: a #VsgPRTree3@t@
 * @region: a #VsgRegion
 *
 * Removes specified @region from @prtree3@t@ lists.
 *
 * Returns %TRUE if @region was in @prtree3@t@, %FALSE if not.
 */
gboolean vsg_prtree3@t@_remove_region (VsgPRTree3@t@ *prtree3@t@,
                                       VsgRegion3 region)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree3@t@ != NULL, FALSE);
  g_return_val_if_fail (region != NULL, FALSE);
#endif

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
 * @user_data: a 3gpointer
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

  return _prtree3@t@node_find_deep_region (prtree3@t@->node, point,
                                           check,
                                           user_data,
                                           &prtree3@t@->config);
}

/**
 * vsg_prtree3@t@_traverse:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @order: traverse order.
 * @func: a #VsgPrtree3@t@Func.
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
 * @func: a #VsgPrtree3@t@Func.
 * @user_data: a #gpointer data to be apssed to @func.
 *
 * Performs a traversal of @prtree3@t@, executing @func on each of its nodes
 * that intersects @selector (as defined by provided region_locfunc in
 * vsg_prtree3@t@_new_full()). Tree nodes are visited in @order order.
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

  _prtree3@t@node_traverse_custom (prtree3@t@->node, NULL,
                                   order, selector, func,
                                   user_data, &prtree3@t@->config);
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
