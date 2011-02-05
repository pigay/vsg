/* basic point insertion, removal and traversal on a cloned VsgPRTree3d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

static void increment (VsgVector3d *pt, gint *count)
{
  (*count) ++;
}

/* tree traversal function that accumulates the total number of points */
static void traverse_point_count (VsgPRTree3dNodeInfo *node_info, gint *count)
{
  g_slist_foreach (node_info->point_list, (GFunc) increment, count);
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  VsgVector3d lb = {-1., -1., -1.};
  VsgVector3d ub = {1., 1., 1.};

  VsgVector3d points[] = {
    {-0.5, -0.5, -.5},
    {0.5, -0.5, -.5},
    {-0.5, 0.5, .5},
    {0.25, 0.25, .5},
    {0.75, 0.75, .5},
    {1.75, 1.75, 1.}, /* exterior point */
    {1.75, 100.75, 1.}, /* another exterior point */
    {1.75, -100.75, 1.}, /* another exterior point */
  };

  const guint n = sizeof (points) / sizeof (VsgVector3d);

  VsgPRTree3d *tree;
  VsgPRTree3d *tree_clone;
  gint i;

  if (argc > 1 && g_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  vsg_init_gdouble ();

  /* create the tree */
  tree =
    vsg_prtree3d_new_full (&lb, &ub,
                           (VsgPoint3dLocFunc) vsg_vector3d_vector3d_locfunc,
                           (VsgPoint3dDistFunc) vsg_vector3d_dist,
                           NULL, 1);

  /* insert some points */
  for (i=0; i<n; i++)
    {
      vsg_prtree3d_insert_point (tree, &points[i]);
    }

  /* clone the tree and destroy it */
  tree_clone = vsg_prtree3d_clone (tree);
  vsg_prtree3d_free (tree);

  /* do some traversal */
  i=0;
  vsg_prtree3d_traverse (tree_clone, G_PRE_ORDER,
                         (VsgPRTree3dFunc) traverse_point_count, &i);

  /* check the results */
  if (i != n)
    {
      g_printerr ("ERROR: traverse point count %d (should be %d)\n",
                  i, n);

      ret ++;
    }

  /* remove the points */
  for (i=0; i<n; i++)
    {
      vsg_prtree3d_remove_point (tree_clone, &points[i]);
    }

  /* destroy the cloned tree */
  vsg_prtree3d_free (tree_clone);

  return ret;
}
