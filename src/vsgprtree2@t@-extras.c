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

#include "vsgvector2@t@-inline.h"

#include "vsgprtree2@t@-extras.h"

#include "vsgprtree2@t@-private.h"

/* axis flags */
#define _X VSG_LOC2_X
#define _Y VSG_LOC2_Y

/* computes child index symmetry using binary indexes */
/*
 * for example symmetry along X axis gives: (_INDEX_SYMMETRY (index, -1, _X))
 * 2 3 -> 3 2
 * 0 1    1 0
 * Y symmetry:(_INDEX_SYMMETRY (index, -1, _Y))
 * 2 3 -> 0 1
 * 0 1    2 3
 */
#define _INDEX_SYMMETRY(index, i, I) (\
(((i)<0)?~(index):(index)) & (I) \
)

/* computes total symmetry given an index and two numbers */
#define _SYMMETRY(index, x, y) (\
(_INDEX_SYMMETRY (index, x, _X) | _INDEX_SYMMETRY (index, y, _Y)) \
)


/* positive value means far boxes. 0 means neighbours */
static guint8 n01[4][4] = {
  /*     0, 1, 2, 3 */
  /*0*/ {1, 1, 1, 1},
  /*1*/ {0, 1, 0, 1},
  /*2*/ {1, 1, 1, 1},
  /*3*/ {0, 1, 0, 1},
};

static guint8 n10[4][4] = {
  /*     0, 1, 2, 3 */
  /*0*/ {1, 1, 1, 1},
  /*1*/ {1, 1, 1, 1},
  /*2*/ {0, 0, 1, 1},
  /*3*/ {0, 0, 1, 1},
};

static guint8 n11[4][4] = {
  /*     0, 1, 2, 3 */
  /*0*/ {1, 1, 1, 1},
  /*1*/ {1, 1, 1, 1},
  /*2*/ {1, 1, 1, 1},
  /*3*/ {0, 1, 1, 1},
};

/* uses n?? to determine near/far position of two children from neighbour
 * parents
 */
#define NEAR_FAR(x,y,i,j) (\
((x)==0)? (((y)==0)?0:n10[i][j]) : (((y)==0)?n01[i][j]:n11[i][j]) \
)

#define _AXIS_DIFF(idx, jdx, I) (\
(((jdx)&(I)) - ((idx)&(I))) \
)

/* given idx, jdx two children of neighbour parents and i,I the parents
 * respective position along axis I, we compute new position between idx,jdx
 * 
 */
#define _SUB_INTERACTION(idx, jdx, i, I) (\
((i)!=0)?(i):_AXIS_DIFF (idx, jdx, I) \
)

void _vsg_prtree2@t@node_get_info (VsgPRTree2@t@Node *node,
                                   VsgPRTree2@t@NodeInfo *node_info,
                                   VsgPRTree2@t@NodeInfo *father_info,
                                   guint8 child_number)
{
  VsgPRTreeKey2@t@ *father_id;

  vsg_vector2@t@_copy (&node->lbound, &node_info->lbound);
  vsg_vector2@t@_copy (&node->ubound, &node_info->ubound);
  vsg_vector2@t@_copy (&node->center, &node_info->center);

  node_info->isleaf = PRTREE2@T@NODE_ISLEAF(node);

  node_info->father_info = father_info;

  if (node_info->isleaf)
    {
      node_info->point_list = PRTREE2@T@NODE_LEAF(node).point;
    }
  else
    {
      node_info->point_list = NULL;
    }

  node_info->region_list = node->region_list;

  node_info->point_count = node->point_count;
  node_info->region_count = node->region_count;

  if (father_info == NULL) node_info->depth = 0;
  else node_info->depth = father_info->depth + 1;

  node_info->user_data = node->user_data;

  if (father_info == NULL) father_id = NULL;
  else father_id = &father_info->id;

  vsg_prtree_key2@t@_build_child (father_id, child_number, &node_info->id);

  node_info->parallel_status = node->parallel_status;
}

VsgPRTree2@t@Node *_vsg_prtree2@t@node_get_child_at (VsgPRTree2@t@Node *node,
                                                     const VsgVector2@t@ *pos,
                                                     gint depth)
{
  vsgloc2 i;
  VsgPRTree2@t@Node *child = NULL;

  if (depth == 0 || PRTREE2@T@NODE_ISLEAF(node)) return node;

  i = vsg_vector2@t@_vector2@t@_locfunc (&node->center, pos);

  child = PRTREE2@T@NODE_CHILD(node, i);

  return _vsg_prtree2@t@node_get_child_at (child, pos, depth - 1);
}

