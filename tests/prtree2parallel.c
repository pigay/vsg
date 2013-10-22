/* basic point insertion, removal and traversal on a cloned VsgPRTree2d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include "glib/gprintf.h"

#include <math.h>
#include <glib.h>

static gboolean _do_write = FALSE;

static gint rk, sz;

static GPtrArray *points = NULL;

static gpointer pt_alloc (gboolean resident, gpointer user_data)
{
  gpointer ret;
  ret = g_malloc (sizeof (VsgVector2d));
  g_ptr_array_add (points, ret);
/*   g_printerr ("%d: alloc 1 VsgVector2d (%p)\n", rk, ret); */
  return ret;
}

static void pt_destroy (gpointer data, gboolean resident,
                        gpointer user_data)
{
/*   g_printerr ("%d: destroy 1 VsgVector2d (%p)\n", rk, data); */
  g_ptr_array_remove (points, data);
  g_free (data);
}

static void pt_migrate_pack (VsgVector2d *pt, VsgPackedMsg *pm,
                             gpointer user_data)
{
/*   g_printerr ("%d: pack ", rk); */
/*   vsg_vector2d_write (pt, stderr); */
/*   g_printerr ("\n"); */
  vsg_packed_msg_send_append (pm, pt, 1, VSG_MPI_TYPE_VECTOR2D);
}

static void pt_migrate_unpack (VsgVector2d *pt, VsgPackedMsg *pm,
                               gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, pt, 1, VSG_MPI_TYPE_VECTOR2D);

/*   g_printerr ("%d: unpack ", rk); */
/*   vsg_vector2d_write (pt, stderr); */
/*   g_printerr ("\n"); */
}

static void empty_array (gpointer var, gpointer data)
{
/*   g_printerr ("%d: static destroy 1 VsgVector2d (%p)\n", rk, var); */
  g_free (var);
}

static GPtrArray *regions = NULL;

typedef struct _Circle Circle;

struct _Circle {
  VsgVector2d center;
  gdouble radius;
};

static vsgrloc2 _circle_loc2 (Circle *circle, VsgVector2d *center)
{
  VsgVector2d tmp;
  gdouble dist;
  vsgloc2 centerpos;
  vsgrloc2 ret;

  vsg_vector2d_sub (center, &circle->center, &tmp);

  dist = vsg_vector2d_norm (&tmp);

  if (dist <= circle->radius) return VSG_RLOC2_MASK;

  ret = 0;

  centerpos = vsg_vector2d_vector2d_locfunc (&circle->center, center);

/*   g_printerr ("%d: {c=", rk); */
/*   vsg_vector2d_write (&circle->center, stderr); */
/*   g_printerr (" r=%g} ref=", circle->radius); */
/*   vsg_vector2d_write (center, stderr); */

  ret |= VSG_RLOC2_COMP (centerpos);

/*   g_printerr (" / 0x%X", ret); */

  if (fabs (tmp.x) <= circle->radius)
    {
      vsgloc2 itmp = centerpos ^ VSG_LOC2_X;
      ret |= VSG_RLOC2_COMP (itmp);
/*       g_printerr (" / x<rad 0x%X (%x %x)", itmp, centerpos, VSG_LOC2_X); */
    }

  if (fabs (tmp.y) <= circle->radius)
    {
      vsgloc2 itmp = centerpos ^ VSG_LOC2_Y;
      ret |= VSG_RLOC2_COMP (itmp);
/*       g_printerr (" / y<rad 0x%X (%x %x)", itmp, centerpos, VSG_LOC2_Y); */
    }

/*   g_printerr (" => 0x%X\n", ret); */

  return ret;
}

static gpointer rg_alloc (gboolean resident, gpointer user_data)
{
  gpointer ret;
  ret = g_malloc (sizeof (Circle));
  g_ptr_array_add (regions, ret);

/*   g_printerr ("%d: alloc 1 Circle (%p)\n", rk, ret); */

  return ret;
}

