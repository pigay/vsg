/* basic point insertion, removal and traversal on VsgPRTree3d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>


static void increment (VsgVector3d *pt, gint *count)
{
  (*count) ++;
}

static void write (VsgVector3d *pt, FILE *file)
{
  vsg_vector3d_write (pt, file);
  fprintf (file, "\n");
}

/* tree traversal function that accumulates the total number of points */
static void traverse_point_count (VsgPRTree3dNodeInfo *node_info, gint *count)
{
  g_slist_foreach (node_info->point_list, (GFunc) increment, count);
  g_slist_foreach (node_info->point_list, (GFunc) write, stdout);
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  VsgVector3d lb = {-1., -1., -1.};
  VsgVector3d ub = {1., 1., 1.};

  VsgVector3d *points;
  gint lvl = 1;
  gint n, np;

  VsgPRTree3d *tree;
  gint i, j, k, l;

  if (argc > 1 && g_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  if (argc > 1)
    {
      sscanf (argv[1], "%d", &lvl);
    }

  n = 1<<lvl;
  np = n*n*n;

  vsg_init_gdouble ();

  /* create the points */
  points = g_malloc (np * sizeof (VsgVector3d));
  l = 0;
  for (i=0; i<n; i++)
    {
      gdouble x = 2. * ((i+0.5)/n) - 1.;

      for (j=0; j<n; j++)
        {
          gdouble y = 2. * ((j+0.5)/n) - 1.;

          for (k=0; k<n; k++)
            {
              gdouble z = 2. * ((k+0.5)/n) - 1.;

              vsg_vector3d_set (&points[l], x, y, z);
              l ++;
            }
        }
    }

  /* create the tree */
  tree =
    vsg_prtree3d_new_full (&lb, &ub,
                           (VsgPoint3dLocFunc) vsg_vector3d_vector3d_locfunc,
                           (VsgPoint3dDistFunc) vsg_vector3d_dist,
                           NULL, 1);

  /* configure for hilbert curve order traversal */
  vsg_prtree3d_set_children_order_hilbert (tree);

  /* insert some points */
  for (i=0; i<np; i++)
    {
      vsg_prtree3d_insert_point (tree, &points[i]);
    }

  /* do some traversal */
  i=0;
  vsg_prtree3d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree3dFunc) traverse_point_count, &i);

  /* check the results */
  if (i != np)
    {
      g_printerr ("ERROR: traverse point count %d (should be %d)\n",
                  i, n);

      ret ++;
    }

  /* remove the points */
  for (i=0; i<np; i++)
    {
      vsg_prtree3d_remove_point (tree, &points[i]);
    }

  g_free (points);

  /* destroy the tree */
  vsg_prtree3d_free (tree);

  return ret;
}