VsgPRTree2@t@Node *vsg_prtree2@t@node_key_lookup (VsgPRTree2@t@Node *node,
                                                  VsgPRTreeKey2@t@ key)
{
  vsgloc2 i;
  VsgPRTree2@t@Node *child = NULL;

  if (key.depth == 0 || PRTREE2@T@NODE_ISLEAF(node)) return node;

  i = vsg_prtree_key2@t@_child (&key);

  child = PRTREE2@T@NODE_CHILD(node, i);

  key.depth --;

  return vsg_prtree2@t@node_key_lookup (child, key);
}

static void recursive_near_func (VsgPRTree2@t@Node *one,
                                 VsgPRTree2@t@NodeInfo *one_info,
                                 VsgPRTree2@t@Node *other,
                                 VsgPRTree2@t@NodeInfo *other_info,
                                 VsgNFConfig2@t@ *nfc)
{
  vsgloc2 i;

  if (PRTREE2@T@NODE_ISINT (one))
    {
      for (i=0; i<4; i++)
        {
          VsgPRTree2@t@Node *one_child = PRTREE2@T@NODE_CHILD(one, i);
          VsgPRTree2@t@NodeInfo one_child_info;

#ifdef VSG_HAVE_MPI
          if (PRTREE2@T@NODE_IS_REMOTE (one_child)) continue;
#endif

          _vsg_prtree2@t@node_get_info (one_child, &one_child_info, one_info,
                                        i);

          recursive_near_func (one_child, &one_child_info, other, other_info,
                               nfc);
        }
    }
  else if (PRTREE2@T@NODE_ISINT (other))
    {
      for (i=0; i<4; i++)
        {
          VsgPRTree2@t@Node *other_child = PRTREE2@T@NODE_CHILD(other, i);
          VsgPRTree2@t@NodeInfo other_child_info;

#ifdef VSG_HAVE_MPI
          if (PRTREE2@T@NODE_IS_REMOTE (other_child)) continue;
#endif

          _vsg_prtree2@t@node_get_info (other_child, &other_child_info,
                                        other_info, i);

          recursive_near_func (one, one_info, other_child, &other_child_info,
                               nfc);
        }
    }
  else
    {
      /* near interaction between one and other */
      if (one->point_count != 0 && other->point_count != 0)
        nfc->near_func (one_info, other_info, nfc->user_data);
    }


}

void vsg_prtree2@t@node_recursive_near_func (VsgPRTree2@t@Node *one,
                                             VsgPRTree2@t@NodeInfo *one_info,
                                             VsgPRTree2@t@Node *other,
                                             VsgPRTree2@t@NodeInfo *other_info,
                                             VsgNFConfig2@t@ *nfc)
{
  recursive_near_func (one, one_info, other, other_info, nfc);
}

#ifdef VSG_HAVE_MPI
#define _NODE_IS_EMPTY(node) ( \
 (node)->point_count == 0 && PRTREE2@T@NODE_IS_LOCAL (node) \
)
#else
#define _NODE_IS_EMPTY(node) ( \
 (node)->point_count == 0 \
)
#endif