static void rg_destroy (gpointer data, gboolean resident,
                        gpointer user_data)
{
/*   g_printerr ("%d: destroy 1 Circle (%p)\n", rk, data); */

  g_ptr_array_remove (regions, data);
  g_free (data);
}

static void rg_migrate_pack (Circle *rg, VsgPackedMsg *pm,
                             gpointer user_data)
{
/*   g_printerr ("%d: pack circle {c=", rk); */
/*   vsg_vector2d_write (&rg->center, stderr); */
/*   g_printerr (" r=%lf}\n", rg->radius); */

  vsg_packed_msg_send_append (pm, &rg->center, 1, VSG_MPI_TYPE_VECTOR2D);
  vsg_packed_msg_send_append (pm, &rg->radius, 1, MPI_DOUBLE);
}

static void rg_migrate_unpack (Circle *rg, VsgPackedMsg *pm,
                               gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &rg->center, 1, VSG_MPI_TYPE_VECTOR2D);
  vsg_packed_msg_recv_read (pm, &rg->radius, 1, MPI_DOUBLE);

/*   g_printerr ("%d: unpack circle {c=", rk); */
/*   vsg_vector2d_write (&rg->center, stderr); */
/*   g_printerr (" r=%lf}\n", rg->radius); */
}

static VsgPRTreeParallelConfig pconfig = {
  /* Point VTable */
  {pt_alloc, NULL,
   pt_destroy, NULL,
   {(VsgMigrablePackDataFunc) pt_migrate_pack, NULL, (VsgMigrablePackDataFunc) pt_migrate_unpack, NULL},
  },
  /* Region VTable */
  {rg_alloc, NULL,
   rg_destroy, NULL,
   {(VsgMigrablePackDataFunc) rg_migrate_pack, NULL,
    (VsgMigrablePackDataFunc) rg_migrate_unpack, NULL,
   },
  },
  /* NodeData VTable */
  {NULL, NULL, NULL, NULL, {NULL, NULL, NULL, NULL},
  },
  /*communicator */
  MPI_COMM_WORLD,
};


static void _pt_write (VsgVector2d *pt, FILE *file)
{
  fprintf (file, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\" " \
           "style=\"stroke:#000000;fill:#00ff00;\"/>\n",
           pt->x, -pt->y, 0.02);
}

static void _rg_write (Circle *c, FILE *file)
{
  fprintf (file, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\" " \
           "style=\"stroke:#000000;fill:none;\"/>\n",
           c->center.x, -c->center.y, c->radius);

}

static void _traverse_bg_write (VsgPRTree2dNodeInfo *node_info, FILE *file)
{
  gdouble x = node_info->lbound.x;
  gdouble y = -node_info->ubound.y;
  gdouble w = node_info->ubound.x - x;
  gdouble h = -node_info->lbound.y - y;
  gchar *fill = "none";

  if (!node_info->isleaf) return;

  // *** PRIVATE-REMOTE
  if (VSG_PRTREE2D_NODE_INFO_IS_PRIVATE_REMOTE (node_info))
    fill = "#ff0000";

  fprintf (file, "<rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" " \
           "rx=\"0\" style=\"stroke:#000000; " \
           "stroke-linejoin:miter; stroke-linecap:butt; fill:%s;\"/>\n",
           x, y, w, h, fill);
}

static void _traverse_fg_write (VsgPRTree2dNodeInfo *node_info, FILE *file)
{
  g_slist_foreach (node_info->region_list, (GFunc) _rg_write, file);
  g_slist_foreach (node_info->point_list, (GFunc) _pt_write, file);
}

