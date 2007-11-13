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

#include "vsgprtree2@t@.h"

#include "vsgprtree2@t@-private.h"

#include <string.h>

#define VSG_PRTREE2@T@_PREALLOC 32
#define PRTREE2@T@NODE_PREALLOC 256

#define PRTREE2@T@LEAF_MAXSIZE 5

/* Nodes allocation */

static GMemChunk *vsg_prtree2@t@_mem_chunk = 0;
static GMemChunk *vsg_prtree2@t@node_mem_chunk = 0;
static guint vsg_prtree2@t@_instances_count = 0;

static void _prtree2@t@_finalize ()
{
  if (vsg_prtree2@t@_mem_chunk)
    {
      g_mem_chunk_destroy (vsg_prtree2@t@_mem_chunk);
      vsg_prtree2@t@_mem_chunk = 0;
      vsg_prtree2@t@_instances_count = 0;
    }
  if (vsg_prtree2@t@node_mem_chunk)
    {
      g_mem_chunk_destroy (vsg_prtree2@t@node_mem_chunk);
      vsg_prtree2@t@node_mem_chunk = 0;
    }
}

void vsg_prtree2@t@_init()
{
  g_atexit (_prtree2@t@_finalize);
}

static VsgPRTree2@t@ *_prtree2@t@_alloc ()
{
  VsgPRTree2@t@ *ret;

  if (!vsg_prtree2@t@_mem_chunk)
    {
      vsg_prtree2@t@_mem_chunk = g_mem_chunk_create (VsgPRTree2@t@,
                                                     VSG_PRTREE2@T@_PREALLOC,
                                                     G_ALLOC_ONLY);

      if (!vsg_prtree2@t@node_mem_chunk)
        vsg_prtree2@t@node_mem_chunk =
          g_mem_chunk_create (VsgPRTree2@t@Node,
                              PRTREE2@T@NODE_PREALLOC,
                              G_ALLOC_ONLY);
    }

  vsg_prtree2@t@_instances_count ++;

  ret = g_chunk_new (VsgPRTree2@t@, vsg_prtree2@t@_mem_chunk);

  ret->config.user_data_type = G_TYPE_NONE;
  ret->config.user_data_model = NULL;

  return ret;
}

static void _prtree2@t@_dealloc (VsgPRTree2@t@ *prtree2@t@)
{
  if (prtree2@t@->config.user_data_type != G_TYPE_NONE)
    {
      g_boxed_free (prtree2@t@->config.user_data_type,
                    prtree2@t@->config.user_data_model);
    }

  g_chunk_free (prtree2@t@, vsg_prtree2@t@_mem_chunk);
  vsg_prtree2@t@_instances_count --;

  if (vsg_prtree2@t@_instances_count == 0)
    {
      _prtree2@t@_finalize ();
    }
}

static
VsgPRTree2@t@Node *_prtree2@t@node_alloc (const VsgVector2@t@ *lbound,
                                          const VsgVector2@t@ *ubound,
                                          const VsgPRTree2@t@Config *config)
{
  VsgPRTree2@t@Node *ret;
  ret = g_chunk_new (VsgPRTree2@t@Node, vsg_prtree2@t@node_mem_chunk);

  ret->region_list = NULL;

  ret->point_count = 0;
  ret->region_count = 0;

  vsg_vector2@t@_copy (lbound, &ret->lbound);
  vsg_vector2@t@_copy (ubound, &ret->ubound);

  vsg_vector2@t@_lerp (lbound, ubound, 0.5, &ret->center);

  if (config->user_data_type != G_TYPE_NONE)
    {
      ret->user_data = g_boxed_copy (config->user_data_type,
                                     config->user_data_model);
    }

  return ret;
}

static void _prtree2@t@node_dealloc (VsgPRTree2@t@Node *prtree2@t@node,
                                     const VsgPRTree2@t@Config *config)
{
  if (config->user_data_type != G_TYPE_NONE)
    {
      g_boxed_free (config->user_data_type,
                    prtree2@t@node->user_data);
    }

  g_chunk_free (prtree2@t@node, vsg_prtree2@t@node_mem_chunk);
}

static VsgPRTree2@t@Node *_leaf_alloc (const VsgVector2@t@ *lbound,
                                       const VsgVector2@t@ *ubound,
                                       const VsgPRTree2@t@Config *config)
{
  VsgPRTree2@t@Node *node = _prtree2@t@node_alloc (lbound, ubound, config);

  node->variable.isint = 0;

  PRTREE2@T@NODE_LEAF(node).point = 0;

  return node;
}

static void _prtree2@t@node_child_get_bounds (const VsgPRTree2@t@Node *node,
                                              vsgloc2 loc,
                                              VsgVector2@t@ *lbound,
                                              VsgVector2@t@ *ubound)
{
  vsgloc2 xcomp = VSG_LOC2_ALONG_X (loc);
  vsgloc2 ycomp = VSG_LOC2_ALONG_Y (loc);

  const VsgVector2@t@ *center = &node->center;

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
}

