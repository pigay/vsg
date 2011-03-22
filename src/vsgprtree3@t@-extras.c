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

#include "vsgvector3@t@-inline.h"

#include "vsgprtree3@t@-extras.h"

#include "vsgprtree3@t@-private.h"

/* axis flags */
#define _X VSG_LOC3_X
#define _Y VSG_LOC3_Y
#define _Z VSG_LOC3_Z

/* computes child index symmetry using binary indexes */
/*
 * see vsgprttree2@t@-extras.c for examples.
 */
#define _INDEX_SYMMETRY(index, i, I) (\
(((i)<0)?~(index):(index)) & (I) \
)

/* computes total symmetry given an index and two numbers */
#define _SYMMETRY(index, x, y, z) (\
(_INDEX_SYMMETRY (index, x, _X) | \
 _INDEX_SYMMETRY (index, y, _Y) | \
 _INDEX_SYMMETRY (index, z, _Z)) \
)


/* positive value means far boxes. 0 means neighbours */
static guint8 n001[8][8] = {
  /*     0, 1, 2, 3, 4, 5, 6, 7 */
  /*0*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*1*/ {0, 1, 0, 1, 0, 1, 0, 1},
  /*2*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*3*/ {0, 1, 0, 1, 0, 1, 0, 1},
  /*4*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*5*/ {0, 1, 0, 1, 0, 1, 0, 1},
  /*6*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*7*/ {0, 1, 0, 1, 0, 1, 0, 1},
};

static guint8 n010[8][8] = {
  /*     0, 1, 2, 3, 4, 5, 6, 7 */
  /*0*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*1*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*2*/ {0, 0, 1, 1, 0, 0, 1, 1},
  /*3*/ {0, 0, 1, 1, 0, 0, 1, 1},
  /*4*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*5*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*6*/ {0, 0, 1, 1, 0, 0, 1, 1},
  /*7*/ {0, 0, 1, 1, 0, 0, 1, 1},
};

static guint8 n100[8][8] = {
  /*     0, 1, 2, 3, 4, 5, 6, 7 */
  /*0*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*1*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*2*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*3*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*4*/ {0, 0, 0, 0, 1, 1, 1, 1},
  /*5*/ {0, 0, 0, 0, 1, 1, 1, 1},
  /*6*/ {0, 0, 0, 0, 1, 1, 1, 1},
  /*7*/ {0, 0, 0, 0, 1, 1, 1, 1},
};

static guint8 n011[8][8] = {
  /*     0, 1, 2, 3, 4, 5, 6, 7 */
  /*0*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*1*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*2*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*3*/ {0, 1, 1, 1, 0, 1, 1, 1},
  /*4*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*5*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*6*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*7*/ {0, 1, 1, 1, 0, 1, 1, 1},
};

static guint8 n101[8][8] = {
  /*     0, 1, 2, 3, 4, 5, 6, 7 */
  /*0*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*1*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*2*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*3*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*4*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*5*/ {0, 1, 0, 1, 1, 1, 1, 1},
  /*6*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*7*/ {0, 1, 0, 1, 1, 1, 1, 1},
};

static guint8 n110[8][8] = {
  /*     0, 1, 2, 3, 4, 5, 6, 7 */
  /*0*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*1*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*2*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*3*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*4*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*5*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*6*/ {0, 0, 1, 1, 1, 1, 1, 1},
  /*7*/ {0, 0, 1, 1, 1, 1, 1, 1},
};

static guint8 n111[8][8] = {
  /*     0, 1, 2, 3, 4, 5, 6, 7 */
  /*0*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*1*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*2*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*3*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*4*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*5*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*6*/ {1, 1, 1, 1, 1, 1, 1, 1},
  /*7*/ {0, 1, 1, 1, 1, 1, 1, 1},
};

/* uses n??? to determine near/far position of two children from neighbour
 * parents
 */
#define NEAR_FAR(x,y,z,i,j) (\
((x)==0)? \
   (((y)==0)? \
      (((z)==0)? 0 : n100[i][j]): \
      (((z)==0)? n010[i][j] : n110[i][j]) \
    ) : \
   (((y)==0)? \
      (((z)==0)? n001[i][j] : n101[i][j]): \
      (((z)==0)? n011[i][j] : n111[i][j]) \
   ) \
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