static void
_sub_neighborhood_near_far_traversal (VsgNFConfig2@t@ *nfc,
                                      VsgPRTree2@t@Node *one,
                                      VsgPRTree2@t@NodeInfo *one_info,
                                      VsgPRTree2@t@Node *other,
                                      VsgPRTree2@t@NodeInfo *other_info,
                                      gint8 x, gint8 y)
{
  vsgloc2 i, j, si, sj;
  vsgloc2 sym[4] = {
    _SYMMETRY (0, x, y),
    _SYMMETRY (1, x, y),
    _SYMMETRY (2, x, y),
    _SYMMETRY (3, x, y),
  };

  if (_NODE_IS_EMPTY(one) || _NODE_IS_EMPTY(other))
    return;

#ifdef VSG_HAVE_MPI
  if (nfc->sz > 1)
    vsg_prtree2@t@_nf_check_receive (nfc, MPI_ANY_TAG, FALSE);
#endif

  if (PRTREE2@T@NODE_ISINT (one) && PRTREE2@T@NODE_ISINT (other))
    {
#ifdef VSG_HAVE_MPI
      if (nfc->sz > 1) vsg_prtree2@t@_nf_check_send (nfc);
#endif
      /* near/far interaction between one and other children */
      for (i=0; i<4; i++)
        {
          VsgPRTree2@t@Node *one_child = PRTREE2@T@NODE_CHILD(one, i);
          VsgPRTree2@t@NodeInfo one_child_info;

#ifdef VSG_HAVE_MPI
          if (PRTREE2@T@NODE_IS_REMOTE (one_child)) continue;
#endif

          _vsg_prtree2@t@node_get_info (one_child, &one_child_info,
                                        one_info, i);

          si = sym[i];

          for (j=0; j<4; j++)
            {
              gboolean far;
              VsgPRTree2@t@Node *other_child =
                PRTREE2@T@NODE_CHILD(other, j);
              VsgPRTree2@t@NodeInfo other_child_info;
              gboolean far_done = TRUE;

#ifdef VSG_HAVE_MPI
              if (PRTREE2@T@NODE_IS_REMOTE (other_child)) continue;
#endif

              _vsg_prtree2@t@node_get_info (other_child,
                                            &other_child_info,
                                            other_info, j);

              sj = sym[j];

              far = NEAR_FAR (x, y, si, sj);

              if (far)
                {
                  /* far interaction between one and other */
                  if (nfc->far_func &&
                      ! (_NODE_IS_EMPTY (one_child) ||
                         _NODE_IS_EMPTY (other_child)))
                    {
#ifdef VSG_HAVE_MPI
                      /* in parallel, only one of the processors has to do a
                         shared/shared far interaction */
                      if (PRTREE2@T@NODE_IS_SHARED (one_child) &&
                          PRTREE2@T@NODE_IS_SHARED (other_child) &&
                          PRTREE2@T@NODE_PROC (one_child) != nfc->rk)
                        continue;
#endif

                      far_done = nfc->far_func (&one_child_info, &other_child_info,
                                                nfc->user_data);
                      if (!far_done)
                        {
#ifdef VSG_HAVE_MPI
                          if (PRTREE2@T@NODE_IS_SHARED (one_child) ||
                              PRTREE2@T@NODE_IS_SHARED (other_child))
                            {
                              g_critical ("far_func() -> FALSE not handled " \
                                          "for shared nodes in \"%s\"",
                                          __PRETTY_FUNCTION__);

                            }
#endif

                          /* near interaction between one and other */
                          recursive_near_func (one_child, &one_child_info,
                                               other_child, &other_child_info,
                                               nfc);
                        }
                    }
                }
              else
                {
                  gint8 newx = _SUB_INTERACTION (i, j, x, _X);
                  gint8 newy = _SUB_INTERACTION (i, j, y, _Y);

                  _sub_neighborhood_near_far_traversal (nfc, one_child,
                                                        &one_child_info,
                                                        other_child,
                                                        &other_child_info,
                                                        newx, newy);
                }
            }
        }

    }
  else
    {
      if (nfc->near_func)
        {
          /* near interaction between one and other */
          recursive_near_func (one, one_info, other, other_info, nfc);
        }
    }
}

static void
vsg_prtree2@t@node_near_far_traversal (VsgNFConfig2@t@ *nfc,
                                       VsgPRTree2@t@Node *node,
                                       VsgPRTree2@t@NodeInfo *father_info,
                                       vsgloc2 child_number,
                                       gboolean parallel_check)
{
  vsgloc2 i,j;
  VsgPRTree2@t@NodeInfo node_info;

#ifdef VSG_HAVE_MPI
  if (PRTREE2@T@NODE_IS_REMOTE (node)) return;
#endif
  if (_NODE_IS_EMPTY (node)) return;

  _vsg_prtree2@t@node_get_info (node, &node_info, father_info, child_number);

#ifdef VSG_HAVE_MPI
  /* check for remote processors to send this node, if needed */
  parallel_check = vsg_prtree2@t@_node_check_parallel_near_far (nfc,
                                                                node,
                                                                &node_info,
                                                                parallel_check);
#endif

  if (PRTREE2@T@NODE_ISLEAF (node))
    {
      if (nfc->near_func)
        {
          /* reflexive near interaction on node */
          nfc->near_func (&node_info, &node_info, nfc->user_data);
        }
    }
  else
    {
      /* interactions in node's children descendants */
      for (i=0; i<4; i++)
        vsg_prtree2@t@node_near_far_traversal (nfc,
                                               PRTREE2@T@NODE_CHILD(node, i),
                                               &node_info, i, parallel_check);

      /* interactions between node's children */
      for (i=0; i<4; i++)
        {
          VsgPRTree2@t@Node *one_child = PRTREE2@T@NODE_CHILD(node, i);
          VsgPRTree2@t@NodeInfo one_child_info;

#ifdef VSG_HAVE_MPI
          if (PRTREE2@T@NODE_IS_REMOTE (one_child)) continue;
#endif

          _vsg_prtree2@t@node_get_info (one_child, &one_child_info,
                                        &node_info, i);
          for (j=i+1; j<4; j++)
            {
              VsgPRTree2@t@Node *other_child =
                PRTREE2@T@NODE_CHILD(node, j);
              VsgPRTree2@t@NodeInfo other_child_info;
              gint8 x, y;

#ifdef VSG_HAVE_MPI
              if (PRTREE2@T@NODE_IS_REMOTE (other_child)) continue;
#endif

              x = _AXIS_DIFF (i, j, _X);
              y = _AXIS_DIFF (i, j, _Y);

              _vsg_prtree2@t@node_get_info (other_child,
                                            &other_child_info,
                                            &node_info, j);

              _sub_neighborhood_near_far_traversal (nfc, one_child,
                                                    &one_child_info,
                                                    other_child,
                                                    &other_child_info,
                                                    x, y); 
            }
        }

    }
}

