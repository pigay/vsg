/* basic point insertion, removal and traversal on VsgPRTree2d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

#include <stdlib.h>
#include <string.h>

#include <glib/gprintf.h>

static gint rk = 0, sz = 1;

static gboolean _verbose = FALSE;
static gboolean _write = FALSE;
static gboolean _do_check = TRUE;
static gboolean _mpi = FALSE;

static void grid_fill (guint *npoints, GPtrArray *array);

static guint _maxbox = 10;
static guint _virtual_maxbox = 10;

static guint _npoints = 4;

static GPtrArray *points_array = NULL;
static GPtrArray *pointsref_array = NULL;

static glong _near_count = 0;
static glong _far_count = 0;
static glong _expect_far_count = -1;

static void (*_fill) (guint *npoints, GPtrArray *array) = grid_fill;

typedef struct _Pt Pt;
typedef struct _NodeCounter NodeCounter;

struct _Pt
{
  VsgVector2d vector;
  gint weight;
  glong count;
};

struct _NodeCounter
{
  glong in_count;
  glong out_count;
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

static NodeCounter _nc = {0, 0};

gpointer node_counter_alloc (gboolean resident, gpointer user_data)
{
  gpointer ret;

  ret = g_boxed_copy (TYPE_NODE_COUNTER, &_nc);

  return ret;
}

void node_counter_destroy (gpointer data, gboolean resident,
                           gpointer user_data)
{
  g_assert (data != NULL);
  g_boxed_free (TYPE_NODE_COUNTER, data);
}

#ifdef VSG_HAVE_MPI
/* migration pack/unpack functions */
void nc_migrate_pack (gpointer nc, VsgPackedMsg *pm,
                      gpointer user_data)
{
  vsg_packed_msg_send_append (pm, nc, 2, MPI_LONG);
}

void nc_migrate_unpack (gpointer nc, VsgPackedMsg *pm,
                        gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, nc, 2, MPI_LONG);
}

/* visit forward pack/unpack functions */
/* only the coordinates part is transmitted */
void nc_visit_fw_pack (gpointer nc, VsgPackedMsg *pm,
                       gpointer user_data)
{
  vsg_packed_msg_send_append (pm, &((NodeCounter *) nc)->in_count, 1, MPI_LONG);
}

void nc_visit_fw_unpack (gpointer nc, VsgPackedMsg *pm,
                         gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &((NodeCounter *) nc)->in_count, 1, MPI_LONG);

}

void nc_visit_fw_reduce (gpointer a, gpointer b, gpointer user_data)
{
  ((NodeCounter *) b)->in_count +=
    ((NodeCounter *) a)->in_count;
}

/* visit forward pack/unpack functions */
/* only the count part is transmitted */
void nc_visit_bw_pack (gpointer nc, VsgPackedMsg *pm,
                       gpointer user_data)

{
/*   g_printerr ("%d : pack out %d\n", rk, nc->out_count); */
  vsg_packed_msg_send_append (pm, &((NodeCounter *) nc)->out_count, 1,
                              MPI_LONG);
}

void nc_visit_bw_unpack (gpointer nc, VsgPackedMsg *pm,
                         gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &((NodeCounter *) nc)->out_count, 1, MPI_LONG);

}

void nc_visit_bw_reduce (gpointer a, gpointer b, gpointer user_data)
{
  ((NodeCounter *) b)->out_count += ((NodeCounter *) a)->out_count;
}

#endif

void test_printerr (const gchar *fmt, ...)
{
  va_list ap;

  if (sz > 1)
    g_fprintf (stderr, "%d: ", rk);

  va_start (ap, fmt);
  g_vfprintf (stderr, fmt, ap);
  va_end (ap);
}

gpointer pt_alloc (gboolean resident, gpointer array)
{
  Pt *ret;
  ret = g_malloc (sizeof (Pt));

  if (resident)
    g_ptr_array_add ((GPtrArray *) array, ret);

  return ret;
}

void pt_destroy (gpointer data, gboolean resident, gpointer array)
{
  if (resident)
    g_ptr_array_remove ((GPtrArray *) array, data);

  g_free (data);
}