void _vsg_prtree3@t@node_get_info (VsgPRTree3@t@Node *node,
                                   VsgPRTree3@t@NodeInfo *node_info,
                                   VsgPRTree3@t@NodeInfo *father_info,
                                   guint8 child_number)
{
  VsgPRTreeKey3@t@ *father_id;

  vsg_vector3@t@_copy (&node->lbound, &node_info->lbound);
  vsg_vector3@t@_copy (&node->ubound, &node_info->ubound);
  vsg_vector3@t@_copy (&node->center, &node_info->center);

  node_info->isleaf = PRTREE3@T@NODE_ISLEAF(node);

  node_info->father_info = father_info;

  if (node_info->isleaf)
    {
      node_info->point_list = PRTREE3@T@NODE_LEAF(node).point;
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

  vsg_prtree_key3@t@_build_child (father_id, child_number, &node_info->id);

  node_info->parallel_status = node->parallel_status;
}

VsgPRTree3@t@Node *_vsg_prtree3@t@node_get_child_at (VsgPRTree3@t@Node *node,
                                                     const VsgVector3@t@ *pos,
                                                     gint depth)
{
  vsgloc3 i;
  VsgPRTree3@t@Node *child = NULL;

  if (depth == 0 || PRTREE3@T@NODE_ISLEAF(node)) return node;

  i = vsg_vector3@t@_vector3@t@_locfunc (&node->center, pos);

  child = PRTREE3@T@NODE_CHILD(node, i);

  return _vsg_prtree3@t@node_get_child_at (child, pos, depth - 1);
}

VsgPRTree3@t@Node *vsg_prtree3@t@node_key_lookup (VsgPRTree3@t@Node *node,
                                                  VsgPRTreeKey3@t@ key)
{
  vsgloc3 i;
  VsgPRTree3@t@Node *child = NULL;

  if (key.depth == 0 || PRTREE3@T@NODE_ISLEAF(node)) return node;

  i = vsg_prtree_key3@t@_child (&key);

  child = PRTREE3@T@NODE_CHILD(node, i);

  key.depth --;

  return vsg_prtree3@t@node_key_lookup (child, key);
}

static void recursive_near_func (VsgPRTree3@t@Node *one,
                                 VsgPRTree3@t@NodeInfo *one_info,
                                 VsgPRTree3@t@Node *other,
                                 VsgPRTree3@t@NodeInfo *other_info,
                                 VsgNFConfig3@t@ *nfc)
{
  vsgloc3 i;

  if (PRTREE3@T@NODE_ISINT (one))
    {
      for (i=0; i<8; i++)
        {
          VsgPRTree3@t@Node *one_child = PRTREE3@T@NODE_CHILD(one, i);
          VsgPRTree3@t@NodeInfo one_child_info;

#ifdef VSG_HAVE_MPI
          if (PRTREE3@T@NODE_IS_REMOTE (one_child)) continue;
#endif

          _vsg_prtree3@t@node_get_info (one_child, &one_child_info, one_info,
                                        i);

          recursive_near_func (one_child, &one_child_info, other, other_info,
                               nfc);
        }
    }
  else if (PRTREE3@T@NODE_ISINT (other))
    {
      for (i=0; i<8; i++)
        {
          VsgPRTree3@t@Node *other_child = PRTREE3@T@NODE_CHILD(other, i);
          VsgPRTree3@t@NodeInfo other_child_info;

#ifdef VSG_HAVE_MPI
          if (PRTREE3@T@NODE_IS_REMOTE (other_child)) continue;
#endif

          _vsg_prtree3@t@node_get_info (other_child, &other_child_info,
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

void vsg_prtree3@t@node_recursive_near_func (VsgPRTree3@t@Node *one,
                                             VsgPRTree3@t@NodeInfo *one_info,
                                             VsgPRTree3@t@Node *other,
                                             VsgPRTree3@t@NodeInfo *other_info,
                                             VsgNFConfig3@t@ *nfc)
{
  recursive_near_func (one, one_info, other, other_info, nfc);
}

#ifdef VSG_HAVE_MPI
#define _NODE_IS_EMPTY(node) ( \
 (node)->point_count == 0 && PRTREE3@T@NODE_IS_LOCAL (node) \
)
#else
#define _NODE_IS_EMPTY(node) ( \
 (node)->point_count == 0 \
)
#endif

static void
_sub_neighborhood_near_far_traversal (VsgNFConfig3@t@ *nfc,
                                      VsgPRTree3@t@Node *one,
                                      VsgPRTree3@t@NodeInfo *one_info,
                                      VsgPRTree3@t@Node *other,
                                      VsgPRTree3@t@NodeInfo *other_info,
                                      gint8 x, gint8 y, gint8 z)
{
  vsgloc3 i, j, si, sj;
  vsgloc3 sym[8] = {
    _SYMMETRY (0, x, y, z),
    _SYMMETRY (1, x, y, z),
    _SYMMETRY (2, x, y, z),
    _SYMMETRY (3, x, y, z),
    _SYMMETRY (4, x, y, z),
    _SYMMETRY (5, x, y, z),
    _SYMMETRY (6, x, y, z),
    _SYMMETRY (7, x, y, z),
  };

  if (_NODE_IS_EMPTY(one) || _NODE_IS_EMPTY(other))
    return;

#ifdef VSG_HAVE_MPI
  if (nfc->sz > 1)
    vsg_prtree3@t@_nf_check_receive (nfc, MPI_ANY_TAG, FALSE);
#endif

  if (PRTREE3@T@NODE_ISINT (one) && PRTREE3@T@NODE_ISINT (other))
    {
#ifdef VSG_HAVE_MPI
      if (nfc->sz > 1) vsg_prtree3@t@_nf_check_send (nfc);
#endif

      /* near/far interaction between one and other children */
      for (i=0; i<8; i++)
        {
          VsgPRTree3@t@Node *one_child = PRTREE3@T@NODE_CHILD(one, i);
          VsgPRTree3@t@NodeInfo one_child_info;

#ifdef VSG_HAVE_MPI
          if (PRTREE3@T@NODE_IS_REMOTE (one_child)) continue;
#endif

          _vsg_prtree3@t@node_get_info (one_child, &one_child_info,
                                        one_info, i);

          si = sym[i];

          for (j=0; j<8; j++)
            {
              gboolean far;
              VsgPRTree3@t@Node *other_child =
                PRTREE3@T@NODE_CHILD(other, j);
              VsgPRTree3@t@NodeInfo other_child_info;
              gboolean far_done = TRUE;

#ifdef VSG_HAVE_MPI
              if (PRTREE3@T@NODE_IS_REMOTE (other_child)) continue;
#endif

              _vsg_prtree3@t@node_get_info (other_child,
                                            &other_child_info,
                                            other_info, j);

              sj = sym[j];

              far = NEAR_FAR (x, y, z, si, sj);

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
                      if (PRTREE3@T@NODE_IS_SHARED (one_child) &&
                          PRTREE3@T@NODE_IS_SHARED (other_child) &&
                          PRTREE3@T@NODE_PROC (one_child) != nfc->rk)
                        continue;
#endif

                      far_done = nfc->far_func (&one_child_info, &other_child_info,
                                                nfc->user_data);
                      if (!far_done)
                        {
#ifdef VSG_HAVE_MPI
                          if (PRTREE3@T@NODE_IS_SHARED (one_child) ||
                              PRTREE3@T@NODE_IS_SHARED (other_child))
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
                  gint8 newz = _SUB_INTERACTION (i, j, z, _Z);

                  _sub_neighborhood_near_far_traversal (nfc, one_child,
                                                        &one_child_info,
                                                        other_child,
                                                        &other_child_info,
                                                        newx, newy, newz);
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
vsg_prtree3@t@node_near_far_traversal (VsgNFConfig3@t@ *nfc,
                                       VsgPRTree3@t@Node *node,
                                       VsgPRTree3@t@NodeInfo *father_info,
                                       vsgloc3 child_number,
                                       gboolean parallel_check)
{
  vsgloc3 i,j;
  VsgPRTree3@t@NodeInfo node_info;

#ifdef VSG_HAVE_MPI
  if (PRTREE3@T@NODE_IS_REMOTE (node)) return;
#endif
  if (_NODE_IS_EMPTY (node)) return;

  _vsg_prtree3@t@node_get_info (node, &node_info, father_info, child_number);

#ifdef VSG_HAVE_MPI
  /* check for remote processors to send this node, if needed */
  parallel_check = vsg_prtree3@t@_node_check_parallel_near_far (nfc,
                                                                node,
                                                                &node_info,
                                                                parallel_check);
#endif

  if (PRTREE3@T@NODE_ISLEAF (node))
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
      for (i=0; i<8; i++)
        vsg_prtree3@t@node_near_far_traversal (nfc,
                                               PRTREE3@T@NODE_CHILD(node, i),
                                               &node_info, i, parallel_check);

      /* interactions between node's children */
      for (i=0; i<8; i++)
        {
          VsgPRTree3@t@Node *one_child = PRTREE3@T@NODE_CHILD(node, i);
          VsgPRTree3@t@NodeInfo one_child_info;

#ifdef VSG_HAVE_MPI
          if (PRTREE3@T@NODE_IS_REMOTE (one_child)) continue;
#endif

          _vsg_prtree3@t@node_get_info (one_child, &one_child_info,
                                        &node_info, i);
          for (j=i+1; j<8; j++)
            {
              VsgPRTree3@t@Node *other_child =
                PRTREE3@T@NODE_CHILD(node, j);
              VsgPRTree3@t@NodeInfo other_child_info;
              gint8 x, y, z;

#ifdef VSG_HAVE_MPI
              if (PRTREE3@T@NODE_IS_REMOTE (other_child)) continue;
#endif

              x = _AXIS_DIFF (i, j, _X);
              y = _AXIS_DIFF (i, j, _Y);
              z = _AXIS_DIFF (i, j, _Z);

              _vsg_prtree3@t@node_get_info (other_child,
                                            &other_child_info,
                                            &node_info, j);

              _sub_neighborhood_near_far_traversal (nfc, one_child,
                                                    &one_child_info,
                                                    other_child,
                                                    &other_child_info,
                                                    x, y, z); 
            }
        }

    }
}

typedef enum _Hilbert3Key Hilbert3Key;
enum _Hilbert3Key {
  HK3_0_1_2,
  HK3_6_2_7,
  HK3_6_7_2,
  HK3_0_1_4,
  HK3_6_4_7,
  HK3_5_1_4,
  HK3_0_2_1,
  HK3_3_1_7,
  HK3_5_1_7,
  HK3_6_2_4,
  HK3_3_2_1,
  HK3_3_7_1,
  HK3_5_7_4,
  HK3_6_4_2,
  HK3_0_2_4,
  HK3_3_7_2,
  HK3_5_7_1,
  HK3_5_4_1,
  HK3_3_2_7,
  HK3_0_4_2,
  HK3_0_4_1,
  HK3_6_7_4,
  HK3_3_1_2,
  HK3_5_4_7,
};

static gint hilbert3_coords[][8] = {
  {0, 1, 5, 4, 6, 7, 3, 2, },
  {6, 2, 0, 4, 5, 1, 3, 7, },
  {6, 7, 5, 4, 0, 1, 3, 2, },
  {0, 1, 3, 2, 6, 7, 5, 4, },
  {6, 4, 0, 2, 3, 1, 5, 7, },
  {5, 1, 3, 7, 6, 2, 0, 4, },
  {0, 2, 6, 4, 5, 7, 3, 1, },
  {3, 1, 0, 2, 6, 4, 5, 7, },
  {5, 1, 0, 4, 6, 2, 3, 7, },
  {6, 2, 3, 7, 5, 1, 0, 4, },
  {3, 2, 6, 7, 5, 4, 0, 1, },
  {3, 7, 6, 2, 0, 4, 5, 1, },
  {5, 7, 3, 1, 0, 2, 6, 4, },
  {6, 4, 5, 7, 3, 1, 0, 2, },
  {0, 2, 3, 1, 5, 7, 6, 4, },
  {3, 7, 5, 1, 0, 4, 6, 2, },
  {5, 7, 6, 4, 0, 2, 3, 1, },
  {5, 4, 6, 7, 3, 2, 0, 1, },
  {3, 2, 0, 1, 5, 4, 6, 7, },
  {0, 4, 5, 1, 3, 7, 6, 2, },
  {0, 4, 6, 2, 3, 7, 5, 1, },
  {6, 7, 3, 2, 0, 1, 5, 4, },
  {3, 1, 5, 7, 6, 4, 0, 2, },
  {5, 4, 0, 1, 3, 2, 6, 7, },
};

static Hilbert3Key hilbert3_decompositions[][8] = {
  {HK3_0_2_1, HK3_0_2_4, HK3_0_1_2, HK3_3_2_7, HK3_5_4_1, HK3_0_1_2, HK3_6_4_2,
   HK3_3_1_2, },
  {HK3_6_7_2, HK3_6_7_4, HK3_6_2_7, HK3_3_7_1, HK3_0_4_2, HK3_6_2_7, HK3_5_4_7,
   HK3_3_2_7, },
  {HK3_6_2_7, HK3_6_2_4, HK3_6_7_2, HK3_3_2_1, HK3_5_4_7, HK3_6_7_2, HK3_0_4_2,
   HK3_3_7_2, },
  {HK3_0_4_1, HK3_0_4_2, HK3_0_1_4, HK3_5_4_7, HK3_3_2_1, HK3_0_1_4, HK3_6_2_4,
   HK3_5_1_4, },
  {HK3_6_7_4, HK3_6_7_2, HK3_6_4_7, HK3_5_7_1, HK3_0_2_4, HK3_6_4_7, HK3_3_2_7,
   HK3_5_4_7, },
  {HK3_5_4_1, HK3_5_4_7, HK3_5_1_4, HK3_0_4_2, HK3_3_7_1, HK3_5_1_4, HK3_6_7_4,
   HK3_0_1_4, },
  {HK3_0_1_2, HK3_0_1_4, HK3_0_2_1, HK3_3_1_7, HK3_6_4_2, HK3_0_2_1, HK3_5_4_1,
   HK3_3_2_1, },
  {HK3_3_7_1, HK3_3_7_2, HK3_3_1_7, HK3_5_7_4, HK3_0_2_1, HK3_3_1_7, HK3_6_2_7,
   HK3_5_1_7, },
  {HK3_5_7_1, HK3_5_7_4, HK3_5_1_7, HK3_3_7_2, HK3_0_4_1, HK3_5_1_7, HK3_6_4_7,
   HK3_3_1_7, },
  {HK3_6_4_2, HK3_6_4_7, HK3_6_2_4, HK3_0_4_1, HK3_3_7_2, HK3_6_2_4, HK3_5_7_4,
   HK3_0_2_4, },
  {HK3_3_1_2, HK3_3_1_7, HK3_3_2_1, HK3_0_1_4, HK3_6_7_2, HK3_3_2_1, HK3_5_7_1,
   HK3_0_2_1, },
  {HK3_3_1_7, HK3_3_1_2, HK3_3_7_1, HK3_5_1_4, HK3_6_2_7, HK3_3_7_1, HK3_0_2_1,
   HK3_5_7_1, },
  {HK3_5_4_7, HK3_5_4_1, HK3_5_7_4, HK3_6_4_2, HK3_3_1_7, HK3_5_7_4, HK3_0_1_4,
   HK3_6_7_4, },
  {HK3_6_2_4, HK3_6_2_7, HK3_6_4_2, HK3_0_2_1, HK3_5_7_4, HK3_6_4_2, HK3_3_7_2,
   HK3_0_4_2, },
  {HK3_0_4_2, HK3_0_4_1, HK3_0_2_4, HK3_6_4_7, HK3_3_1_2, HK3_0_2_4, HK3_5_1_4,
   HK3_6_2_4, },
  {HK3_3_2_7, HK3_3_2_1, HK3_3_7_2, HK3_6_2_4, HK3_5_1_7, HK3_3_7_2, HK3_0_1_2,
   HK3_6_7_2, },
  {HK3_5_1_7, HK3_5_1_4, HK3_5_7_1, HK3_3_1_2, HK3_6_4_7, HK3_5_7_1, HK3_0_4_1,
   HK3_3_7_1, },
  {HK3_5_1_4, HK3_5_1_7, HK3_5_4_1, HK3_0_1_2, HK3_6_7_4, HK3_5_4_1, HK3_3_7_1,
   HK3_0_4_1, },
  {HK3_3_7_2, HK3_3_7_1, HK3_3_2_7, HK3_6_7_4, HK3_0_1_2, HK3_3_2_7, HK3_5_1_7,
   HK3_6_2_7, },
  {HK3_0_2_4, HK3_0_2_1, HK3_0_4_2, HK3_6_2_7, HK3_5_1_4, HK3_0_4_2, HK3_3_1_2,
   HK3_6_4_2, },
  {HK3_0_1_4, HK3_0_1_2, HK3_0_4_1, HK3_5_1_7, HK3_6_2_4, HK3_0_4_1, HK3_3_2_1,
   HK3_5_4_1, },
  {HK3_6_4_7, HK3_6_4_2, HK3_6_7_4, HK3_5_4_1, HK3_3_2_7, HK3_6_7_4, HK3_0_2_4,
   HK3_5_7_4, },
  {HK3_3_2_1, HK3_3_2_7, HK3_3_1_2, HK3_0_2_4, HK3_5_7_1, HK3_3_1_2, HK3_6_7_2,
   HK3_0_1_2, },
  {HK3_5_7_4, HK3_5_7_1, HK3_5_4_7, HK3_6_7_2, HK3_0_1_4, HK3_5_4_7, HK3_3_1_7,
   HK3_6_4_7, },
};

static void hilbert3_order (gpointer node_key, gint *children,
                            gpointer *children_keys)
{
  gint i;
  Hilbert3Key hkey = GPOINTER_TO_INT (node_key);

  for (i=0; i<8; i++)
    {
      children[i] = hilbert3_coords[hkey][i];
      children_keys[i] = GINT_TO_POINTER (hilbert3_decompositions[hkey][i]);
    }
}


/*-------------------------------------------------------------------*/
/* typedefs and structure doc */
/*-------------------------------------------------------------------*/
/**
 * VsgPRTree3@t@NodeInfo:
 * @lbound: A copy of the lower left corner of the node's box (lower bound point).
 * @ubound: A copy of the upper right corner of the node's box (upper bound point).
 * @center: A copy of the center of the node's box.
 * @point_list: the list of #VsgPoint3 contained in this node if it's a leaf node. %NULL otherwise.
 * @region_list: the list of #VsgRegion3 contained in this node.
 * @point_count: the total number of #VsgPoint3 contained in this subtree.
 * @region_count: the total number of #VsgRegion3 contained in this subtree.
 * @depth: node's depth in the tree.
 * @user_data: the data eventually attached to this node.
 * @father_info: link to this node's father's #VsgPRTree3@t@NodeInfo. NULL if root node.
 * @isleaf: a #gboolean set if the node is a leaf.
 * @id: the key representing this node in the tree hierarchy.
 * @parallel_status: parallel status of this node.
 *
 * A structure provided to expose a #VsgPrtree3@t@ node during a traversal
 * process. Since @user_data and @children_user_data are a user controlled
 * feature (set by the vsg_prtree3@t@_set_node_data() function), it's up to
 * the user himself to decide if it's safe to modify these fields or not
 * during a traversal.
 *
 * WARNING: Please, note that modifying @point_list, @region_list will lead
 * to undefined behaviour.
 */

/**
 * VsgPRTree3@t@Func:
 * @node_info: a #VsgPRTree3@t@NodeInfo.
 * @user_data: user provided data.
 *
 * A type of functions for #VsgPRTree3@t@ nodes traversals.
 */

/**
 * VsgPRTree3@t@InteractionFunc:
 * @one_info: a #VsgPRTree3@t@NodeInfo.
 * @other_info: a #VsgPRTree3@t@NodeInfo.
 * @user_data: user provided data.
 *
 * A type of functions for #VsgPRTree3@t@ nodes interaction traversals.
 */

/**
 * VsgPRTree3@t@FarInteractionFunc:
 * @one_info: a #VsgPRTree3@t@NodeInfo.
 * @other_info: a #VsgPRTree3@t@NodeInfo.
 * @user_data: user provided data.
 *
 * A type of functions for #VsgPRTree3@t@ nodes interaction traversals. Used
 * for far interactions, the function should return %TRUE when it effectively
 * realizes the far interaction computations. Alternatively, the function can
 * return %FALSE if the far interaction won't be computed (for any reason).
 * In this case, vsg_prtree3@t@_near_far_traversal() will fallback to near
 * interaction evaluation.
 *
 * Returns: Confirmation that the interaction really occurred.
 */

/*-------------------------------------------------------------------*/
/* public functions */
/*-------------------------------------------------------------------*/

/**
 * vsg_prtree3@t@_near_far_traversal:
 * @prtree3@t@: a #VsgPRTree3@t@.
 * @far_func: the far interaction user provided function.
 * @near_func: the near interaction user provided function.
 * @user_data: the data to pass to @far_fun and @near_func.
 *
 * Computes an interaction traversal for @prtree3@t@. This means that
 * @near_func will be called once for every couple of neighbour @prtree3@t@
 * leaf nodes (this is reflexive) and @far_func will be called once for every
 * couple of non neighbour nodes which parents are neighbours. If @far_func
 * happens to return %FALSE, the far interaction would be considered not
 * treated. In this case, @near_func will be called on every pair of @far_func
 * actual nodes arguments' descendants.
 * 
 * This kind of traversal is mainly provided for multipole algorithms.
 *
 * WARNING: Please note that @near_func and @far_func will only be called on
 * nodes that contain at least one #VsgPoint3.
 */
void
vsg_prtree3@t@_near_far_traversal (VsgPRTree3@t@ *prtree3@t@,
                                   VsgPRTree3@t@FarInteractionFunc far_func,
                                   VsgPRTree3@t@InteractionFunc near_func,
                                   gpointer user_data)
{
  VsgNFConfig3@t@ nfc;

#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
#endif

#ifdef VSG_HAVE_MPI
  /* vsg_packed_msg_trace ("enter 1 [nf traversal]"); */

  vsg_nf_config3@t@_init (&nfc, prtree3@t@, far_func, near_func, user_data);

  vsg_nf_config3@t@_tmp_alloc (&nfc, &prtree3@t@->config);

  if (nfc.sz > 1 && prtree3@t@->config.remote_depth_dirty)
    {
      vsg_prtree3@t@_update_remote_depths (prtree3@t@);
    }
#else

  nfc.tree = prtree3@t@;
  nfc.far_func = far_func;
  nfc.near_func = near_func;
  nfc.user_data = user_data;

#endif

  vsg_prtree3@t@node_near_far_traversal (&nfc,
                                         prtree3@t@->node,
                                         NULL, 0, TRUE);

#ifdef VSG_HAVE_MPI
  /* vsg_packed_msg_trace ("leave 1 [nf traversal]"); */
  /* vsg_packed_msg_trace ("enter 1 [nf parallel_end]"); */

  if (nfc.sz > 1)
    {
      vsg_prtree3@t@_nf_check_parallel_end (&nfc);
    }

  vsg_nf_config3@t@_tmp_free (&nfc, &prtree3@t@->config);

  vsg_nf_config3@t@_clean (&nfc);

  /* vsg_packed_msg_trace ("leave 1 [nf parallel_end]"); */
#endif
}

/**
 * vsg_prtree3@t@_set_children_order_hilbert:
 * @prtree3@t@: a #VsgPRTree3@t@.
 *
 * Configures @prtree3@t@ for Hilbert curve order traversal.
 */
void vsg_prtree3@t@_set_children_order_hilbert (VsgPRTree3@t@ *prtree3@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
#endif

  vsg_prtree3@t@_set_children_order (prtree3@t@, hilbert3_order,
                                     GINT_TO_POINTER (HK3_0_2_1));

}
/**
 * vsg_prtree3@t@_set_children_order_default:
 * @prtree3@t@: a #VsgPRTree3@t@.
 *
 * Configures @prtree3@t@ for default (Z order) order traversal.
 */
void vsg_prtree3@t@_set_children_order_default (VsgPRTree3@t@ *prtree3@t@)
{
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
#endif

  vsg_prtree3@t@_set_children_order (prtree3@t@, NULL, NULL);
}
