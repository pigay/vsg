#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

typedef struct _Pt Pt;

struct _Pt
{
  VsgVector2d vector;
  gint count;
};

static void pt_add_count (Pt *pt, gint *count)
{
  pt->count += *count;
}

static void near (VsgPRTree2dNodeInfo *one_info,
                  VsgPRTree2dNodeInfo *other_info,
                  gint *err)
{
  if (!(one_info->isleaf && other_info->isleaf))
    {
      g_printerr ("ERROR: call for near_func on non leaf node\n");
      (*err) ++;
    }

  g_slist_foreach (one_info->point_list, (GFunc) pt_add_count,
                   &other_info->point_count);

  if (one_info != other_info)
    g_slist_foreach (other_info->point_list, (GFunc) pt_add_count,
                     &one_info->point_count);
}

static void far (VsgPRTree2dNodeInfo *one_info,
                 VsgPRTree2dNodeInfo *other_info,
                 gint *err)
{
  if (one_info->isleaf || other_info->isleaf)
    {
      g_printerr ("ERROR: call for far_func on  leaf node\n");
      (*err) ++;
    }
  
  g_slist_foreach (one_info->point_list, (GFunc) pt_add_count,
                   &other_info->point_count);
  g_slist_foreach (other_info->point_list, (GFunc) pt_add_count,
                   &one_info->point_count);
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  VsgVector2d lb = {-1., -1.};
  VsgVector2d ub = {1., 1.};

  Pt points[] = {
    {{-0.75, -0.75}, 0},
    {{-0.25, -0.25}, 0},
    {{0.5, -0.5}, 0},
    {{-0.5, 0.5}, 0},
    {{0.25, 0.25}, 0},
    {{0.75, 0.75}, 0},
  };

  const guint n = sizeof (points) / sizeof (Pt);

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

  vsg_prtree2d_near_far_traversal (tree, (VsgPRTree2dInteractionFunc) far,
                                   (VsgPRTree2dInteractionFunc) near,
                                   &ret);

  for (i=0; i<n; i++)
    {
      if (points[i].count != n)
        {
          g_printerr ("ERROR: wrong count on point %d: %d (should be %d).\n",
                      i, points[i].count, n);
          ret ++;
        }
      vsg_prtree2d_remove_point (tree, &points[i]);
    }

  vsg_prtree2d_free (tree);

  return ret;
}
