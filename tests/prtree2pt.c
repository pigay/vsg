#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

static void traverse_point_count (VsgPRTree2dNodeInfo *node_info, gint *count)
{
  if (node_info->isleaf)
    *count += node_info->point_count;
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
  };

  const guint n = sizeof (points) / sizeof (VsgVector2d);

  VsgPRTree2d *tree;
  gint i;

  if (argc > 1 && g_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  tree = vsg_prtree2d_new (&lb, &ub, NULL);

  for (i=0; i<n; i++)
    {
      vsg_prtree2d_insert_point (tree, &points[i]);
    }

  i=0;

  vsg_prtree2d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree2dFunc) traverse_point_count, &i);

  if (i != n)
    {
      g_printerr ("ERROR: traverse point count %d (should be %d)\n",
                  i, n);

      ret ++;
    }

  for (i=0; i<n; i++)
    {
      vsg_prtree2d_remove_point (tree, &points[i]);
    }

  vsg_prtree2d_free (tree);

  return ret;
}