typedef enum _Hilbert2Key Hilbert2Key;
enum _Hilbert2Key {
  HK2_0_1,
  HK2_3_2,
  HK2_3_1,
  HK2_0_2,

};

static gint hilbert2_coords[][4] = {
  {0, 2, 3, 1, },
  {3, 1, 0, 2, },
  {3, 2, 0, 1, },
  {0, 1, 3, 2, },

};

static Hilbert2Key hilbert2_decompositions[][4] = {
  {HK2_0_2, HK2_0_1, HK2_0_1, HK2_3_1, },
  {HK2_3_1, HK2_3_2, HK2_3_2, HK2_0_2, },
  {HK2_3_2, HK2_3_1, HK2_3_1, HK2_0_1, },
  {HK2_0_1, HK2_0_2, HK2_0_2, HK2_3_2, },

};

static void hilbert2_order (gpointer node_key, gint *children,
                            gpointer *children_keys)
{
  gint i;
  Hilbert2Key hkey = GPOINTER_TO_INT (node_key);

  for (i=0; i<4; i++)
    {
      children[i] = hilbert2_coords[hkey][i];
      children_keys[i] = GINT_TO_POINTER (hilbert2_decompositions[hkey][i]);
    }
}


/*-------------------------------------------------------------------*/
/* typedefs and structure doc */
/*-------------------------------------------------------------------*/
/**
 * VsgPRTree2@t@NodeInfo:
 * @lbound: A copy of the lower left corner of the node's box (lower bound point).
 * @ubound: A copy of the upper right corner of the node's box (upper bound point).
 * @center: A copy of the center of the node's box.
 * @point_list: the list of #VsgPoint2 contained in this node if it's a leaf node. %NULL otherwise.
 * @region_list: the list of #VsgRegion2 contained in this node.
 * @point_count: the total number of #VsgPoint2 contained in this subtree.
 * @region_count: the total number of #VsgRegion2 contained in this subtree.
 * @depth: node's depth in the tree.
 * @user_data: the data eventually attached to this node.
 * @father_info: link to this node's father's #VsgPRTree2@t@NodeInfo. NULL if root node.
 * @isleaf: a #gboolean set if the node is a leaf.
 * @id: the key representing this node in the tree hierarchy.
 * @parallel_status: parallel status of this node.
 *
 * A structure provided to expose a #VsgPrtree2@t@ node during a traversal
 * process. Since @user_data and @children_user_data are a user controlled
 * feature (set by the vsg_prtree2@t@_set_node_data() function), it's up to
 * the user himself to decide if it's safe to modify these fields or not
 * during a traversal.
 *
 * WARNING: Please, note that modifying @point_list, @region_list will lead
 * to undefined behaviour.
 */

/**
 * VsgPRTree2@t@Func:
 * @node_info: a #VsgPRTree2@t@NodeInfo.
 * @user_data: user provided data.
 *
 * A type of functions for #VsgPRTree2@t@ nodes traversals.
 */

/**
 * VsgPRTree2@t@InteractionFunc:
 * @one_info: a #VsgPRTree2@t@NodeInfo.
 * @other_info: a #VsgPRTree2@t@NodeInfo.
 * @user_data: user provided data.
 *
 * A type of functions for #VsgPRTree2@t@ nodes interaction traversals.
 */

