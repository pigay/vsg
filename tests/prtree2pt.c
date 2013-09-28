/* basic point insertion, removal and traversal on VsgPRTree2d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

static void increment (VsgVector2d *pt, gint *count)
{
  (*count) ++;
}

/* tree traversal function that accumulates the total number of points */
static void traverse_point_count (VsgPRTree2dNodeInfo *node_info, gint *count)
{
  g_slist_foreach (node_info->point_list, (GFunc) increment, count);
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  VsgVector2d lb = {-1., -1.};
  VsgVector2d ub = {1., 1.};

  VsgVector2d points[] = {
    {-0.5, -0.5},
    {0.5, -0.5},
    {-0.5, 0.5},
    {0.25, 0.25},
    {0.75, 0.75},
    {1.75, 1.75}, /* exterior point */
    {1.75, 100.75}, /* another exterior point */
    {1.75, -100.75}, /* another exterior point */
  };

  const guint n = sizeof (points) / sizeof (VsgVector2d);

  VsgPRTree2d *tree;
  gint i;

  if (argc > 1 && g_ascii_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  vsg_init_gdouble ();

  /* create the tree */
  tree =
    vsg_prtree2d_new_full (&lb, &ub,
                           (VsgPoint2dLocFunc) vsg_vector2d_vector2d_locfunc,
                           (VsgPoint2dDistFunc) vsg_vector2d_dist,
                           NULL, 1);

  /* insert some points */
  for (i=0; i<n; i++)
    {
      vsg_prtree2d_insert_point (tree, &points[i]);
    }

  /* do some traversal */
  i=0;
  vsg_prtree2d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree2dFunc) traverse_point_count, &i);

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
      vsg_prtree2d_remove_point (tree, &points[i]);
    }

  /* destroy the tree */
  vsg_prtree2d_free (tree);

  return ret;
}
