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
                                   VsgPRTree3@t@NodeInfo *father_info)
{
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
}

static void recursive_near_func (VsgPRTree3@t@Node *one,
                                 VsgPRTree3@t@NodeInfo *one_info,
                                 VsgPRTree3@t@Node *other,
                                 VsgPRTree3@t@NodeInfo *other_info,
                                 VsgPRTree3@t@InteractionFunc near_func,
                                 gpointer user_data)
{
  vsgloc3 i;

  if (PRTREE3@T@NODE_ISINT (one))
    {
      for (i=0; i<8; i++)
        {
          VsgPRTree3@t@Node *one_child = PRTREE3@T@NODE_CHILD(one, i);
          VsgPRTree3@t@NodeInfo one_child_info;
          _vsg_prtree3@t@node_get_info (one_child, &one_child_info,
                                        one_info);

          recursive_near_func (one_child, &one_child_info, other, other_info,
                               near_func, user_data);
        }
    }
  else if (PRTREE3@T@NODE_ISINT (other))
    {
      for (i=0; i<8; i++)
        {
          VsgPRTree3@t@Node *other_child = PRTREE3@T@NODE_CHILD(other, i);
          VsgPRTree3@t@NodeInfo other_child_info;
          _vsg_prtree3@t@node_get_info (other_child, &other_child_info,
                                        other_info);

          recursive_near_func (one, one_info, other_child, &other_child_info,
                               near_func, user_data);
        }
    }
  else
    {
      /* near interaction between one and other */
      near_func (one_info, other_info, user_data);
    }


}

static void
_sub_neighborhood_near_far_traversal (VsgPRTree3@t@Node *one,
                                      VsgPRTree3@t@NodeInfo *one_info,
				      VsgPRTree3@t@Node *other,
                                      VsgPRTree3@t@NodeInfo *other_info,
				      gint8 x, gint8 y, gint8 z,
				      VsgPRTree3@t@FarInteractionFunc far_func,
				      VsgPRTree3@t@InteractionFunc near_func,
				      gpointer user_data)
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

  if (one->point_count==0 || other->point_count==0)
    return;

  if (PRTREE3@T@NODE_ISINT (one) && PRTREE3@T@NODE_ISINT (other))
    {
      /* near/far interaction between one and other children */

      for (i=0; i<8; i++)
        {
          VsgPRTree3@t@Node *one_child = PRTREE3@T@NODE_CHILD(one, i);
          VsgPRTree3@t@NodeInfo one_child_info;
          _vsg_prtree3@t@node_get_info (one_child, &one_child_info,
                                        one_info);

          si = sym[i];

          for (j=0; j<8; j++)
            {
              gboolean far;
              VsgPRTree3@t@Node *other_child =
                PRTREE3@T@NODE_CHILD(other, j);
              VsgPRTree3@t@NodeInfo other_child_info;

              _vsg_prtree3@t@node_get_info (other_child,
                                            &other_child_info,
                                            other_info);

              sj = sym[j];

              far = NEAR_FAR (x, y, z, si, sj);

	      if (far)
                {
                  /* far interaction between one and other */
                  if (far_func &&
                      one_child_info.point_count != 0 &&
                      other_child_info.point_count != 0)
                    {
                      gboolean far_done;
                      far_done = far_func (&one_child_info, &other_child_info,
					   user_data);
		      if (!far_done)
			{
			  /* near interaction between one and other */
			  recursive_near_func (one_child, &one_child_info,
					       other_child, &other_child_info,
					       near_func,
					       user_data);
			}
                    }
                }
	      else
		{
		  gint8 newx = _SUB_INTERACTION (i, j, x, _X);
		  gint8 newy = _SUB_INTERACTION (i, j, y, _Y);
		  gint8 newz = _SUB_INTERACTION (i, j, z, _Z);

		  _sub_neighborhood_near_far_traversal (one_child,
							&one_child_info,
							other_child,
							&other_child_info,
							newx, newy, newz,
							far_func,
							near_func,
							user_data);
		}
	    }
        }

    }
  else
    {
      if (near_func)
        {
          /* near interaction between one and other */
          recursive_near_func (one, one_info, other, other_info, near_func,
                               user_data);
        }
    }
}

static void
vsg_prtree3@t@node_near_far_traversal (VsgPRTree3@t@Node *node,
                                       VsgPRTree3@t@NodeInfo *father_info,
                                       VsgPRTree3@t@FarInteractionFunc far_func,
                                       VsgPRTree3@t@InteractionFunc near_func,
                                       gpointer user_data)
{
  vsgloc3 i,j;
  VsgPRTree3@t@NodeInfo node_info;

  _vsg_prtree3@t@node_get_info (node, &node_info, father_info);

  if (node->point_count==0) return;

  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      if (near_func)
	{
	  /* reflexive near interaction on node */
	  near_func (&node_info, &node_info, user_data);
	}
    }
  else
    {
      /* interactions between node's children */
      for (i=0; i<8; i++)
        {
          VsgPRTree3@t@Node *one_child = PRTREE3@T@NODE_CHILD(node, i);
          VsgPRTree3@t@NodeInfo one_child_info;
          _vsg_prtree3@t@node_get_info (one_child, &one_child_info,
                                        &node_info);
          for (j=i+1; j<8; j++)
            {
              VsgPRTree3@t@Node *other_child =
                PRTREE3@T@NODE_CHILD(node, j);
              VsgPRTree3@t@NodeInfo other_child_info;
	      gint8 x, y, z;
	      x = _AXIS_DIFF (i, j, _X);
	      y = _AXIS_DIFF (i, j, _Y);
	      z = _AXIS_DIFF (i, j, _Z);


              _vsg_prtree3@t@node_get_info (other_child,
                                            &other_child_info,
                                            &node_info);

	      _sub_neighborhood_near_far_traversal (one_child,
						    &one_child_info,
						    other_child,
						    &other_child_info,
						    x, y, z,
						    far_func, near_func,
						    user_data); 
            }
        }

      /* interactions in node's children descendants */
      for (i=0; i<8; i++)
        vsg_prtree3@t@node_near_far_traversal (PRTREE3@T@NODE_CHILD(node, i),
                                               &node_info,
                                               far_func, near_func,
                                               user_data);
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
#ifdef VSG_CHECK_PARAMS
  g_return_if_fail (prtree3@t@ != NULL);
#endif

  vsg_prtree3@t@node_near_far_traversal (prtree3@t@->node,
                                         NULL,
                                         far_func, near_func,
                                         user_data);
 
}