/**
 * VsgPRTree2@t@FarInteractionFunc:
 * @one_info: a #VsgPRTree2@t@NodeInfo.
 * @other_info: a #VsgPRTree2@t@NodeInfo.
 * @user_data: user provided data.
 *
 * A type of functions for #VsgPRTree2@t@ nodes interaction traversals. Used
 * for far interactions, the function should return %TRUE when it effectively
 * realizes the far interaction computations. Alternatively, the function can
 * return %FALSE if the far interaction won't be computed (for any reason).
 * In this case, vsg_prtree2@t@_near_far_traversal() will fallback to near
 * interaction evaluation.
 *
 * Returns: Confirmation that the interaction really occurred.
 */

/*-------------------------------------------------------------------*/
/* public functions */
/*-------------------------------------------------------------------*/

/**
 * vsg_prtree2@t@_near_far_traversal:
 * @prtree2@t@: a #VsgPRTree2@t@.
 * @far_func: the far interaction user provided function.
 * @near_func: the near interaction user provided function.
 * @user_data: the data to pass to @far_fun and @near_func.
 *
 * Computes an interaction traversal for @prtree2@t@. This means that
 * @near_func will be called once for every couple of neighbour @prtree2@t@
 * leaf nodes (this is reflexive) and @far_func will be called once for every
 * couple of non neighbour nodes which parents are neighbours. If @far_func
 * happens to return %FALSE, the far interaction would be considered not
 * treated. In this case, @near_func will be called on every pair of @far_func
 * actual nodes arguments' descendants.
 * 
 * This kind of traversal is mainly provided for multipole algorithms.
 *
 * WARNING: Please note that @near_func and @far_func will only be called on
 * nodes that contain at least one #VsgPoint2.
 */
void
vsg_prtree2@t@_near_far_traversal (VsgPRTree2@t@ *prtree2@t@,
                                   VsgPRTree2@t@FarInteractionFunc far_func,
                                   VsgPRTree2@t@InteractionFunc near_func,
                                   gpointer user_data)
{
  VsgNFConfig2@t@ nfc;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
#endif

#ifdef VSG_HAVE_MPI
  /* vsg_packed_msg_trace ("enter 1 [nf traversal]"); */

  vsg_nf_config2@t@_init (&nfc, prtree2@t@, far_func, near_func, user_data);

  vsg_nf_config2@t@_tmp_alloc (&nfc, &prtree2@t@->config);

  if (nfc.sz > 1 && prtree2@t@->config.remote_depth_dirty)
    {
      vsg_prtree2@t@_update_remote_depths (prtree2@t@);
    }
#else

  nfc.tree = prtree2@t@;
  nfc.far_func = far_func;
  nfc.near_func = near_func;
  nfc.user_data = user_data;

#endif

  vsg_prtree2@t@node_near_far_traversal (&nfc,
                                         prtree2@t@->node,
                                         NULL, 0, TRUE);

#ifdef VSG_HAVE_MPI
  /* vsg_packed_msg_trace ("leave 1 [nf traversal]"); */
  /* vsg_packed_msg_trace ("enter 1 [nf parallel_end]"); */

  if (nfc.sz > 1)
    {
      vsg_prtree2@t@_nf_check_parallel_end (&nfc);
    }

  vsg_nf_config2@t@_tmp_free (&nfc, &prtree2@t@->config);

  vsg_nf_config2@t@_clean (&nfc);

  /* vsg_packed_msg_trace ("leave 1 [nf parallel_end]"); */
#endif

}

/**
 * vsg_prtree2@t@_set_children_order_hilbert:
 * @prtree2@t@: a #VsgPRTree2@t@.
 *
 * Configures @prtree2@t@ for Hilbert curve order traversal.
 */
void vsg_prtree2@t@_set_children_order_hilbert (VsgPRTree2@t@ *prtree2@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
#endif

  vsg_prtree2@t@_set_children_order (prtree2@t@, hilbert2_order,
                                     GINT_TO_POINTER (HK2_0_1));

}
/**
 * vsg_prtree2@t@_set_children_order_default:
 * @prtree2@t@: a #VsgPRTree2@t@.
 *
 * Configures @prtree2@t@ for default (Z order) order traversal.
 */
void vsg_prtree2@t@_set_children_order_default (VsgPRTree2@t@ *prtree2@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree2@t@ != NULL);
#endif

  vsg_prtree2@t@_set_children_order (prtree2@t@, NULL, NULL);
}
