/* basic point insertion, removal and traversal on VsgPRTree3d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

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
  vsg_prtree3d_set_children_order (tree, hilbert3_order,
                                   GINT_TO_POINTER (HK3_0_2_1));

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
