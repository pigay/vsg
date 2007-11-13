/* basic point insertion, removal and traversal on VsgPRTree2d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

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

static void increment (VsgVector2d *pt, gint *count)
{
  (*count) ++;
}

static void write (VsgVector2d *pt, FILE *file)
{
  vsg_vector2d_write (pt, file);
  fprintf (file, "\n");
}

/* tree traversal function that accumulates the total number of points */
static void traverse_point_count (VsgPRTree2dNodeInfo *node_info, gint *count)
{
  g_slist_foreach (node_info->point_list, (GFunc) increment, count);
  g_slist_foreach (node_info->point_list, (GFunc) write, stdout);
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  VsgVector2d lb = {-1., -1.};
  VsgVector2d ub = {1., 1.};

  VsgVector2d *points;
  gint lvl = 1;
  gint n, np;

  VsgPRTree2d *tree;
  gint i, j, k;

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
  np = n*n;

  vsg_init_gdouble ();

  /* create the points */
  points = g_malloc (np * sizeof (VsgVector2d));
  k = 0;
  for (i=0; i<n; i++)
    {
      gdouble x = 2. * ((i+0.5)/n) - 1.;

      for (j=0; j<n; j++)
        {
          gdouble y = 2. * ((j+0.5)/n) - 1.;

          vsg_vector2d_set (&points[k], x, y);
          k ++;
        }
    }

  /* create the tree */
  tree =
    vsg_prtree2d_new_full (&lb, &ub,
                           (VsgPoint2dLocFunc) vsg_vector2d_vector2d_locfunc,
                           (VsgPoint2dDistFunc) vsg_vector2d_dist,
                           NULL, 1);

  /* configure for hilbert curve order traversal */
  vsg_prtree2d_set_children_order (tree, hilbert2_order,
                                   GINT_TO_POINTER (HK2_0_1));

  /* insert some points */
  for (i=0; i<np; i++)
    {
      vsg_prtree2d_insert_point (tree, &points[i]);
    }

  /* do some traversal */
  i=0;
  vsg_prtree2d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree2dFunc) traverse_point_count, &i);

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
      vsg_prtree2d_remove_point (tree, &points[i]);
    }

  g_free (points);

  /* destroy the tree */
  vsg_prtree2d_free (tree);

  return ret;
}