static void _tree_write (VsgPRTree2d *tree)
{
  gchar fn[1024];
  FILE *f;
  VsgVector2d lb, ub;
  gdouble x;
  gdouble y;
  gdouble w;
  gdouble h;

  vsg_prtree2d_get_bounds (tree, &lb, &ub);

  x = lb.x;
  y = -ub.y;
  w = ub.x - x;
  h = -lb.y - y;

  g_sprintf (fn, "prtree2parallel-%03d.svg", rk);
  f = fopen (fn, "w");

  fprintf (f, "<?xml version=\"1.0\" standalone=\"no\"?>\n" \
           "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n" \
           "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n" \
           "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"10cm\" " \
           "height=\"10cm\" viewBox=\"%g %g %g %g\">\n" \
           "<g style=\"stroke-width:0.01; stroke:black; fill:none\">\n",
           x, y, w, h);

  vsg_prtree2d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree2dFunc) _traverse_bg_write,
                         f);
  vsg_prtree2d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree2dFunc) _traverse_fg_write,
                         f);

  fprintf (f, "</g>\n</svg>\n");

  fclose (f);

}

static gint reference_total_points = 0;

static void init_total_points_count ()
{
  gint local_points = points->len;

  MPI_Allreduce (&local_points, &reference_total_points, 1, MPI_INT, MPI_SUM,
                 MPI_COMM_WORLD);
}

static void _local_points_count (VsgPRTree2dNodeInfo *node_info, gint *cnt)
{
  *cnt += g_slist_length (node_info->point_list);
}

static gint check_points_number (VsgPRTree2d *tree)
{
  gint ret = 0;
  gint local_points = 0;
  gint total_points = 0;

  vsg_prtree2d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree2dFunc) _local_points_count, &local_points);

  MPI_Allreduce (&local_points, &total_points, 1, MPI_INT, MPI_SUM,
                 MPI_COMM_WORLD);

  if (total_points != reference_total_points)
    {
      g_printerr ("%d: total points mismatch : ref=%d verif=%d\n",
                  rk, reference_total_points, total_points);

      ret ++;
    }

  return ret;
}

static gint reference_total_regions = 0;

static void init_total_regions_count ()
{
  gint local_regions = regions->len;

  MPI_Allreduce (&local_regions, &reference_total_regions, 1, MPI_INT, MPI_SUM,
                 MPI_COMM_WORLD);
}

static void _local_regions_count (VsgPRTree2dNodeInfo *node_info, gint *cnt)
{
  // *** PRIVATE-LOCAL
  if (VSG_PRTREE2D_NODE_INFO_IS_PRIVATE_LOCAL (node_info) || rk == 0)
    *cnt += g_slist_length (node_info->region_list);
}

