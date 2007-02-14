/* basic near/far traversal check. This test simulates a tree algorithm. */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

typedef struct _Pt Pt;
typedef struct _NodeCounter NodeCounter;

struct _Pt
{
  VsgVector2d vector;
  gint count;
};

struct _NodeCounter
{
  gint in_count;
  gint out_count;
};

#define TYPE_NODE_COUNTER (node_counter_get_type ())

NodeCounter *node_counter_clone (NodeCounter *src)
{
  return g_memdup (src, sizeof (NodeCounter));
}
void node_counter_free (NodeCounter *counter)
{
  g_free (counter);
}

GType node_counter_get_type ()
{
  static GType type = G_TYPE_NONE;

  if (G_UNLIKELY (type == G_TYPE_NONE))
    {
      type =
	g_boxed_type_register_static ("TestNodeCounter",
				      (GBoxedCopyFunc) node_counter_clone,
				      (GBoxedFreeFunc) node_counter_free);
    }

  return type;
}


static void pt_add_count (Pt *pt, gint *count)
{
  pt->count += *count;
}

static void near (VsgPRTree2dNodeInfo *one_info,
                  VsgPRTree2dNodeInfo *other_info,
                  gpointer data)
{
  g_slist_foreach (one_info->point_list, (GFunc) pt_add_count,
                   &other_info->point_count);

  if (one_info != other_info)
    g_slist_foreach (other_info->point_list, (GFunc) pt_add_count,
                     &one_info->point_count);
}

static guint far_stopped = 0;

static gboolean far (VsgPRTree2dNodeInfo *one_info,
		     VsgPRTree2dNodeInfo *other_info,
		     gint *maxcount)
{
  if (one_info->point_count <= *maxcount ||
      other_info->point_count <= *maxcount)
    {
      far_stopped ++;
      /* stop far count transfer and switch to near interaction */
      return FALSE;
    }

  ((NodeCounter *) one_info->user_data)->out_count +=
    ((NodeCounter *) other_info->user_data)->in_count;

  ((NodeCounter *) other_info->user_data)->out_count +=
    ((NodeCounter *) one_info->user_data)->in_count;

  return TRUE;
}

static void up (VsgPRTree2dNodeInfo *node_info, gpointer data)
{
  if (node_info->isleaf)
    {
      ((NodeCounter *) node_info->user_data)->in_count +=
        node_info->point_count;
    }

  if (node_info->father_info)
    {
      ((NodeCounter *) node_info->father_info->user_data)->in_count +=
        ((NodeCounter *) node_info->user_data)->in_count;
    }
}

static void down (VsgPRTree2dNodeInfo *node_info, gpointer data)
{
  gint count;

  if (node_info->father_info)
    {
      ((NodeCounter *) node_info->user_data)->out_count +=
        ((NodeCounter *) node_info->father_info->user_data)->out_count;
    }

  count = ((NodeCounter *) node_info->user_data)->out_count;

  g_slist_foreach (node_info->point_list, (GFunc) pt_add_count,
                   &count);
}

/* create a circle */
static Pt *create_points (guint n)
{
  Pt *points = g_malloc0 (n * sizeof (Pt));
  gint i;
  const gdouble r = 0.95;
  gdouble dtheta = 2. * G_PI / (n-1);

  for (i=0; i<n; i++)
    {
      points[i].vector.x = r * cos (i * dtheta);
      points[i].vector.y = r * sin (i * dtheta);
      points[i].count = 0;
    }
  return points;
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  VsgVector2d lb = {-1., -1.};
  VsgVector2d ub = {1., 1.};
  NodeCounter counter = {0, 0};
  const guint n = 10000;
  gint maxpoint = 5;
  gint maxcount = 2*maxpoint;

  Pt *points;

  VsgPRTree2d *tree;
  gint i;

  if (argc > 1 && g_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  vsg_init_gdouble ();

  /* create the points */
  points = create_points (n);

  /* create the tree */
  tree = vsg_prtree2d_new_full (&lb, &ub,
                                (VsgPoint2dLocFunc) vsg_vector2d_vector2d_locfunc,
                                (VsgPoint2dDistFunc) vsg_vector2d_dist,
                                NULL, maxpoint);

  vsg_prtree2d_set_node_data (tree, TYPE_NODE_COUNTER, &counter);

  /* insert the points into the tree */
  for (i=0; i<n; i++)
    {
      vsg_prtree2d_insert_point (tree, &points[i]);
    }

  /* accumulate the point counts across the tree */
  vsg_prtree2d_traverse (tree, G_POST_ORDER, (VsgPRTree2dFunc) up, NULL);

  /* do another near/far traversal */
  vsg_prtree2d_near_far_traversal (tree,
				   (VsgPRTree2dFarInteractionFunc) far,
				   (VsgPRTree2dInteractionFunc) near,
				   &maxcount);

  /*  distribute back the point counts across the tree */
  vsg_prtree2d_traverse (tree, G_PRE_ORDER, (VsgPRTree2dFunc) down, NULL);

  /* check the results */
  for (i=0; i<n; i++)
    {
      if (points[i].count != n)
        {
          g_printerr ("ERROR: wrong count on point %d: %d (should be %d).\n",
                      i, points[i].count, n);
          ret ++;
        }
    }

  if (far_stopped == 0)
    {
      g_printerr ("no far_func stopped in %s\n", __FILE__);
      ret ++;
    }

  /* remove the points */
  for (i=0; i<n; i++)
    {
      vsg_prtree2d_remove_point (tree, &points[i]);
    }
  g_free (points);

  /* destroy the tree */
  vsg_prtree2d_free (tree);

  return ret;
}