#ifdef VSG_HAVE_MPI
/* migration pack/unpack functions */
void pt_migrate_pack (gpointer pt, VsgPackedMsg *pm,
                      gpointer user_data)
{
  vsg_packed_msg_send_append (pm, &((Pt *) pt)->vector, 1,
                              VSG_MPI_TYPE_VECTOR2D);
  vsg_packed_msg_send_append (pm, &((Pt *) pt)->weight, 1, MPI_INT);
  vsg_packed_msg_send_append (pm, &((Pt *) pt)->count, 1, MPI_LONG);
}

void pt_migrate_unpack (gpointer pt, VsgPackedMsg *pm,
                        gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &((Pt *) pt)->vector, 1, VSG_MPI_TYPE_VECTOR2D);
  vsg_packed_msg_recv_read (pm, &((Pt *) pt)->weight, 1, MPI_INT);
  vsg_packed_msg_recv_read (pm, &((Pt *) pt)->count, 1, MPI_LONG);
}

/* visit forward pack/unpack functions */
/* only the coordinates part is transmitted */
void pt_visit_fw_pack (gpointer pt, VsgPackedMsg *pm,
                       gpointer user_data)
{
  vsg_packed_msg_send_append (pm, &((Pt *) pt)->vector, 1,
                              VSG_MPI_TYPE_VECTOR2D);
  vsg_packed_msg_send_append (pm, &((Pt *) pt)->weight, 1, MPI_INT);
}

void pt_visit_fw_unpack (gpointer pt, VsgPackedMsg *pm,
                         gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &((Pt *) pt)->vector, 1, VSG_MPI_TYPE_VECTOR2D);
  vsg_packed_msg_recv_read (pm, &((Pt *) pt)->weight, 1, MPI_INT);
  ((Pt *) pt)->count = 0;
}

/* visit forward pack/unpack functions */
/* only the count part is transmitted */
void pt_visit_bw_pack (gpointer pt, VsgPackedMsg *pm,
                       gpointer user_data)
{
  vsg_packed_msg_send_append (pm, &((Pt *) pt)->count, 1, MPI_LONG);
}

void pt_visit_bw_unpack (gpointer pt, VsgPackedMsg *pm,
                         gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &((Pt *) pt)->count, 1, MPI_LONG);
}

void pt_visit_bw_reduce (gpointer a, gpointer b, gpointer user_data)
{
  ((Pt *) b)->count += ((Pt *) a)->count;
}
#endif

static void grid_fill (guint *npoints, GPtrArray *array)
{
  gint i, j, n;

  n = ceil (sqrt (*npoints));

  for (i=0; i<n; i++)
    {
      gdouble x = (2.*i)/(n) - 1.;

      for (j=0; j<n; j++)
        {
          Pt *point = pt_alloc (TRUE, array);
          gdouble y = (2.*j)/(n) - 1.;


          point->vector.x = x;
          point->vector.y = y;

          point->weight = 1;
          /* vsg_vector2d_write (point, stderr); */
          /* g_printerr ("\n"); */
        }
    }

  *npoints = n*n;
}


static VsgPRTreeParallelConfig pconfig;
static VsgPRTreeParallelConfig prefconfig;

void pt_get_weight (Pt *pt, glong *count)
{
  *count += pt->weight;
}

void pt_add_count (Pt *pt, glong *count)
{
  pt->count += *count;
}

void _near (VsgPRTree2dNodeInfo *one_info,
            VsgPRTree2dNodeInfo *other_info,
            gint *err)
{
  glong one_count = 0, other_count = 0;

  if (!(one_info->isleaf && other_info->isleaf))
    {
      g_printerr ("ERROR: call for near_func on non leaf node [");
      vsg_prtree_key2d_write (&one_info->id, stderr);
      g_printerr ("]=%d / [", one_info->isleaf);
      vsg_prtree_key2d_write (&other_info->id, stderr);
      g_printerr ("]=%d\n", other_info->isleaf);
      (*err) ++;
    }

  g_slist_foreach (other_info->point_list, (GFunc) pt_get_weight,
                   &other_count);

  g_slist_foreach (one_info->point_list, (GFunc) pt_add_count,
                   &other_count);

  if (one_info != other_info)
    {
      g_slist_foreach (one_info->point_list, (GFunc) pt_get_weight,
                       &one_count);

      g_slist_foreach (other_info->point_list, (GFunc) pt_add_count,
                       &one_count);
    }

  if (one_info->point_count == 0 || other_info->point_count == 0)
    {
      g_printerr ("%d : unnecessary near call point_counts = %d/%d\n", rk,
                  one_info->point_count, other_info->point_count);
      (*err) ++;
    }

  _near_count ++;
}