static gint check_regions_number (VsgPRTree2d *tree)
{
  gint ret = 0;
  gint local_regions = 0;
  gint total_regions = 0;

  vsg_prtree2d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree2dFunc) _local_regions_count,
                         &local_regions);

  MPI_Allreduce (&local_regions, &total_regions, 1, MPI_INT, MPI_SUM,
                 MPI_COMM_WORLD);

  if (total_regions != reference_total_regions)
    {
      g_printerr ("%d: total regions mismatch : ref=%d verif=%d\n",
                  rk, reference_total_regions, total_regions);

      ret ++;
    }

  return ret;
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  VsgPRTree2d *tree;
  gint i;

  VsgVector2d lb;
  VsgVector2d ub;


  MPI_Init (&argc, &argv);

  MPI_Comm_size (MPI_COMM_WORLD, &sz);
  MPI_Comm_rank (MPI_COMM_WORLD, &rk);

  if (argc > 1 && g_ascii_strncasecmp (argv[1], "--version", 9) == 0)
    {
      if (rk == 0)
        g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  if (argc > 1 && g_ascii_strncasecmp (argv[1], "--write", 7) == 0)
    {
      _do_write = TRUE;
    }

  vsg_init_gdouble ();

  points = g_ptr_array_new ();
  regions = g_ptr_array_new ();

  if (rk == 0)
    {
      VsgVector2d *pt;
      Circle *c;

      lb.x = -1.; lb.y = -1.;
      ub.x = 0.; ub.y = 0.;

      pt = pt_alloc (TRUE, NULL);
      pt->x = -0.5; pt->y = -0.5;

      c = rg_alloc (TRUE, NULL);
      c->center.x = -0.6; c->center.y = -0.6;
      c->radius = 0.1;
    }
  else
    {
      VsgVector2d *pt;
      Circle *c;

      lb.x = 0.; lb.y = 0.;
      ub.x = 1.*rk; ub.y = 1.*rk;

      pt = pt_alloc (TRUE, NULL);
      pt->x = 0.5*rk; pt->y = 0.5*rk;

      pt = pt_alloc (TRUE, NULL);
      pt->x = 0.60*rk; pt->y = 0.65*rk;

      pt = pt_alloc (TRUE, NULL);
      pt->x = 0.15*rk; pt->y = 0.75*rk;

      c = rg_alloc (TRUE, NULL);
      c->center.x = 0.6*rk; c->center.y = 0.6*rk;
      c->radius = 0.11;
    }

  /* create the tree */
  tree =
    vsg_prtree2d_new_full (&lb, &ub,
                           (VsgPoint2dLocFunc) vsg_vector2d_vector2d_locfunc,
                           (VsgPoint2dDistFunc) vsg_vector2d_dist,
                           (VsgRegion2dLocFunc) _circle_loc2, 2);

  /* insert the points */
  for (i=0; i<points->len; i++)
    {
      vsg_prtree2d_insert_point (tree, g_ptr_array_index (points, i));
    }

  /* insert the regions */
  for (i=0; i<regions->len; i++)
    {
      vsg_prtree2d_insert_region (tree, g_ptr_array_index (regions, i));
    }

  /* count total created points and regions */
  init_total_points_count ();
  init_total_regions_count ();

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: set_parallel begin\n", rk); */

  vsg_prtree2d_set_parallel (tree, &pconfig);

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: set_parallel ok\n", rk); */

  ret += check_points_number (tree);
  ret += check_regions_number (tree);

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: before migrate_flush ok\n", rk); */
  {
    VsgVector2d *pt;
    Circle *c;

    pt = pt_alloc (TRUE, NULL);
    pt->x = 0.5*rk; pt->y = 0.75*rk;
    vsg_prtree2d_insert_point (tree, pt);

    c = rg_alloc (TRUE, NULL);
    c->center.x = 1.; c->center.y = 0.6*rk;
    c->radius = 0.1;
    vsg_prtree2d_insert_region (tree, c);
  }

  /* update total points and regions count */
  init_total_points_count ();
  init_total_regions_count ();

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: migrate_flush begin\n", rk); */

  vsg_prtree2d_migrate_flush (tree);

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: migrate_flush ok\n", rk); */

  ret += check_points_number (tree);
  ret += check_regions_number (tree);

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: distribute_nodes begin\n", rk); */

  for (i=0; i<1; i++)
    {
      gint dst = (i+1) % sz;

/*       MPI_Barrier (MPI_COMM_WORLD); */
/*       g_printerr ("%d: move to %d\n", rk, dst); */

      vsg_prtree2d_distribute_concentrate (tree, dst);

      ret += check_points_number (tree);
      ret += check_regions_number (tree);
    }

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: split between nodes\n", rk); */

  vsg_prtree2d_distribute_scatter_leaves (tree);

/*   ret += check_points_number (tree); */
/*   ret += check_regions_number (tree); */

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: distribute_nodes ok\n", rk); */

  if (_do_write)
    {
      MPI_Barrier (MPI_COMM_WORLD);
      _tree_write (tree);
    }

  if (_do_write)
    {
      gchar fn[1024];
      FILE *f;

      g_sprintf (fn, "prtree2parallel-%03d.txt", rk);
      f = fopen (fn, "w");
      vsg_prtree2d_write (tree, f);
      fclose (f);
    }

  /* destroy the points */
  g_ptr_array_foreach (points, empty_array, NULL);
  g_ptr_array_free (points, TRUE);

  /* destroy the circles */
  g_ptr_array_foreach (regions, empty_array, NULL);
  g_ptr_array_free (regions, TRUE);

  /* destroy the tree */
  vsg_prtree2d_free (tree);

  MPI_Finalize ();

  return ret;
}