static VsgPRTree2@t@Node *_int_alloc (const VsgVector2@t@ *lbound,
                                      const VsgVector2@t@ *ubound,
                                      VsgPRTree2@t@Node *child,
                                      vsgloc2 loc,
                                      const VsgPRTree2@t@Config *config)
{
  VsgPRTree2@t@Node *node = _prtree2@t@node_alloc (lbound, ubound, config);
  vsgloc2 i;
  VsgPRTree2@t@Node *children[4];

  for (i=0; i<4; i++)
    {
      VsgVector2@t@ lbound, ubound;

      if ((child == NULL) || (i != loc))
        {
          _prtree2@t@node_child_get_bounds (node, i, &lbound, &ubound);
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
  memcpy (PRTREE2@T@NODE_INT (node).children, children,
          4*sizeof (VsgPRTree2@t@Node*));

  return node;
}

static guint _prtree2@t@node_point_count (const VsgPRTree2@t@Node *node)
{
  /*   guint result = 0; */
  /*   if (PRTREE2@T@NODE_ISLEAF(node)) */
  /*     { */
  /*       result = g_slist_length (PRTREE2@T@NODE_LEAF(node).point); */
  /*     } */
  /*   else */
  /*     { */
  /*       vsgloc2 i; */

  /*       for (i=0; i<4; i++) */
  /*         result += _prtree2@t@node_point_count (PRTREE2@T@NODE_CHILD (node, i)); */
  /*     } */

  /*   return result; */

  return node->point_count;
}

static guint _prtree2@t@node_region_count (const VsgPRTree2@t@Node *node)
{
  /*   guint result = 0; */
  /*   if (PRTREE2@T@NODE_ISINT(node)) */
  /*     { */
  /*       vsgloc2 i; */

  /*       for (i=0; i<4; i++) */
  /*         result += */
  /*           _prtree2@t@node_region_count (PRTREE2@T@NODE_CHILD (node, i)); */
  /*     } */

  /*   result += g_slist_length (node->region_list); */

  /*   return result; */

  return node->region_count;
}


static GSList *_prtree2@t@node_steal_point (VsgPRTree2@t@Node *node)
{
  GSList *result = NULL;

  if (PRTREE2@T@NODE_ISLEAF(node))
    {
      VsgPRTree2@t@Leaf *leaf_ = &PRTREE2@T@NODE_LEAF(node);

      result = leaf_->point;
      leaf_->point = NULL;
      node->point_count = 0;
    }
  else
    {
      vsgloc2 i;

      for (i=0; i<4; i++)
        {
          GSList *stolen;

          node->point_count -= PRTREE2@T@NODE_CHILD (node, i)->point_count;

          stolen =
            _prtree2@t@node_steal_point (PRTREE2@T@NODE_CHILD (node, i));

          result = g_slist_concat (result, stolen);
        }
    }

  return result;
}

static GSList *_prtree2@t@node_steal_region (VsgPRTree2@t@Node *node)
{
  GSList *result = NULL;

  if (PRTREE2@T@NODE_ISLEAF(node))
    {
      result = node->region_list;
      node->region_list = NULL;
      node->region_count = 0;
    }
  else
    {
      vsgloc2 i;

      for (i=0; i<4; i++)
        {
          GSList *stolen;

          node->region_count -= PRTREE2@T@NODE_CHILD (node, i)->region_count;

          stolen =
            _prtree2@t@node_steal_region (PRTREE2@T@NODE_CHILD (node, i));

          result = g_slist_concat (result, stolen);
        }
    }

  return result;
}

static void _prtree2@t@node_make_int (VsgPRTree2@t@Node *node,
                                      const VsgPRTree2@t@Config *config);

static guint
_prtree2@t@node_insert_point_list(VsgPRTree2@t@Node *node,
                                  GSList *point,
                                  const VsgPRTree2@t@Config *config);
static guint
_prtree2@t@node_insert_region_list (VsgPRTree2@t@Node *node,
                                    GSList *region_list,
                                    const VsgPRTree2@t@Config *config);

static gboolean _check_point_dist (VsgPRTree2@t@Leaf *node,
                                   GSList *point,
                                   const VsgPRTree2@t@Config *config)
{
  /*
   * We check the distance from inserted point to first of already stored
   * point. If the distance is zero, we may have problems in the future,
   * eventually leading to infinite recursion on leaf subdivision. We then
   * choose to abort insertion.
   */

  if (CALL_POINT2@T@_DIST (config,
                           (VsgPoint2) point->data,
                           (VsgPoint2) node->point->data) <=
      config->tolerance)
    {
      g_warning ("detected multiple insertion of a same Point2."
                 " Aborting insertion.\n");
      g_slist_free (point);
      return FALSE;
    }

  return TRUE;
}

static guint
_prtree2@t@leaf_insert_point_list(VsgPRTree2@t@Node *node,
                                  GSList *point,
                                  const VsgPRTree2@t@Config *config)
{
  g_assert (point != NULL);
  g_assert (point->next == NULL);

  if (node->point_count+1 > config->max_point)
    {
      if (_check_point_dist (&PRTREE2@T@NODE_LEAF (node),
                             point, config))
        {
          _prtree2@t@node_make_int (node, config);
          _prtree2@t@node_insert_point_list (node, point, config);

        }
      else
        return 0;
    }
  else
    {
      PRTREE2@T@NODE_LEAF (node).point =
        g_slist_concat (point, PRTREE2@T@NODE_LEAF (node).point);
      node->point_count ++;
    }

  return 1;
}

static guint
_prtree2@t@node_insert_point_list(VsgPRTree2@t@Node *node,
                                  GSList *point,
                                  const VsgPRTree2@t@Config *config)
{
  guint len = 0;

  if (PRTREE2@T@NODE_ISLEAF (node))
    {
      while (point)
        {
          GSList *current;

          current = point;
          point = g_slist_next (current);
          current->next = NULL;

          len += _prtree2@t@leaf_insert_point_list (node, current, config);
        }
    }
  else
    {
      while (point)
        {
          GSList *current;
          vsgloc2 i;

          current = point;
          point = g_slist_next (current);
          current->next = NULL;

          i = CALL_POINT2@T@_LOC (config, current->data, &node->center);

          len +=
            _prtree2@t@node_insert_point_list(PRTREE2@T@NODE_CHILD (node, i),
                                              current, config);
        }

      node->point_count += len;
    }

  return len;
}

static void _prtree2@t@node_insert_point(VsgPRTree2@t@Node *node,
                                         VsgPoint2 point,
                                         const VsgPRTree2@t@Config *config)
{
  GSList *point_list = g_slist_alloc();

  point_list->data = (gpointer) point;

  _prtree2@t@node_insert_point_list(node, point_list, config);
}

static void _prtree2@t@node_make_int (VsgPRTree2@t@Node *node,
                                      const VsgPRTree2@t@Config *config)
{
  GSList *stolen_point;
  GSList *stolen_region;
  vsgloc2 i;
  VsgPRTree2@t@Node *children[4];

  g_return_if_fail (PRTREE2@T@NODE_ISLEAF(node));

  stolen_point = _prtree2@t@node_steal_point (node);
  stolen_region = _prtree2@t@node_steal_region (node);

  for (i=0; i<4; i++)
    {
      VsgVector2@t@ lbound, ubound;

      _prtree2@t@node_child_get_bounds (node, i, &lbound, &ubound);

      children[i] = _leaf_alloc (&lbound, &ubound, config);
    }

  /* It is very important that new leaves are inserted once all calls to
   * _leaf_alloc are done, leaving the tree state consistent, because
   * certain language bindings do weird things with the tree when node_data
   * are duplicated. For example, Python and its cyclic garbage collector.
   */
  memcpy (PRTREE2@T@NODE_INT (node).children, children,
          4*sizeof (VsgPRTree2@t@Node*));

  _prtree2@t@node_insert_point_list(node, stolen_point, config);
  _prtree2@t@node_insert_region_list(node, stolen_region, config);
      
}

static void _prtree2@t@node_free (VsgPRTree2@t@Node *node,
                                  const VsgPRTree2@t@Config *config)
{
  if (PRTREE2@T@NODE_ISLEAF (node))
    {
      g_slist_free (PRTREE2@T@NODE_LEAF (node).point);

    }
  else
    {
      vsgloc2 i;

      for (i=0; i<4; i++)
        _prtree2@t@node_free (PRTREE2@T@NODE_CHILD (node, i), config);
    }

  g_slist_free (node->region_list);

  _prtree2@t@node_dealloc (node, config);
}

static guint _prtree2@t@node_depth (const VsgPRTree2@t@Node *node)
{
  guint res = 0;
  vsgloc2 i;

  if (PRTREE2@T@NODE_ISLEAF (node)) return 0;

  for (i=0; i<4; i++)
    {
      guint tmp = _prtree2@t@node_depth (PRTREE2@T@NODE_CHILD (node, i));
      if (tmp > res) res = tmp;
    }

  return res + 1;
}

static void _prtree2@t@node_flatten (VsgPRTree2@t@Node *node,
                                     const VsgPRTree2@t@Config *config)
{
  GSList *point = NULL;
  GSList *region = NULL;
  vsgloc2 i;

  g_return_if_fail (PRTREE2@T@NODE_ISINT (node));

  point = _prtree2@t@node_steal_point (node);
  region = _prtree2@t@node_steal_region (node);

  for (i=0; i<4; i++)
    {
      _prtree2@t@node_free (PRTREE2@T@NODE_CHILD (node, i), config);
      PRTREE2@T@NODE_CHILD (node, i) = NULL;
    }

  _prtree2@t@node_insert_point_list (node, point, config);
  _prtree2@t@node_insert_region_list (node, region, config);
}

static gboolean
_prtree2@t@node_remove_point (VsgPRTree2@t@Node *node,
                              VsgPoint2 point,
                              const VsgPRTree2@t@Config *config)
{
  gboolean ret = FALSE;
  if (PRTREE2@T@NODE_ISLEAF (node))
    {
      ret = g_slist_find (PRTREE2@T@NODE_LEAF (node).point,
                          point) != NULL;
      if (ret)
        {
          PRTREE2@T@NODE_LEAF (node).point =
            g_slist_remove (PRTREE2@T@NODE_LEAF (node).point, point);
          node->point_count --;
        }
    }
  else
    {
      vsgloc2 i = CALL_POINT2@T@_LOC (config, point, &node->center);

      ret = _prtree2@t@node_remove_point (PRTREE2@T@NODE_CHILD (node, i),
                                          point, config);

      if (ret)
        {
          node->point_count --;
          if (node->point_count <= config->max_point)
            _prtree2@t@node_flatten (node, config);
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

static void _prtree2@t@node_write (const VsgPRTree2@t@Node *node, FILE *file,
                                   guint tab)
{
  if (PRTREE2@T@NODE_ISLEAF (node))
    {
      _wtabs (file, tab);
      fprintf (file,
               "leaf[(%@tcode@,%@tcode@) (%@tcode@,%@tcode@)]"
               " point=%d region=%d\n",
               node->lbound.x, node->lbound.y,
               node->ubound.x, node->ubound.y,
               node->point_count,
               node->region_count);
    }
  else
    {
      vsgloc2 i;

      _wtabs (file, tab);
      fprintf (file,
               "int[(%@tcode@,%@tcode@) (%@tcode@,%@tcode@)]"
               " point=%d region=%d {\n",
               node->lbound.x, node->lbound.y,
               node->ubound.x, node->ubound.y,
               node->point_count,
               node->region_count);

      for (i=0; i<4; i++)
        {
          _prtree2@t@node_write (PRTREE2@T@NODE_CHILD (node, i), file,
                                 tab+1);
        }
      _wtabs (file, tab); fprintf (file, "} local region=%d depth=%u\n",
                                   g_slist_length (node->region_list),
                                   _prtree2@t@node_depth(node));
    }
}

static VsgPoint2
_prtree2@t@node_find_point (VsgPRTree2@t@Node *node,
                            VsgPoint2 selector,
                            const VsgPRTree2@t@Config *config)
{
  if (PRTREE2@T@NODE_ISLEAF (node))
    {
      GSList *point = PRTREE2@T@NODE_LEAF (node).point;

      while (point)
        {
          if (CALL_POINT2@T@_DIST (config, selector,
                                   (VsgPoint2) point->data) <=
              config->tolerance)
            {
              return point->data;
            }
          point = g_slist_next (point);
        }
    }
  else
    {
      vsgloc2 i = CALL_POINT2@T@_LOC (config, selector, &node->center);

      return
        _prtree2@t@node_find_point (PRTREE2@T@NODE_CHILD (node, i),
                                    selector, config);
    }
  return NULL;
}

static vsgrloc2 _region2_point2_locfunc_always (const gpointer region,
                                                const VsgPoint2 point)
{
  return VSG_RLOC2_MASK;
}

static gboolean _share_region (vsgrloc2 locmask)
{
  vsgloc2 i;
  vsgrloc2 ipow;
  guint count = 0;

  for (i=0; i<4; i++)
    {
      ipow = VSG_RLOC2_COMP (i);
      if (ipow & locmask)
        count++;
    }

  return count != 1;
}

static guint
_prtree2@t@node_insert_region_list (VsgPRTree2@t@Node *node,
                                    GSList *region_list,
                                    const VsgPRTree2@t@Config *config)
{
  guint len = g_slist_length (region_list);

  if (PRTREE2@T@NODE_ISLEAF (node))
    {
      node->region_list = g_slist_concat (region_list, node->region_list);
    }
  else
    {
      while (region_list)
        {
          GSList *current;
          vsgrloc2 locmask;

          current = region_list;
          region_list = g_slist_next (current);
          current->next = NULL;

          locmask = CALL_REGION2@T@_LOC (config, current->data, &node->center);

          if (_share_region(locmask))
            {
              node->region_list =
                g_slist_concat (current, node->region_list);
            }
          else
            {
              vsgloc2 i;
              vsgrloc2 ipow;
              
              for (i=0; i<4; i++)
                {
                  ipow =  VSG_RLOC2_COMP (i);
                  if (locmask & ipow)
                    _prtree2@t@node_insert_region_list (PRTREE2@T@NODE_CHILD (node, i),
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
_prtree2@t@node_insert_region (VsgPRTree2@t@Node *node,
                               VsgRegion2 region,
                               const VsgPRTree2@t@Config *config)
{
  GSList *region_list = g_slist_alloc ();

  region_list->data = (gpointer) region;
  region_list->next = NULL;

  _prtree2@t@node_insert_region_list (node, region_list, config);
}

static gboolean
_prtree2@t@node_remove_region (VsgPRTree2@t@Node *node,
                               VsgRegion2 region,
                               const VsgPRTree2@t@Config *config)
{
  gboolean ret = FALSE;

  if (PRTREE2@T@NODE_ISLEAF (node))
    {
      ret = g_slist_find (node->region_list, region) != NULL;
      if (ret)
        node->region_list = g_slist_remove (node->region_list, region);
    }
  else
    {
      vsgrloc2 locmask = CALL_REGION2@T@_LOC (config, region, &node->center);

      if (_share_region(locmask))
        {
          ret = g_slist_find (node->region_list, region) != NULL;

          if (ret)
            node->region_list = g_slist_remove (node->region_list, region);
        }
      else
        {
          vsgloc2 i;
          vsgrloc2 ipow;
              
          for (i=0; i<4; i++)
            {
              ipow = VSG_RLOC2_COMP (i);
              if (locmask & ipow)
                {
                  ret =
                    _prtree2@t@node_remove_region (PRTREE2@T@NODE_CHILD (node, i),
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

static VsgRegion2
_prtree2@t@node_find_deep_region (VsgPRTree2@t@Node *node,
                                  VsgPoint2 point,
                                  VsgRegion2CheckFunc check,
                                  gpointer user_data,
                                  const VsgPRTree2@t@Config *config)
{
  VsgRegion2 result = NULL;

  if (PRTREE2@T@NODE_ISINT (node))
    {
      vsgloc2 i = 0;

      if (point)
        i = CALL_POINT2@T@_LOC (config, point, &node->center);

      result =
        _prtree2@t@node_find_deep_region (PRTREE2@T@NODE_CHILD (node, i),
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

void _prtree2@t@node_update_user_data_type (VsgPRTree2@t@Node *node,
                                            GType new_type,
                                            gpointer new_model,
                                            const VsgPRTree2@t@Config *config)
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

  if (PRTREE2@T@NODE_ISINT (node))
    {
      vsgloc2 i;

      for (i=0; i<4; i++)
        {
          _prtree2@t@node_update_user_data_type
            (PRTREE2@T@NODE_CHILD (node, i),
             new_type, new_model, config);
        }

    }
}

static void _copy_point (VsgPoint2 *point, VsgPRTree2@t@ *tree)
{
  vsg_prtree2@t@_insert_point (tree, point);
}

static void _copy_region (VsgRegion2 *region, VsgPRTree2@t@ *tree)
{
  vsg_prtree2@t@_insert_region (tree, region);
}

static void
_prtree2@t@node_traverse_custom (VsgPRTree2@t@Node *node,
                                 VsgPRTree2@t@NodeInfo *father_info,
                                 GTraverseType order,
                                 VsgRegion2 selector,
                                 VsgPRTree2@t@Func func,
                                 gpointer user_data,
                                 VsgPRTree2@t@Config *config,
                                 gpointer node_key)
{
  VsgPRTree2@t@NodeInfo node_info;
  guint8 i;
  vsgrloc2 ipow;
  vsgrloc2 locmask = (selector == NULL) ? VSG_RLOC2_MASK :
    CALL_REGION2@T@_LOC (config, selector, &node->center);

  gint children[4];
  gpointer children_keys[4];

  _vsg_prtree2@t@node_get_info (node, &node_info, father_info);

  if (order == G_PRE_ORDER)
    func (&node_info, user_data);

  if (PRTREE2@T@NODE_ISINT (node))
    {
      config->children_order (node_key, children, children_keys,
                              config->children_order_data);

      for (i=0; i<2; i++)
	{
          gint ic = children[i];

          ipow = VSG_RLOC2_COMP (ic);

          if (ipow & locmask)
            _prtree2@t@node_traverse_custom (PRTREE2@T@NODE_CHILD(node, ic),
                                             &node_info,
                                             order, selector, func, user_data,
                                             config, children_keys[i]);
	}
    }

  if (order == G_IN_ORDER)
    func (&node_info, user_data);

  if (PRTREE2@T@NODE_ISINT (node))
    {
      for (i=2; i<4; i++)
	{
          gint ic = children[i];

          ipow = VSG_RLOC2_COMP (ic);

          if (ipow & locmask)
            _prtree2@t@node_traverse_custom (PRTREE2@T@NODE_CHILD(node, ic),
                                             &node_info,
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
  VsgRegion2 selector;
  VsgRegion2Point2LocFunc locfunc;
};

static void _foreach_point_custom (const VsgPRTree2@t@NodeInfo *node_info,
                                   CustomForeach *custom)
{
  GSList *point_list = node_info->point_list;

  while (point_list)
    {
      VsgPoint2 point = point_list->data;

      if (custom->locfunc (custom->selector, point) == VSG_RLOC2_MASK)
        custom->func (point, custom->data);

      point_list = g_slist_next (point_list);
    }
}

static void _foreach_region_custom (const VsgPRTree2@t@NodeInfo *node_info,
                                    CustomForeach *custom)
{
  GSList *region_list = node_info->region_list;

  while (region_list)
    {
      VsgRegion2 region = region_list->data;

      custom->func (region, custom->data);

      region_list = g_slist_next (region_list);
    }
}

typedef struct _Foreach Foreach;

struct _Foreach {
  GFunc func;
  gpointer data;
};

static void _foreach_point (const VsgPRTree2@t@NodeInfo *node_info,
                            Foreach *custom)
{
  GSList *point_list = node_info->point_list;

  while (point_list)
    {
      VsgPoint2 point = point_list->data;

      custom->func (point, custom->data);

      point_list = g_slist_next (point_list);
    }
}

static void _foreach_region (const VsgPRTree2@t@NodeInfo *node_info,
                             Foreach *custom)
{
  GSList *region_list = node_info->region_list;

  while (region_list)
    {
      VsgRegion2 region = region_list->data;

      custom->func (region, custom->data);

      region_list = g_slist_next (region_list);
    }
}

static void
_z_order_data (gpointer key, gint *children, gpointer *children_keys,
               gpointer user_data)
{
  static const gint zorder[4] = {0, 1, 2, 3};

  memcpy (children, zorder, 4 * sizeof (gint));
}

/*-------------------------------------------------------------------*/
/* typedefs and structure doc */
/*-------------------------------------------------------------------*/
/**
 * VsgPRTree2@t@:
 *
 * Opaque structure. It must be manipulated only by the following functions.
 */

/**
 * VsgPoint2@t@LocFunc:
 * @candidate: a #VsgPoint2
 * @center: a #VsgVector2@t@
 *
 * Type of functions that perform #VsgPoint2 localization.
 *
 * Returns: localization quadrant (in #vsgloc2 form).
 */

/**
 * VsgRegion2@t@LocFunc:
 * @region: a #VsgRegion2
 * @center: a #VsgVector2@t@
 *
 * Type of functions that perform #VsgRegion2 localization.
 *
 * Returns: localization quadrant(s) (in #vsgrloc2 form).
 */

/**
 * VsgPoint2@t@DistFunc:
 * @one: a #VsgPoint2
 * @other: a #VsgPoint2
 *
 * Type of functions that compute #VsgPoint2 distance
 *
 * Returns: the distance between @one and @other.
 */

/**
 * VSG_TYPE_PRTREE2@T@:
 *
 * The #GBoxed #GType associated to #VsgPrtree2@t@.
 */

/*-------------------------------------------------------------------*/
/* public functions */
/*-------------------------------------------------------------------*/

GType vsg_prtree2@t@_get_type (void)
{
  static GType prtree2@t@_type = 0;

  if (!prtree2@t@_type)
    {
      prtree2@t@_type =
        g_boxed_type_register_static ("VsgPrtree2@t@",
                                      (GBoxedCopyFunc) vsg_prtree2@t@_clone,
                                      (GBoxedFreeFunc) vsg_prtree2@t@_free);

    }

  return prtree2@t@_type;
}

/**
 * vsg_prtree2@t@_new_full:
 * @lbound: lower limit for coordinates of #VsgPoint2 to be stored
 * @ubound: upper limit for coordinates of #VsgPoint2 to be stored
 * @point_locfunc: the #VsgPoint2 localization function
 * @point_distfunc: the #VsgPoint2 distance function
 * @region_locfunc: the #VsgRegion2 localization function
 * @max_point: maximum number of #VsgPoint to be stored in a VsgPRTree2@t@ Leaf
 *
 * Allocates a new instance of #VsgPRTree2@t@. Specifying a @lax_point value of 0
 * means taking library default.
 *
 * Returns: newly allocated #VsgPRTree2@t@
 */
VsgPRTree2@t@ *
vsg_prtree2@t@_new_full (const VsgVector2@t@ *lbound,
                         const VsgVector2@t@ *ubound,
                         const VsgPoint2@t@LocFunc point_locfunc,
                         const VsgPoint2@t@DistFunc point_distfunc,
                         const VsgRegion2@t@LocFunc region_locfunc,
                         guint max_point)
{
  VsgPRTree2@t@ *prtree2@t@;

  g_return_val_if_fail (lbound != NULL, NULL);
  g_return_val_if_fail (ubound != NULL, NULL);
  g_return_val_if_fail (point_locfunc != NULL, NULL);

  prtree2@t@ = _prtree2@t@_alloc ();

  prtree2@t@->node = _leaf_alloc(lbound, ubound, &prtree2@t@->config);

  prtree2@t@->config.point_loc_marshall = NULL;
  prtree2@t@->config.point_loc_data = point_locfunc;

  prtree2@t@->config.point_dist_marshall = NULL;
  prtree2@t@->config.point_dist_data = point_distfunc;

  prtree2@t@->config.region_loc_marshall = NULL;
  prtree2@t@->config.region_loc_data = region_locfunc;
  prtree2@t@->config.tolerance = @epsilon@;

  prtree2@t@->config.children_order = _z_order_data;
  prtree2@t@->config.children_order_data = NULL;
  prtree2@t@->config.root_key = NULL;

  if (max_point == 0)
    max_point = PRTREE2@T@LEAF_MAXSIZE;
  prtree2@t@->config.max_point = max_point;

  return prtree2@t@;
}

/**
 * vsg_prtree2@t@_new:
 * @lbound: lower limit for coordinates of #VsgPoint2 to be stored
 * @ubound: upper limit for coordinates of #VsgPoint2 to be stored
 * @region_locfunc: the #VsgRegion2 localization function
 *
 * Convenience macro that calls #vsg_prtree2@t@_new_full ().
 */

/**
 * vsg_prtree2@t@_free:
 * @prtree2@t@: a #VsgPRTree2@t@
 *
 * Frees @prtree2@t@.
 */
void vsg_prtree2@t@_free (VsgPRTree2@t@ *prtree2@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
#endif

  _prtree2@t@node_free (prtree2@t@->node, &prtree2@t@->config);

  _prtree2@t@_dealloc (prtree2@t@);
}

/**
 * vsg_prtree2@t@_clone:
 * @prtree2@t@: a #VsgPRTree2@t@
 *
 * Copies @prtree2@t@ into a newly allocated #VsgPRTree2@t@. All references
 * to #VsgPoint@2@ and #VsgRegion2 from @prtree2@t@ are copied into the
 * new tree.
 *
 * Returns: a copy of @prtree2@t@.
 */
VsgPRTree2@t@ *vsg_prtree2@t@_clone (VsgPRTree2@t@ *prtree2@t@)
{
  VsgPRTree2@t@ *res;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree2@t@ != NULL, NULL);
#endif

  res = _prtree2@t@_alloc ();

  res->config.user_data_type = prtree2@t@->config.user_data_type;

  if (res->config.user_data_type != G_TYPE_NONE)
    res->config.user_data_model =
      g_boxed_copy (res->config.user_data_type,
                    prtree2@t@->config.user_data_model);

  res->config.point_loc_marshall = prtree2@t@->config.point_loc_marshall;
  res->config.point_loc_data = prtree2@t@->config.point_loc_data;

  res->config.point_dist_marshall =
    prtree2@t@->config.point_dist_marshall;
  res->config.point_dist_data = prtree2@t@->config.point_dist_data;

  res->config.region_loc_marshall = prtree2@t@->config.region_loc_marshall;
  res->config.region_loc_data = prtree2@t@->config.region_loc_data;
  res->config.tolerance = prtree2@t@->config.tolerance;

  res->config.max_point = prtree2@t@->config.max_point;

  res->config.children_order = prtree2@t@->config.children_order;
  res->config.children_order_data = prtree2@t@->config.children_order_data;
  res->config.root_key = prtree2@t@->config.root_key;

  res->node = _leaf_alloc(&prtree2@t@->node->lbound,
                          &prtree2@t@->node->ubound,
                          &res->config);

  vsg_prtree2@t@_foreach_point (prtree2@t@, (GFunc) _copy_point, res);

  vsg_prtree2@t@_foreach_region (prtree2@t@, (GFunc) _copy_region, res);

  return res;
}

/**
 * vsg_prtree2@t@_set_point_loc_marshall:
 * @prtree2@t@: a #VsgPRTree2@t@.
 * @marshall: the function marshaller or %NULL.
 * @locdata: the data passed to @marshall or the function for direct loc test.
 *
 * Configure @prtree2@t@ for marshalling its VsgPoint2@t@ localization.
 */
void
vsg_prtree2@t@_set_point_loc_marshall (VsgPRTree2@t@ *prtree2@t@,
                                       VsgPoint2@t@LocMarshall marshall,
                                       gpointer locdata)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (marshall != NULL || locdata != NULL);
#endif

  prtree2@t@->config.point_loc_marshall = marshall;

  prtree2@t@->config.point_loc_data = locdata;
}

/**
 * vsg_prtree2@t@_set_point_loc_marshall:
 * @prtree2@t@: a #VsgPRTree2@t@.
 * @marshall: the function marshaller or %NULL.
 * @locdata: the data passed to @marshall or the function for direct loc test.
 *
 * Configure @prtree2@t@ for marshalling its VsgRegion2@t@ localization.
 */
void vsg_prtree2@t@_set_region_loc_marshall (VsgPRTree2@t@ *prtree2@t@,
                                             VsgRegion2@t@LocMarshall marshall,
                                             gpointer locdata)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (marshall != NULL || locdata != NULL);
#endif

  prtree2@t@->config.region_loc_marshall = marshall;

  prtree2@t@->config.region_loc_data = locdata;
}

/**
 * vsg_prtree2@t@_set_point_loc_marshall:
 * @prtree2@t@: a #VsgPRTree2@t@.
 * @marshall: the function marshaller or %NULL.
 * @locdata: the data passed to @marshall or the function for direct dist test.
 *
 * Configure @prtree2@t@ for marshalling its VsgRegion2@t@ distance evaluation.
 */
void
vsg_prtree2@t@_set_point_dist_marshall (VsgPRTree2@t@ *prtree2@t@,
                                        VsgPoint2@t@DistMarshall marshall,
                                        gpointer distdata)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (marshall != NULL || distdata != NULL);
#endif

  prtree2@t@->config.point_dist_marshall = marshall;

  prtree2@t@->config.point_dist_data = distdata;
}

/**
 * vsg_prtree2@t@_set_children_order_with_data :
 * @prtree2@t@: a #VsgPRTree2@t@.
 * @children_order: a function for defining children order in traversals.
 * @root_key: the key to pass to @children_order for the first (root) node.
 * @user_data: user provided data that will be passed to @children_order at each call.
 *
 * Configures @prtree2@t@ to use @children_order for determining children order
 * in traversals. If @children_order is set to %NULL, children ordering returns
 * to the default Z-order.
 *
 * Applies only to single traversal operations (currently not to near/far
 * traversals).
 */
void
vsg_prtree2@t@_set_children_order_with_data (VsgPRTree2@t@ *prtree2@t@,
                                             VsgChildrenOrderDataFunc children_order,
                                             gpointer root_key,
                                             gpointer user_data)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree2@t@ != NULL, -1.);
#endif

  if (children_order == NULL)
    {
      children_order = _z_order_data;
      root_key = NULL;
      user_data = NULL;
    }

  prtree2@t@->config.children_order = children_order;

  prtree2@t@->config.root_key = root_key;
  prtree2@t@->config.children_order_data = user_data;
}

/**
 * vsg_prtree2@t@_set_children_order :
 * @prtree2@t@: a #VsgPRTree2@t@.
 * @children_order: a function for defining children order in traversals.
 * @root_key: the key to pass to @children_order for the first (root) node.
 *
 * Configures @prtree2@t@ to use @children_order for determining children order
 * in traversals. If @children_order is set to %NULL, children ordering returns
 * to the default Z-order.
 *
 * Applies only to single traversal operations (currently not to near/far
 * traversals).
 */
void
vsg_prtree2@t@_set_children_order (VsgPRTree2@t@ *prtree2@t@,
                                   VsgChildrenOrderFunc children_order,
                                   gpointer root_key)
{
  vsg_prtree2@t@_set_children_order_with_data (prtree2@t@,
                                               (VsgChildrenOrderDataFunc) children_order,
                                               root_key, NULL);
}

/**
 * vsg_prtree2@t@_get_tolerance:
 * @prtree2@t@: a #VsgPRTree2@t@
 *
 * Read access to current @prtree2@t@ spatial tolerance.
 *
 * Returns @prtree2@t@ spatial tolerance.
 */
@type@ vsg_prtree2@t@_get_tolerance (VsgPRTree2@t@ *prtree2@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree2@t@ != NULL, -1.);
#endif

  return prtree2@t@->config.tolerance;
}

/**
 * vsg_prtree2@t@_set_tolerance:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @tolerance: a #@type@
 *
 * Sets @prtree2@t@ spatial tolerance to the value of @tolerance.
 * Any distance below @tolerance will be considered as %0. by
 * @prtree2@t@.
 */
void vsg_prtree2@t@_set_tolerance (VsgPRTree2@t@ *prtree2@t@,
                                   @type@ tolerance)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
#endif

  g_return_if_fail (tolerance >= 0.);

  prtree2@t@->config.tolerance = tolerance;
}

/**
 * vsg_prtree2@t@_set_node_data:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @user_data_type: a #GType derived from %G_BOXED_TYPE.
 * @user_data_model: a #GBoxed instance of type @user_data_type.
 *
 * Sets the user data type associated with each nodes of @prtree2@t@ to
 * @user_data_type. If a data type was already present in @prtree2@t@, it
 * will be deleted.
 */
void vsg_prtree2@t@_set_node_data (VsgPRTree2@t@ *prtree2@t@,
                                   GType user_data_type,
                                   gpointer user_data_model)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
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

  _prtree2@t@node_update_user_data_type (prtree2@t@->node,
                                         user_data_type,
                                         user_data_model,
                                         &prtree2@t@->config);

  if (prtree2@t@->config.user_data_type != G_TYPE_NONE)
    {
      g_boxed_free (prtree2@t@->config.user_data_type,
                    prtree2@t@->config.user_data_model);

      prtree2@t@->config.user_data_type = G_TYPE_NONE;
    }

  prtree2@t@->config.user_data_type = user_data_type;
  prtree2@t@->config.user_data_model = user_data_model;
}

/**
 * vsg_prtree2@t@_get_bounds:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @lbound: a #VsgVector2@t@.
 * @ubound: a #VsgVector2@t@.
 *
 * Get the current lower and upper corners of @prtree2@t@ bounding box.
 */
void vsg_prtree2@t@_get_bounds (VsgPRTree2@t@ *prtree2@t@,
                                VsgVector2@t@ *lbound,
                                VsgVector2@t@ *ubound)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (lbound != NULL);
  g_return_if_fail (ubound != NULL);
#endif

  vsg_vector2@t@_copy (&prtree2@t@->node->lbound, lbound);
  vsg_vector2@t@_copy (&prtree2@t@->node->ubound, ubound);
}

/**
 * vsg_prtree2@t@_depth:
 * @prtree2@t@: a #VsgPRTree2@t@
 *
 * Computes the depth of @prtree2@t@.
 *
 * Returns: Depth of @prtree2@t@.
 */
guint vsg_prtree2@t@_depth (const VsgPRTree2@t@ *prtree2@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree2@t@ != NULL, 0);
#endif

  return _prtree2@t@node_depth (prtree2@t@->node);
}

/**
 * vsg_prtree2@t@_point_count
 * @prtree2@t@: a #VsgPRTree2@t@
 *
 * Computes the number of #VsgPoint2 currently stored in @prtree2@t@.
 *
 * Returns: Number of #VsgPoint2 currently stored in @prtree2@t@.
 */
guint vsg_prtree2@t@_point_count (const VsgPRTree2@t@ *prtree2@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree2@t@ != NULL, 0);
#endif

  return _prtree2@t@node_point_count (prtree2@t@->node);
}

/**
 * vsg_prtree2@t@_region_count:
 * @prtree2@t@: a #VsgPRTree2@t@
 *
 * Computes the number of #VsgRegion2 currently stored in @prtree2@t@.
 *
 * Returns: Number of #VsgRegion2 currently stored in @prtree2@t@.
 */
guint vsg_prtree2@t@_region_count (const VsgPRTree2@t@ *prtree2@t@)
{
  g_return_val_if_fail (prtree2@t@ != NULL, 0);

  return _prtree2@t@node_region_count (prtree2@t@->node);
}

/**
 * vsg_prtree2@t@_insert_point
 * @prtree2@t@: a #VsgPRTree2@t@
 * @point: a #VsgPoint2
 *
 * Stores @point reference in prtree2@t@. (#GObject reference support not
 * provided)
 */
void vsg_prtree2@t@_insert_point (VsgPRTree2@t@ *prtree2@t@,
                                  VsgPoint2 point)
{
  const VsgVector2@t@ *lbound;
  const VsgVector2@t@ *ubound;
  const VsgPRTree2@t@Config *config;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (point != NULL);
#endif

  config = &prtree2@t@->config;

  lbound = &prtree2@t@->node->lbound;
  ubound = &prtree2@t@->node->ubound;

  if (CALL_POINT2@T@_LOC (config, point, lbound) != VSG_LOC2_NE ||
      CALL_POINT2@T@_LOC (config, point, ubound) != VSG_LOC2_SW)
    {
      VsgVector2@t@ center = {0.,};
      VsgVector2@t@ diff, new_lbound, new_ubound;
      vsgloc2 loc;

      vsg_vector2@t@_lerp (lbound, ubound, 0.5, &center);

      loc = CALL_POINT2@T@_LOC (config, point, &center);

      vsg_vector2@t@_sub (ubound, lbound, &diff);

      vsg_vector2@t@_copy (lbound, &new_lbound);
      vsg_vector2@t@_copy (ubound, &new_ubound);

      if (loc & VSG_LOC2_X)
        new_ubound.x += diff.x;
      else
        new_lbound.x -= diff.x;

      if (loc & VSG_LOC2_Y)
        new_ubound.y += diff.y;
      else
        new_lbound.y -= diff.y;

      loc = VSG_LOC2_MASK & ~(loc);
      prtree2@t@->node = _int_alloc (&new_lbound,
                                     &new_ubound,
                                     prtree2@t@->node,
                                     loc,
                                     config);
      vsg_prtree2@t@_insert_point (prtree2@t@, point);
    }
  else
    _prtree2@t@node_insert_point (prtree2@t@->node, point,
                                  config);
}

/**
 * vsg_prtree2@t@_remove_point:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @point: a #VsgPoint2
 *
 * Removes specified #VsgPoint2 reference from @prtree2@t@.
 *
 * Returns: %TRUE if @point was found and removed, %FALSE otherwise.
 */
gboolean vsg_prtree2@t@_remove_point (VsgPRTree2@t@ *prtree2@t@,
                                      VsgPoint2 point)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree2@t@ != NULL, FALSE);
  g_return_val_if_fail (point != NULL, FALSE);
#endif

  return
    _prtree2@t@node_remove_point (prtree2@t@->node, point,
                                  &prtree2@t@->config);
}

/**
 * vsg_prtree2@t@_write:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @file: a #FILE
 * 
 * Writes a summary of @prtree2@t@ in @file. Mainly used for debugging.
 */
void vsg_prtree2@t@_write (const VsgPRTree2@t@ *prtree2@t@, FILE *file)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (file != NULL);
#endif

  _prtree2@t@node_write (prtree2@t@->node, file, 0);
}

/**
 * vsg_prtree2@t@_print:
 * @prtree2@t@: a #VsgPRTree2@t@
 *
 * Prints a summary of @prtree2@t@ on standard output. Mainly used for debugging.
 */
void vsg_prtree2@t@_print (const VsgPRTree2@t@ *prtree2@t@)
{
  vsg_prtree2@t@_write (prtree2@t@, stdout);
}

/**
 * vsg_prtree2@t@_find_point:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @selector: a #VsgPoint2
 *
 * Search @prtree2@t@ for a #VsgPoint2 which distance from @selector
 * is zero.
 *
 * Returns the first #VsgPoint2 found or NULL.
 */
VsgPoint2 vsg_prtree2@t@_find_point (VsgPRTree2@t@ *prtree2@t@,
                                     VsgPoint2 selector)
{
  const VsgVector2@t@ *lbound;
  const VsgVector2@t@ *ubound;
  const VsgPRTree2@t@Config *config;

#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree2@t@ != NULL, NULL);
  g_return_val_if_fail (selector != NULL, NULL);
#endif

  config = &prtree2@t@->config;

  lbound = &prtree2@t@->node->lbound;
  ubound = &prtree2@t@->node->ubound;

  if (CALL_POINT2@T@_LOC (config, selector, lbound) != VSG_LOC2_NE ||
      CALL_POINT2@T@_LOC (config, selector, ubound) != VSG_LOC2_SW)
    return NULL;

  return _prtree2@t@node_find_point (prtree2@t@->node, selector, config);
}

/**
 * vsg_prtree2@t@_foreach_point:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @func: a #GFunc
 * @user_data: a #gpointer associated to func
 *
 * Performs a "@func (point, @user_data)" call on every #VsgPoint2
 * found in @prtree2@t@.
 */
void vsg_prtree2@t@_foreach_point (VsgPRTree2@t@ *prtree2@t@, GFunc func,
                                   gpointer user_data)
{
  Foreach foreach = {func, user_data};

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (func != NULL);
#endif

  vsg_prtree2@t@_traverse (prtree2@t@, G_POST_ORDER,
                           (VsgPRTree2@t@Func) _foreach_point,
                           &foreach);
}

/**
 * vsg_prtree2@t@_foreach_point_custom:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @selector: a #VsgRegion2
 * @locfunc: #VsgRegion2Point2LocFunc associated to @selector
 * @func: a #GFunc
 * @user_data: a #gpointer associated to func
 *
 * Performs a "@func (point, @user_data)" call on every #VsgPoint2
 * conforming to @selector/@locfunc test found in @prtree2@t@.
 */
void
vsg_prtree2@t@_foreach_point_custom (VsgPRTree2@t@ *prtree2@t@,
                                     VsgRegion2 selector,
                                     VsgRegion2Point2LocFunc locfunc,
                                     GFunc func,
                                     gpointer user_data)
{
  CustomForeach custom = {func, user_data, selector, locfunc};

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (func != NULL);
  g_return_if_fail (prtree2@t@->config.region_loc_data != NULL);
#endif

  if (custom.locfunc == NULL)
    custom.locfunc = _region2_point2_locfunc_always;

  vsg_prtree2@t@_traverse_custom (prtree2@t@, G_POST_ORDER, selector,
                                  (VsgPRTree2@t@Func) _foreach_point_custom,
                                  &custom);
}

/**
 * vsg_prtree2@t@_insert_region:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @region: a #VsgRegion
 *
 *  Inserts @region in @prtree2@t@ lists.
 */
void vsg_prtree2@t@_insert_region (VsgPRTree2@t@ *prtree2@t@,
                                   VsgRegion2 region)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (region != NULL);
#endif

  _prtree2@t@node_insert_region (prtree2@t@->node, region,
                                 &prtree2@t@->config);
}

/**
 * vsg_prtree2@t@_remove_region:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @region: a #VsgRegion
 *
 * Removes specified @region from @prtree2@t@ lists.
 *
 * Returns %TRUE if @region was in @prtree2@t@, %FALSE if not.
 */
gboolean vsg_prtree2@t@_remove_region (VsgPRTree2@t@ *prtree2@t@,
                                       VsgRegion2 region)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree2@t@ != NULL, FALSE);
  g_return_val_if_fail (region != NULL, FALSE);
#endif

  return
    _prtree2@t@node_remove_region (prtree2@t@->node, region,
                                   &prtree2@t@->config);
}

/**
 * vsg_prtree2@t@_foreach_region:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @func: function to perform on found #VsgRegion2
 * @user_data: user provided pointer
 *
 * Performs "@func (region, @user_data)" call on every #VsgRegion2 in @prtree2@t@.
 */
void vsg_prtree2@t@_foreach_region (VsgPRTree2@t@ *prtree2@t@, GFunc func,
                                    gpointer user_data)
{
  Foreach foreach = {func, user_data};

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (func != NULL);
#endif

  vsg_prtree2@t@_traverse (prtree2@t@, G_POST_ORDER,
                           (VsgPRTree2@t@Func) _foreach_region,
                           &foreach);
}

/**
 * vsg_prtree2@t@_foreach_region_custom:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @selector: the #VsgRegion2 used to specify search
 * @func: function to perform on found #VsgRegion2
 * @user_data: user provided pointer
 *
 * Performs "@func (region, @user_data)" call on found #VsgRegion2 in 
 * @prtree2@t@. Warning: call to @func will be performed on every #VsgRegion2 that
 * is in a @prtree2@t@ sub-node that matches @selector/@locfunc localzation. This
 * means @locfunc must provide its own #VsgRegion2/#VsgRegion2 intersection
 * tests if it needs to.
 */
void vsg_prtree2@t@_foreach_region_custom (VsgPRTree2@t@ *prtree2@t@,
                                           VsgRegion2 selector,
                                           GFunc func,
                                           gpointer user_data)
{
  CustomForeach custom = {func, user_data, selector, NULL};

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (func != NULL);
#endif

  vsg_prtree2@t@_traverse_custom (prtree2@t@, G_POST_ORDER, selector,
                                  (VsgPRTree2@t@Func) _foreach_region_custom,
                                  &custom);
}

static gboolean _checkregion2_true (const VsgRegion2 region,
                                    gpointer user_data)
{
  return TRUE;
}

/**
 * vsg_prtree2@t@_find_deep_region:
 * @prtree2@t@: a #VsgPRTree2@t@
 * @point: a #VsgPoint2 or %NULL
 * @check: a #VsgRegion2CheckFunc
 * @user_data: a pointer to be passed to @check
 *
 * Performs a depth first search of a #VsgRegion2 in the vicinity of
 * @point. If @check is not %NULL, the returned #VsgRegion2 is the
 * first one that return %TRUE to a call of @check().
 *
 * Returns: the deepest #VsgRegion2 found.
 */
VsgRegion2 vsg_prtree2@t@_find_deep_region (VsgPRTree2@t@ *prtree2@t@,
                                            VsgPoint2 point,
                                            VsgRegion2CheckFunc check,
                                            gpointer user_data)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (prtree2@t@ != NULL, NULL);
#endif

  if (check == NULL) check = _checkregion2_true;

  return _prtree2@t@node_find_deep_region (prtree2@t@->node,
                                           point,
                                           check,
                                           user_data,
                                           &prtree2@t@->config);
}


/**
 * vsg_prtree2@t@_traverse:
 * @prtree2@t@: a #VsgPRTree2@t@.
 * @order: traverse order.
 * @func: a #VsgPrtree2@t@Func.
 * @user_data: a #gpointer data to be apssed to @func.
 *
 * Performs a traversal of @prtree2@t@, executing @func on each of its nodes
 * in @order order.
 *
 * WARNING: Please note that %G_LEVEL_ORDER is not currenlty implemented.
 */
void vsg_prtree2@t@_traverse (VsgPRTree2@t@ *prtree2@t@,
                              GTraverseType order,
                              VsgPRTree2@t@Func func,
                              gpointer user_data)
{
  vsg_prtree2@t@_traverse_custom (prtree2@t@, order, NULL, func, user_data);
}

/**
 * vsg_prtree2@t@_traverse_custom:
 * @prtree2@t@: a #VsgPRTree2@t@.
 * @order: traverse order.
 * @selector: a #VsgRegion2
 * @func: a #VsgPrtree2@t@Func.
 * @user_data: a #gpointer data to be apssed to @func.
 *
 * Performs a traversal of @prtree2@t@, executing @func on each of its nodes
 * that intersects @selector (as defined by provided region_locfunc in
 * vsg_prtree2@t@_new_full()). The @order parameter refers to the order between
 * @func call and children traversal for each node. If you need to set a
 * specific order between node's children, please refer to
 * vsg_prtree2@t@_set_children_order() function.
 *
 * WARNING: Please note that %G_LEVEL_ORDER is not currenlty implemented.
 */
void vsg_prtree2@t@_traverse_custom (VsgPRTree2@t@ *prtree2@t@,
                                     GTraverseType order,
                                     VsgRegion2 selector,
                                     VsgPRTree2@t@Func func,
                                     gpointer user_data)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
  g_return_if_fail (order != G_LEVEL_ORDER);
  g_return_if_fail (func != NULL);
#endif

  _prtree2@t@node_traverse_custom (prtree2@t@->node, NULL,
                                   order, selector, func,
                                   user_data, &prtree2@t@->config,
                                   prtree2@t@->config.root_key);
}

GType vsg_prtree2@t@_node_info_get_type (void)
{
  static GType prtree2@t@_node_info_type = 0;

  if (!prtree2@t@_node_info_type)
    {
      prtree2@t@_node_info_type =
        g_boxed_type_register_static ("VsgPRTree2@t@NodeInfo",
                                      (GBoxedCopyFunc) vsg_prtree2@t@_node_info_clone,
                                      (GBoxedFreeFunc) vsg_prtree2@t@_node_info_free);

    }

  return prtree2@t@_node_info_type;
}

void vsg_prtree2@t@_node_info_free (VsgPRTree2@t@NodeInfo *node_info)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (node_info != NULL);
#endif

  g_free (node_info);
}

VsgPRTree2@t@NodeInfo *
vsg_prtree2@t@_node_info_clone (VsgPRTree2@t@NodeInfo *node_info)
{
#ifdef VSG_CHECK_PARAMS
  g_return_val_if_fail (node_info != NULL, NULL);
#endif

  return g_memdup (node_info, sizeof (VsgPRTree2@t@NodeInfo));
}