void _far (VsgPRTree2dNodeInfo *one_info,
           VsgPRTree2dNodeInfo *other_info,
           gint *err)
{
  ((NodeCounter *) one_info->user_data)->out_count +=
    ((NodeCounter *) other_info->user_data)->in_count;

  ((NodeCounter *) other_info->user_data)->out_count +=
    ((NodeCounter *) one_info->user_data)->in_count;

  if ((one_info->point_count == 0 &&
       VSG_PRTREE2D_NODE_INFO_IS_LOCAL (one_info)) ||
      (other_info->point_count == 0 &&
       VSG_PRTREE2D_NODE_INFO_IS_LOCAL (other_info)))
    g_printerr ("%d : unnecessary far call\n", rk);

  _far_count ++;
}

void _up (VsgPRTree2dNodeInfo *node_info, gpointer data)
{
  if (! VSG_PRTREE2D_NODE_INFO_IS_REMOTE (node_info))
    {
      if (node_info->isleaf)
        {
          glong count = 0;

          g_slist_foreach (node_info->point_list, (GFunc) pt_get_weight,
                           &count);

          ((NodeCounter *) node_info->user_data)->in_count = count;
        }

      if (node_info->father_info)
        {
          ((NodeCounter *) node_info->father_info->user_data)->in_count +=
            ((NodeCounter *) node_info->user_data)->in_count;
        }
    }
}

void _zero_pt (Pt *pt, gpointer data)
{
  pt->count = 0;
}

gboolean _nf_isleaf_virtual_maxbox (const VsgPRTree2dNodeInfo *node_info,
                                    gpointer virtual_maxbox);

void _zero (VsgPRTree2dNodeInfo *node_info, gpointer data)
{
  if (! VSG_PRTREE2D_NODE_INFO_IS_REMOTE (node_info))
    {
      ((NodeCounter *) node_info->user_data)->in_count = 0;
      ((NodeCounter *) node_info->user_data)->out_count = 0;

      if (node_info->isleaf)
        g_slist_foreach (node_info->point_list, (GFunc) _zero_pt, NULL);
    }
}

void _do_upward_pass (VsgPRTree2d *tree)
{
  /* zero counts  */
  vsg_prtree2d_traverse (tree, G_POST_ORDER, (VsgPRTree2dFunc) _zero, NULL);

  /* accumulate from leaves to top */
  vsg_prtree2d_traverse (tree, G_POST_ORDER, (VsgPRTree2dFunc) _up, NULL);

#ifdef VSG_HAVE_MPI
  /* gather shared in_counts */
  vsg_prtree2d_shared_nodes_allreduce (tree, &pconfig.node_data.visit_forward);
#endif
}

void _down (VsgPRTree2dNodeInfo *node_info, gpointer data)
{
  glong count;

  if (VSG_PRTREE2D_NODE_INFO_IS_REMOTE (node_info)) return;

  if (node_info->father_info)
    {
      ((NodeCounter *) node_info->user_data)->out_count +=
        ((NodeCounter *) node_info->father_info->user_data)->out_count;
    }

  count = ((NodeCounter *) node_info->user_data)->out_count;

  g_slist_foreach (node_info->point_list, (GFunc) pt_add_count,
                   &count);
}

void _check_pt_count (Pt *pt, const glong *ref)
{

  if (pt->count != *ref)
    {
      g_printerr ("%d : error ", rk);
      vsg_vector2d_write (&pt->vector, stderr);
      g_printerr (" weight=%d count=%ld (should be %ld)\n", pt->weight,
                  pt->count, *ref);
    }
}

