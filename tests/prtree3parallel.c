/* basic point insertion, removal and traversal on a cloned VsgPRTree3d */

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
  ret = g_malloc (sizeof (VsgVector3d));
  g_ptr_array_add (points, ret);
/*   g_printerr ("%d: alloc 1 VsgVector3d (%p)\n", rk, ret); */
  return ret;
}

static void pt_destroy (gpointer data, gboolean resident,
                        gpointer user_data)
{
/*   g_printerr ("%d: destroy 1 VsgVector3d (%p)\n", rk, data); */
  g_ptr_array_remove (points, data);
  g_free (data);
}

static void pt_migrate_pack (VsgVector3d *pt, VsgPackedMsg *pm,
                             gpointer user_data)
{
/*   g_printerr ("%d: pack ", rk); */
/*   vsg_vector3d_write (pt, stderr); */
/*   g_printerr ("\n"); */
  vsg_packed_msg_send_append (pm, pt, 1, VSG_MPI_TYPE_VECTOR3D);
}

static void pt_migrate_unpack (VsgVector3d *pt, VsgPackedMsg *pm,
                               gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, pt, 1, VSG_MPI_TYPE_VECTOR3D);

/*   g_printerr ("%d: unpack ", rk); */
/*   vsg_vector3d_write (pt, stderr); */
/*   g_printerr ("\n"); */
}

static void empty_array (gpointer var, gpointer data)
{
/*   g_printerr ("%d: static destroy 1 VsgVector3d (%p)\n", rk, var); */
  g_free (var);
}

static GPtrArray *regions = NULL;

typedef struct _Sphere Sphere;

struct _Sphere {
  VsgVector3d center;
  gdouble radius;
};

static vsgrloc3 _sphere_loc3 (Sphere *sphere, VsgVector3d *center)
{
  VsgVector3d tmp;
  gdouble dist;
  vsgloc3 centerpos;
  vsgrloc3 ret;

  vsg_vector3d_sub (center, &sphere->center, &tmp);

  dist = vsg_vector3d_norm (&tmp);

  if (dist <= sphere->radius) return VSG_RLOC3_MASK;

  ret = 0;

  centerpos = vsg_vector3d_vector3d_locfunc (&sphere->center, center);

/*   g_printerr ("%d: {c=", rk); */
/*   vsg_vector3d_write (&sphere->center, stderr); */
/*   g_printerr (" r=%g} ref=", sphere->radius); */
/*   vsg_vector3d_write (center, stderr); */

  ret |= VSG_RLOC3_COMP (centerpos);

/*   g_printerr (" / 0x%X", ret); */

  if (fabs (tmp.x) <= sphere->radius)
    {
      vsgloc3 itmp = centerpos ^ VSG_LOC3_X;
      ret |= VSG_RLOC3_COMP (itmp);
/*       g_printerr (" / x<rad 0x%X (%x %x)", itmp, centerpos, VSG_LOC3_X); */
    }

  if (fabs (tmp.y) <= sphere->radius)
    {
      vsgloc3 itmp = centerpos ^ VSG_LOC3_Y;
      ret |= VSG_RLOC3_COMP (itmp);
/*       g_printerr (" / y<rad 0x%X (%x %x)", itmp, centerpos, VSG_LOC3_Y); */
    }

  if (fabs (tmp.z) <= sphere->radius)
    {
      vsgloc3 itmp = centerpos ^ VSG_LOC3_Z;
      ret |= VSG_RLOC3_COMP (itmp);
/*       g_printerr (" / y<rad 0x%X (%x %x)", itmp, centerpos, VSG_LOC3_Y); */
    }

/*   g_printerr (" => 0x%X\n", ret); */

  return ret;
}

static gpointer rg_alloc (gboolean resident, gpointer user_data)
{
  gpointer ret;
  ret = g_malloc (sizeof (Sphere));
  g_ptr_array_add (regions, ret);

/*   g_printerr ("%d: alloc 1 Sphere (%p)\n", rk, ret); */

  return ret;
}

static void rg_destroy (gpointer data, gboolean resident,
                        gpointer user_data)
{
/*   g_printerr ("%d: destroy 1 Sphere (%p)\n", rk, data); */

  g_ptr_array_remove (regions, data);
  g_free (data);
}

static void rg_migrate_pack (Sphere *rg, VsgPackedMsg *pm,
                             gpointer user_data)
{
/*   g_printerr ("%d: pack sphere {c=", rk); */
/*   vsg_vector3d_write (&rg->center, stderr); */
/*   g_printerr (" r=%lf}\n", rg->radius); */

  vsg_packed_msg_send_append (pm, &rg->center, 1, VSG_MPI_TYPE_VECTOR3D);
  vsg_packed_msg_send_append (pm, &rg->radius, 1, MPI_DOUBLE);
}

static void rg_migrate_unpack (Sphere *rg, VsgPackedMsg *pm,
                               gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &rg->center, 1, VSG_MPI_TYPE_VECTOR3D);
  vsg_packed_msg_recv_read (pm, &rg->radius, 1, MPI_DOUBLE);