gboolean _nf_isleaf_virtual_maxbox (const VsgPRTree2dNodeInfo *node_info,
                                    gpointer virtual_maxbox)
{
  /* shared nodes shouldn't be considered as virtual leaves in this case
   * because point_count is only a local count. For example, a shared node
   * without any local child would be considered as a virtual leaf whatever is
   * its global point_count */
  if (VSG_PRTREE2D_NODE_INFO_IS_SHARED (node_info)) return FALSE;

  return node_info->point_count <= * ((guint *) virtual_maxbox);
}

static gchar **counts = NULL;

static
void parse_args (int argc, char **argv)
{
  int iarg = 1;
  char *arg;

  while (iarg < argc)
    {
      arg = argv[iarg];

      if (g_ascii_strncasecmp (arg, "--maxbox", 8) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            _maxbox = tmp;
	  else
	    test_printerr ("Invalid maxbox (--maxbox %s)\n", arg);
	}
      else if (g_ascii_strncasecmp (arg, "--virtual-maxbox", 16) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            _virtual_maxbox = tmp;
	  else
	    test_printerr ("Invalid virtual maxbox (--virtual-maxbox %s)\n",
                           arg);
	}
      else if (g_ascii_strncasecmp (arg, "--expect-far-counts", 19) == 0)
	{
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

          counts = g_strsplit (arg, ",", 100);

	}
      else if (g_ascii_strncasecmp (arg, "--expect-far-count", 18) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            _expect_far_count = tmp;
	  else
	    test_printerr ("Invalid expected far count (--expect-far-count %s)\n",
                           arg);
	}
      else if (g_ascii_strncasecmp (arg, "--fill", 6) == 0)
	{
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (g_ascii_strncasecmp (arg, "grid", 4) == 0)
            {
              _fill = grid_fill;
            }
          else 
            {
              test_printerr ("Invalid fill function name \"%s\"\n", arg);
            }
	}
      else if (g_strncasecmp (arg, "--np", 4) == 0)
        {
	  iarg ++;
	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  _npoints = atoi (arg);
        }
      else if (g_strncasecmp (arg, "--no-check", 9) == 0)
        {
          _do_check = FALSE;
        }
      else if (g_strncasecmp (arg, "--write", 7) == 0)
        {
          _write = TRUE;
        }
      else if (g_strncasecmp (arg, "-v", 2) == 0 ||
               g_strncasecmp (arg, "--verbose", 9) == 0)
        {
          _verbose = TRUE;
        }
      else if (g_ascii_strcasecmp (arg, "--version") == 0)
	{
	  g_printerr ("%s version %s\n", argv[0], PACKAGE_VERSION);
	  exit (0);
	}
      else if (g_strncasecmp (arg, "--mpi", 5) == 0)
        {
          _mpi = TRUE;
        }
      else
	{
	  test_printerr ("Invalid argument \"%s\"\n", arg);
	}

      iarg ++;
    }
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  VsgVector2d lb = {-1., -1.};
  VsgVector2d ub = {1., 1.};

  VsgPRTree2d *tree;
  VsgPRTree2d *treeref;
  gint i;
  gint ref_far_count;

  vsg_init_gdouble ();

  parse_args (argc, argv);

#ifdef VSG_HAVE_MPI
  if (_mpi)
    {
      MPI_Init (&argc, &argv);

      MPI_Comm_size (MPI_COMM_WORLD, &sz);
      MPI_Comm_rank (MPI_COMM_WORLD, &rk);

      if (counts != NULL)
        {
          gint i = 0;
          gint tmp = 0;

          while (counts[i+1] != NULL && i<rk) i ++;

	  if (sscanf (counts[i], "%u", &tmp) == 1)
            _expect_far_count = tmp;
	  else
	    test_printerr ("Invalid expected far count list (--expect-far-counts)\n");

          g_strfreev (counts);
        }
    }
#endif

  points_array = g_ptr_array_new ();

  if (rk == 0) _fill (&_npoints, points_array);
  else _npoints = 0;

  pointsref_array = g_ptr_array_sized_new (_npoints);
  for (i=0; i<_npoints; i++)
    {
      Pt *ptref = pt_alloc (TRUE, pointsref_array);
      memcpy (ptref, (Pt *) g_ptr_array_index (points_array, i), sizeof (Pt));
    }

  /* create the trees */
  tree =
    vsg_prtree2d_new_full (&lb, &ub,
                           (VsgPoint2dLocFunc) vsg_vector2d_vector2d_locfunc,
                           (VsgPoint2dDistFunc) vsg_vector2d_dist,
                           NULL, _maxbox);
  treeref = vsg_prtree2d_clone (tree);

  vsg_prtree2d_set_nf_isleaf (tree, _nf_isleaf_virtual_maxbox,
                              &_virtual_maxbox);

  vsg_parallel_vtable_set (&pconfig.node_data,
                           node_counter_alloc, NULL,
                           node_counter_destroy, NULL);
  vsg_prtree2d_set_node_data_vtable (tree, &pconfig.node_data);

  vsg_parallel_vtable_set (&prefconfig.node_data,
                           node_counter_alloc, NULL,
                           node_counter_destroy, NULL);
  vsg_prtree2d_set_node_data_vtable (treeref, &prefconfig.node_data);

#ifdef VSG_HAVE_MPI

  if (_mpi)
    {
      vsg_parallel_vtable_set (&pconfig.point,
                               pt_alloc, points_array,
                               pt_destroy, points_array);

      vsg_parallel_vtable_set (&prefconfig.point,
                               pt_alloc, pointsref_array,
                               pt_destroy, pointsref_array);

      vsg_parallel_vtable_set_parallel (&pconfig.point,
                                        pt_migrate_pack, NULL,
                                        pt_migrate_unpack, NULL,
                                        NULL, NULL,
                                        pt_visit_fw_pack, NULL,
                                        pt_visit_fw_unpack, NULL,
                                        NULL, NULL,
                                        pt_visit_bw_pack, NULL,
                                        pt_visit_bw_unpack, NULL,
                                        pt_visit_bw_reduce, NULL);

      vsg_parallel_vtable_set_parallel (&pconfig.node_data,
                                        nc_migrate_pack, NULL,
                                        nc_migrate_unpack, NULL,
                                        NULL, NULL,
                                        nc_visit_fw_pack, NULL,
                                        nc_visit_fw_unpack, NULL,
                                        nc_visit_fw_reduce, NULL,
                                        nc_visit_bw_pack, NULL,
                                        nc_visit_bw_unpack, NULL,
                                        nc_visit_bw_reduce, NULL);

      vsg_parallel_vtable_set_parallel (&prefconfig.point,
                                        pt_migrate_pack, NULL,
                                        pt_migrate_unpack, NULL,
                                        NULL, NULL,
                                        pt_visit_fw_pack, NULL,
                                        pt_visit_fw_unpack, NULL,
                                        NULL, NULL,
                                        pt_visit_bw_pack, NULL,
                                        pt_visit_bw_unpack, NULL,
                                        pt_visit_bw_reduce, NULL);

      vsg_parallel_vtable_set_parallel (&prefconfig.node_data,
                                        nc_migrate_pack, NULL,
                                        nc_migrate_unpack, NULL,
                                        NULL, NULL,
                                        nc_visit_fw_pack, NULL,
                                        nc_visit_fw_unpack, NULL,
                                        nc_visit_fw_reduce, NULL,
                                        nc_visit_bw_pack, NULL,
                                        nc_visit_bw_unpack, NULL,
                                        nc_visit_bw_reduce, NULL);

      vsg_prtree_parallel_config_set_communicator (&pconfig, MPI_COMM_WORLD);
      vsg_prtree_parallel_config_set_communicator (&prefconfig, MPI_COMM_WORLD);

      vsg_prtree2d_set_parallel (tree, &pconfig);

      vsg_prtree2d_set_parallel (treeref, &prefconfig);
    }
#endif

  /* insert points */
  for (i=0; i<_npoints; i++)
    {
      if (_verbose)
	{
	  test_printerr ("points[%d]: ", i);
	  vsg_vector2d_write (g_ptr_array_index (points_array, i), stderr);
	  g_printerr ("\n");
	}

      vsg_prtree2d_insert_point (tree, g_ptr_array_index (points_array, i));
      vsg_prtree2d_insert_point (treeref,
                                 g_ptr_array_index (pointsref_array, i));
    }

#ifdef VSG_HAVE_MPI
  if (_mpi)
    {
      vsg_prtree2d_migrate_flush (tree);
      vsg_prtree2d_migrate_flush (treeref);

      vsg_prtree2d_distribute_contiguous_leaves (tree);
      vsg_prtree2d_distribute_contiguous_leaves (treeref);
    }
#endif

  if (_write)
    {
      gchar fn[128];
      FILE *f;

      g_sprintf (fn, "tree-%03d.txt", rk);
      f = fopen (fn, "w");
      vsg_prtree2d_write (tree, f);
      fclose (f);
      g_sprintf (fn, "treeref-%03d.txt", rk);
      f = fopen (fn, "w");
      vsg_prtree2d_write (treeref, f);
      fclose (f);
    }

  /* compute neaf/far interactions for treeref */
  _far_count = 0;
  _do_upward_pass (treeref);
  vsg_prtree2d_near_far_traversal (treeref, (VsgPRTree2dFarInteractionFunc) _far,
                                   (VsgPRTree2dInteractionFunc) _near,
                                   &ret);
  vsg_prtree2d_traverse (treeref, G_PRE_ORDER, (VsgPRTree2dFunc) _down, NULL);
  ref_far_count = _far_count;

  /* compute neaf/far interactions for tree */
  _far_count = 0;
  _do_upward_pass (tree);
  vsg_prtree2d_near_far_traversal (tree, (VsgPRTree2dFarInteractionFunc) _far,
                                   (VsgPRTree2dInteractionFunc) _near,
                                   &ret);
  vsg_prtree2d_traverse (tree, G_PRE_ORDER, (VsgPRTree2dFunc) _down, NULL);

  /* migrate points back to processor 0 before checking */
  vsg_prtree2d_distribute_concentrate (tree, 0);
  vsg_prtree2d_distribute_concentrate (treeref, 0);

  /* check results */
  for (i=0; i<points_array->len; i++)
    {
      Pt *pt = g_ptr_array_index (points_array, i);
      /* FIXME: what if pointsref and points are no longer in the same order? */
      Pt *ptref = g_ptr_array_index (pointsref_array, i);

      if (pt->count != ptref->count)
        {
          test_printerr ("error pt[");
          vsg_vector2d_write (&pt->vector, stderr);
          g_printerr ("]=%ld ptref[", pt->count);
          vsg_vector2d_write (&ptref->vector, stderr);
          g_printerr ("]=%ld\n", ptref->count);
        }
      else if (_verbose)
        {
          test_printerr ("correct comparison [");
          vsg_vector2d_write (&pt->vector, stderr);
          g_printerr ("]=%ld ptref[", pt->count);
          vsg_vector2d_write (&ptref->vector, stderr);
          g_printerr ("]=%ld\n", ptref->count);
        }
    }

  if ((_expect_far_count >= 0) && (_expect_far_count != _far_count))
    test_printerr ("far_count=%d != expected far_count=%d (ref=%d)\n",
                   _far_count, _expect_far_count, ref_far_count);
  else if (_verbose)
    test_printerr ("far_count=%d == expected far_count=%d (ref=%d)\n",
                   _far_count, _expect_far_count, ref_far_count);

  /* remove the points */
  for (i=0; i<points_array->len; i++)
    {
      vsg_prtree2d_remove_point (tree, g_ptr_array_index (points_array, i));
      vsg_prtree2d_remove_point (treeref,
                                 g_ptr_array_index (pointsref_array, i));
    }

  /* destroy the trees */
  vsg_prtree2d_free (tree);
  vsg_prtree2d_free (treeref);

  for (i=0; i<points_array->len; i++)
    pt_destroy (g_ptr_array_index (points_array, i), FALSE, points_array);

  for (i=0; i<pointsref_array->len; i++)
    pt_destroy (g_ptr_array_index (pointsref_array, i), FALSE, pointsref_array);


  g_ptr_array_free (points_array, TRUE);
  g_ptr_array_free (pointsref_array, TRUE);

#ifdef VSG_HAVE_MPI
  if (_mpi)
    MPI_Finalize ();
#endif

  return ret;
}