/*   g_printerr ("%d: unpack sphere {c=", rk); */
/*   vsg_vector3d_write (&rg->center, stderr); */
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


static void _pt_write (VsgVector3d *pt, FILE *file)
{
  fprintf (file, "%g %g %g\n",
           pt->x, pt->y, pt->z);
}

static void _rg_write (Sphere *c, FILE *file)
{
/*   fprintf (file, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\" " \ */
/*            "style=\"stroke:#000000;fill:none;\"/>\n", */
/*            c->center.x, -c->center.y, c->radius); */

}

static void _traverse_bg_write (VsgPRTree3dNodeInfo *node_info, FILE *file)
{
  gdouble x = node_info->lbound.x;
  gdouble y = -node_info->ubound.y;
  gdouble w = node_info->ubound.x - x;
  gdouble h = -node_info->lbound.y - y;
  gchar *fill = "none";

  if (!node_info->isleaf) return;

  if (VSG_PRTREE3D_NODE_INFO_IS_REMOTE (node_info))
    fill = "#ff0000";

  fprintf (file, "<rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" " \
           "rx=\"0\" style=\"stroke:#000000; " \
           "stroke-linejoin:miter; stroke-linecap:butt; fill:%s;\"/>\n",
           x, y, w, h, fill);
}

static void _traverse_fg_write (VsgPRTree3dNodeInfo *node_info, FILE *file)
{
  g_slist_foreach (node_info->region_list, (GFunc) _rg_write, file);
  g_slist_foreach (node_info->point_list, (GFunc) _pt_write, file);
}

static void _tree_write (VsgPRTree3d *tree)
{
  gchar fn[1024];
  FILE *f;
  gint np = vsg_prtree3d_point_count (tree);
  gint i;

  g_sprintf (fn, "prtree3parallel-%03d.vtp", rk);
  f = fopen (fn, "w");

  fprintf (f, "<?xml version=\"1.0\" ?>\n" \
           "<VTKFile type=\"PolyData\" version=\"0.1\">\n" \
           "<PolyData>\n" \
           "<Piece NumberOfPoints=\"%d\" NumberOfVerts=\"%d\" " \
           "NumberOfLines=\"0\" NumberOfStrips=\"0\" " \
           "NumberOfPolys=\"0\">\n",
           np, np);

/*   vsg_prtree3d_traverse (tree, G_PRE_ORDER, */
/*                          (VsgPRTree3dFunc) _traverse_bg_write, */
/*                          f); */

  fprintf (f, "<Points>\n");
  fprintf (f, "<DataArray type=\"Float64\" NumberOfComponents=\"3\" " \
           "format=\"ascii\">\n");
  vsg_prtree3d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree3dFunc) _traverse_fg_write,
                         f);
  fprintf (f, "</DataArray>\n</Points>\n");

  fprintf (f, "<Verts>\n");
  fprintf (f, "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n");
  for (i=0; i<np; i++) fprintf (f, "%d ", i);
  fprintf (f, "\n</DataArray>\n");
  fprintf (f, "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\" >");
  for (i=0; i<np; i++) fprintf (f, "%d ", i+1);
  fprintf (f, "\n</DataArray>\n</Verts>\n");
  fprintf (f, "\n</Piece>\n</PolyData>\n</VTKFile>\n");

  fclose (f);

}

static gint reference_total_points = 0;

static void init_total_points_count ()
{
  gint local_points = points->len;

  MPI_Allreduce (&local_points, &reference_total_points, 1, MPI_INT, MPI_SUM,
                 MPI_COMM_WORLD);
}

static void _local_points_count (VsgPRTree3dNodeInfo *node_info, gint *cnt)
{
  *cnt += g_slist_length (node_info->point_list);
}

static gint check_points_number (VsgPRTree3d *tree)
{
  gint ret = 0;
  gint local_points = 0;
  gint total_points = 0;

  vsg_prtree3d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree3dFunc) _local_points_count, &local_points);

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

static void _local_regions_count (VsgPRTree3dNodeInfo *node_info, gint *cnt)
{
  if (VSG_PRTREE3D_NODE_INFO_IS_LOCAL (node_info) || rk == 0)
    *cnt += g_slist_length (node_info->region_list);
}

static gint check_regions_number (VsgPRTree3d *tree)
{
  gint ret = 0;
  gint local_regions = 0;
  gint total_regions = 0;

  vsg_prtree3d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree3dFunc) _local_regions_count,
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

  VsgPRTree3d *tree;
  gint i;

  VsgVector3d lb;
  VsgVector3d ub;


  MPI_Init (&argc, &argv);

  MPI_Comm_size (MPI_COMM_WORLD, &sz);
  MPI_Comm_rank (MPI_COMM_WORLD, &rk);

  if (argc > 1 && g_strncasecmp (argv[1], "--version", 9) == 0)
    {
      if (rk == 0)
        g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  if (argc > 1 && g_strncasecmp (argv[1], "--write", 7) == 0)
    {
      _do_write = TRUE;
    }

  vsg_init_gdouble ();

  points = g_ptr_array_new ();
  regions = g_ptr_array_new ();

  if (rk == 0)
    {
      VsgVector3d *pt;
      Sphere *c;

      lb.x = -1.; lb.y = -1.; lb.z = -1.;
      ub.x = 0.; ub.y = 0.; ub.z = 0.;

      pt = pt_alloc (TRUE, NULL);
      pt->x = -0.5; pt->y = -0.5; pt->z = -0.5;

      c = rg_alloc (TRUE, NULL);
      c->center.x = -0.6; c->center.y = -0.6; c->center.z = -0.6;
      c->radius = 0.1;
    }
  else
    {
      VsgVector3d *pt;
      Sphere *c;

      lb.x = 0.; lb.y = 0.; lb.z = 0.;
      ub.x = 1.*rk; ub.y = 1.*rk; ub.z = 1.*rk;

      pt = pt_alloc (TRUE, NULL);
      pt->x = 0.5*rk; pt->y = 0.5*rk; pt->z = 0.5*rk;

      pt = pt_alloc (TRUE, NULL);
      pt->x = 0.60*rk; pt->y = 0.65*rk; pt->z = 0.70*rk;

      pt = pt_alloc (TRUE, NULL);
      pt->x = 0.15*rk; pt->y = 0.75*rk; pt->z = 0.80*rk;

      c = rg_alloc (TRUE, NULL);
      c->center.x = 0.6*rk; c->center.y = 0.6*rk; c->center.z = 0.6*rk;
      c->radius = 0.11;
    }

  /* create the tree */
  tree =
    vsg_prtree3d_new_full (&lb, &ub,
                           (VsgPoint3dLocFunc) vsg_vector3d_vector3d_locfunc,
                           (VsgPoint3dDistFunc) vsg_vector3d_dist,
                           (VsgRegion3dLocFunc) _sphere_loc3, 2);

  /* insert the points */
  for (i=0; i<points->len; i++)
    {
      vsg_prtree3d_insert_point (tree, g_ptr_array_index (points, i));
    }

  /* insert the regions */
  for (i=0; i<regions->len; i++)
    {
      vsg_prtree3d_insert_region (tree, g_ptr_array_index (regions, i));
    }

  /* count total created points and regions */
  init_total_points_count ();
  init_total_regions_count ();

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: set_parallel begin\n", rk); */

  vsg_prtree3d_set_parallel (tree, &pconfig);

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: set_parallel ok\n", rk); */

  ret += check_points_number (tree);
  ret += check_regions_number (tree);

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: before migrate_flush ok\n", rk); */
  {
    VsgVector3d *pt;
    Sphere *c;

    pt = pt_alloc (TRUE, NULL);
    pt->x = 0.5*rk; pt->y = 0.75*rk; pt->z = 0.75*rk;
    vsg_prtree3d_insert_point (tree, pt);

    c = rg_alloc (TRUE, NULL);
    c->center.x = 1.; c->center.y = 0.6*rk; c->center.z = 0.6*rk;
    c->radius = 0.1;
    vsg_prtree3d_insert_region (tree, c);
  }

  /* update total points and regions count */
  init_total_points_count ();
  init_total_regions_count ();

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: migrate_flush begin\n", rk); */

  vsg_prtree3d_migrate_flush (tree);

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: migrate_flush ok\n", rk); */

  ret += check_points_number (tree);
  ret += check_regions_number (tree);

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: distribute_nodes begin\n", rk); */

  for (i=0; i<sz; i++)
    {
      gint dst = (i+1) % sz;

/*       MPI_Barrier (MPI_COMM_WORLD); */
/*       g_printerr ("%d: move to %d\n", rk, dst); */

      vsg_prtree3d_distribute_concentrate (tree, dst);

      ret += check_points_number (tree);
      ret += check_regions_number (tree);
    }

/*   MPI_Barrier (MPI_COMM_WORLD); */
/*   g_printerr ("%d: split between nodes\n", rk); */

  vsg_prtree3d_distribute_scatter_leaves (tree);

  ret += check_points_number (tree);
  ret += check_regions_number (tree);

/* /\*   MPI_Barrier (MPI_COMM_WORLD); *\/ */
/* /\*   g_printerr ("%d: distribute_nodes ok\n", rk); *\/ */

  if (_do_write)
    {
      MPI_Barrier (MPI_COMM_WORLD);
      _tree_write (tree);
    }

  if (_do_write)
    {
      gchar fn[1024];
      FILE *f;

      g_sprintf (fn, "prtree3parallel-%03d.txt", rk);
      f = fopen (fn, "w");
      vsg_prtree3d_write (tree, f);
      fclose (f);
    }

  /* destroy the points */
  g_ptr_array_foreach (points, empty_array, NULL);
  g_ptr_array_free (points, TRUE);

  /* destroy the spheres */
  g_ptr_array_foreach (regions, empty_array, NULL);
  g_ptr_array_free (regions, TRUE);

  /* destroy the tree */
  vsg_prtree3d_free (tree);

  MPI_Finalize ();

  return ret;
}
