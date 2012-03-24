/* basic point insertion, removal and traversal on VsgPRTree2d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

#include <stdlib.h>
#include <string.h>

#include <glib/gprintf.h>

static gint rk = 0, sz = 1;

static gboolean _verbose = FALSE;
static gboolean _do_write = FALSE;
static gboolean _do_check = TRUE;
static gboolean _time = FALSE;
static gboolean _mpi = FALSE;

static guint _maxbox = 10;

void exterior_move (VsgVector2d *point, gpointer user_data);
static void points1 (guint *npoints, GPtrArray *array);

static GFunc _move = (GFunc) exterior_move;
static gpointer _move_data = NULL;
static guint _random_seed = 0;
static GRand *_rand;
static gdouble _random_move_scale = 1.;

static gdouble _theta = 0.1;
static VsgMatrix3d _rotate;

static guint _npoints = 4;
static GPtrArray *points_array = NULL;
static GPtrArray *pointsref_array = NULL;
static void (*_fill) (guint *npoints, GPtrArray *array) = points1;

void test_printerr (const gchar *fmt, ...)
{
  va_list ap;

  if (sz > 1)
    g_fprintf (stderr, "%d: ", rk);

  va_start (ap, fmt);
  g_vfprintf (stderr, fmt, ap);
  va_end (ap);
}

VsgVector2d *vsg_vector2d_alloc (gboolean resident, GPtrArray *ptarray)
{
  VsgVector2d *ret;
  ret = vsg_vector2d_new (0., 0.);

  if (resident)
    g_ptr_array_add (ptarray, ret);

  return ret;
}

void vsg_vector2d_destroy (VsgVector2d *v, gboolean resident,
                           GPtrArray *ptarray)
{
  if (resident)
    g_ptr_array_remove (ptarray, v);

  vsg_vector2d_free (v);
}

static void points1 (guint *npoints, GPtrArray *array)
{
  const VsgVector2d pts[] = {
    {-0.5, -0.5},
    {0.5, -0.5},
    {-0.5, 0.5},
    {0.5, 0.5},
  };
  guint n = sizeof (pts) / sizeof (VsgVector2d);
  gint i;

  for (i=0; i<n; i++)
  {
    VsgVector2d *point = vsg_vector2d_alloc (TRUE, array);
    vsg_vector2d_copy (&pts[i], point);
  }

  *npoints = n;
}

static void random_points (guint *npoints, GPtrArray *array)
{
  gint i;

  for (i=0; i<*npoints; i++)
    {
      VsgVector2d *point = vsg_vector2d_alloc (TRUE, array);
      point->x = g_rand_double_range (_rand, -1., 1.);
      point->y = g_rand_double_range (_rand, -1., 1.);
    }

}

gboolean old_move (VsgPRTree2d *tree, VsgPoint2 point, GFunc move_func,
		   gpointer move_data)
{
  gboolean found = vsg_prtree2d_remove_point (tree, point);

  if (! found) return FALSE;

  move_func (point, move_data);

  vsg_prtree2d_insert_point (tree, point);

  return TRUE;
}

void random_move (VsgVector2d *point, gdouble *scale)
{
  point->x += g_rand_double_range (_rand, -*scale, *scale);
  point->y += g_rand_double_range (_rand, -*scale, *scale);

  if (_verbose) test_printerr ("new pos: %f %f\n", point->x, point->y);
}

void exterior_move (VsgVector2d *point, gpointer user_data)
{
  if (point->x != 0.) point->x *= (1. + 1./fabs (point->x));
  if (point->y != 0.) point->y *= (1. + 1./fabs (point->y));

  if (_verbose) test_printerr ("new pos: %f %f\n", point->x, point->y);
}

void rotate_move (VsgVector2d *point, VsgMatrix3d *rotation)
{
  vsg_matrix3d_vecmult (rotation, point, point);

  if (_verbose) test_printerr ("new pos: %f %f\n", point->x, point->y);
}

void zero_move (VsgVector2d *point, VsgMatrix3d *rotation)
{
}

void point_checksum (VsgVector2d *point, GChecksum *checksum)
{
  g_checksum_update (checksum, (guchar *) point, sizeof (VsgVector2d));

  if (_verbose)
    {
      test_printerr ("");
      vsg_vector2d_write (point, stderr);
      g_printerr ("\n");
    }
}

#define KEY2D_CHKSUM_SIZE  (2*sizeof (guint64) + sizeof (guint8))

/* tree traversal function that compute checksum for a tree */
static void traverse_checksum (VsgPRTree2dNodeInfo *node_info,
			       GChecksum *checksum)
{
  g_checksum_update (checksum, (guchar *) &node_info->id, KEY2D_CHKSUM_SIZE);
  if (_verbose)
    {
      test_printerr ("");
      vsg_prtree_key2d_write (&node_info->id, stderr);
      g_printerr ("\n");
    }
  g_slist_foreach (node_info->point_list, (GFunc) point_checksum, checksum);
}
static GChecksum *vsg_prtree2d_checksum (VsgPRTree2d *tree, GChecksumType type)
{
  GChecksum *checksum = g_checksum_new (type);

  vsg_prtree2d_traverse (tree, G_PRE_ORDER,
			 (VsgPRTree2dFunc) traverse_checksum, checksum);

  return checksum;
}

#ifdef VSG_HAVE_MPI

void vsg_vector2d_migrate_pack (VsgVector2d *v, VsgPackedMsg *pm,
                                 gpointer user_data)
{
  vsg_packed_msg_send_append (pm, v, 1, VSG_MPI_TYPE_VECTOR2D);
}

void vsg_vector2d_migrate_unpack (VsgVector2d *v, VsgPackedMsg *pm,
                                  gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, v, 1, VSG_MPI_TYPE_VECTOR2D);
}

static VsgPRTreeParallelConfig pconfig = {
  /* Point VTable */
  {(VsgMigrableAllocDataFunc) NULL, NULL,
   (VsgMigrableDestroyDataFunc) NULL, NULL,
   {(VsgMigrablePackDataFunc) vsg_vector2d_migrate_pack, NULL,
    (VsgMigrableUnpackDataFunc) vsg_vector2d_migrate_unpack, NULL,
    NULL, NULL},
  },
  {},{},
  MPI_COMM_WORLD,
};

static VsgPRTreeParallelConfig prefconfig = {
  /* Point VTable */
  {(VsgMigrableAllocDataFunc) NULL, NULL,
   (VsgMigrableDestroyDataFunc) NULL, NULL,
   {(VsgMigrablePackDataFunc) vsg_vector2d_migrate_pack, NULL,
    (VsgMigrableUnpackDataFunc) vsg_vector2d_migrate_unpack, NULL,
    NULL, NULL},
  },
  {},{},
  MPI_COMM_WORLD,
};
#endif

static
void parse_args (int argc, char **argv)
{
  int iarg = 1;
  char *arg;

  _rand = g_rand_new_with_seed (_random_seed);
  vsg_matrix3d_identity (&_rotate);

  while (iarg < argc)
    {
      arg = argv[iarg];

      if (g_ascii_strncasecmp (arg, "--seed", 6) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            _random_seed = tmp;
	  else
	    test_printerr ("Invalid random seed (--seed %s)\n", arg);
	}
      else if (g_ascii_strncasecmp (arg, "--maxbox", 8) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            _maxbox = tmp;
	  else
	    test_printerr ("Invalid maxbox (--maxbox %s)\n", arg);
	}
      else if (g_ascii_strncasecmp (arg, "--move", 6) == 0)
	{
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (g_ascii_strncasecmp (arg, "random", 6) == 0)
            {
              if (strlen (arg) > 6)
                _random_move_scale = atof (arg+6);

              _move = (GFunc) random_move;
	      _move_data = &_random_move_scale;
            }
          else if (g_ascii_strncasecmp (arg, "exterior", 8) == 0)
            {
              _move = (GFunc) exterior_move;
	      _move_data = NULL;
            }
           else if (g_ascii_strncasecmp (arg, "rotate", 6) == 0)
            {
              if (strlen (arg) > 6)
                _theta = atof (arg+6);
              vsg_matrix3d_rotate (&_rotate, _theta);

              _move = (GFunc) rotate_move;
	      _move_data = &_rotate;
            }
           else if (g_ascii_strncasecmp (arg, "zero", 6) == 0)
            {
              _move = (GFunc) zero_move;
	      _move_data = NULL;
            }
          else 
            {
              test_printerr ("Invalid move function name \"%s\"\n", arg);
            }
	}
      else if (g_ascii_strncasecmp (arg, "--fill", 6) == 0)
	{
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (g_ascii_strncasecmp (arg, "corners", 4) == 0)
            {
              _fill = points1;
            }
          else if (g_ascii_strncasecmp (arg, "random", 8) == 0)
            {
	      _fill = random_points;
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
      else if (g_strncasecmp (arg, "--write", 7) == 0)
        {
          _do_write = TRUE;
        }
      else if (g_strncasecmp (arg, "--no-check", 9) == 0)
        {
          _do_check = FALSE;
        }
      else if (g_strncasecmp (arg, "-v", 2) == 0 ||
               g_strncasecmp (arg, "--verbose", 9) == 0)
        {
          _verbose = TRUE;
        }
      else if (g_strncasecmp (arg, "-t", 2) == 0 ||
               g_strncasecmp (arg, "--time", 6) == 0)
        {
          _time = TRUE;
        }
      else if (g_ascii_strcasecmp (arg, "--version") == 0)
	{
	  test_printerr ("%s version %s\n", argv[0], PACKAGE_VERSION);
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
  GChecksum *checksum, *checksumref;
  const gchar *chkstr, *chkstrref;
  GTimer *timer;
  gdouble t1, t2;

  vsg_init_gdouble ();

  parse_args (argc, argv);

#ifdef VSG_HAVE_MPI
  if (_mpi)
    {
      MPI_Init (&argc, &argv);

      MPI_Comm_size (MPI_COMM_WORLD, &sz);
      MPI_Comm_rank (MPI_COMM_WORLD, &rk);
    }
#endif

  points_array = g_ptr_array_new ();

  if (rk == 0) _fill (&_npoints, points_array);
  else _npoints = 0;

  pointsref_array = g_ptr_array_sized_new (_npoints);
  for (i=0; i<_npoints; i++)
    {
      VsgVector2d *ptref = vsg_vector2d_alloc (TRUE, pointsref_array);
      vsg_vector2d_copy ((VsgVector2d *) g_ptr_array_index (points_array, i),
                         ptref);
    }

  /* create the trees */
  tree =
    vsg_prtree2d_new_full (&lb, &ub,
                           (VsgPoint2dLocFunc) vsg_vector2d_vector2d_locfunc,
                           (VsgPoint2dDistFunc) vsg_vector2d_dist,
                           NULL, _maxbox);
  treeref = vsg_prtree2d_clone (tree);

#ifdef VSG_HAVE_MPI
  if (_mpi)
    {
      pconfig.point.alloc = (VsgMigrableAllocDataFunc) vsg_vector2d_alloc;
      pconfig.point.alloc_data = points_array;
      pconfig.point.destroy = (VsgMigrableDestroyDataFunc) vsg_vector2d_destroy;
      pconfig.point.destroy_data = points_array;

      prefconfig.point.alloc = (VsgMigrableAllocDataFunc) vsg_vector2d_alloc;
      prefconfig.point.alloc_data = pointsref_array;
      prefconfig.point.destroy = (VsgMigrableDestroyDataFunc) vsg_vector2d_destroy;
      prefconfig.point.destroy_data = pointsref_array;

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
	  test_printerr ("\n");
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

  /* move points */

  timer = g_timer_new ();
  g_rand_set_seed (_rand, 0);
  for (i=0; i<points_array->len; i++)
  {
    vsg_prtree2d_move_point (tree, g_ptr_array_index (points_array, i), _move,
                             _move_data);
  }
#ifdef VSG_HAVE_MPI
  if (_mpi)
    {
      vsg_prtree2d_migrate_flush (tree);
    }
#endif
  t1 = g_timer_elapsed (timer, NULL);

  g_rand_set_seed (_rand, 0);
  for (i=0; i<pointsref_array->len; i++)
  {
    old_move (treeref, g_ptr_array_index (pointsref_array, i), _move,
              _move_data);
  }
#ifdef VSG_HAVE_MPI
  if (_mpi)
    {
      vsg_prtree2d_migrate_flush (treeref);
    }
#endif
  t2 = g_timer_elapsed (timer, NULL) - t1;

  g_timer_destroy (timer);

  checksum = vsg_prtree2d_checksum (tree, G_CHECKSUM_MD5);
  checksumref = vsg_prtree2d_checksum (treeref, G_CHECKSUM_MD5);

  chkstr = g_checksum_get_string (checksum);
  chkstrref = g_checksum_get_string (checksumref);

  if (_do_check && g_strcasecmp (chkstr, chkstrref) != 0)
    {

      test_printerr ("Tree checksums difference found\n");
      test_printerr ("ref = \"%s\"\n", chkstrref);
      test_printerr ("res = \"%s\"\n", chkstr);

#ifdef VSG_HAVE_MPI
      if (_mpi)
        MPI_Barrier (MPI_COMM_WORLD);
#endif
      test_printerr ("-------------------------------\n");
      vsg_prtree2d_write (tree, stderr);
#ifdef VSG_HAVE_MPI
      if (_mpi)
        MPI_Barrier (MPI_COMM_WORLD);
#endif
      test_printerr ("-------------------------------\n");
      vsg_prtree2d_write (treeref, stderr);
    }
  else if (_verbose)
    {
      test_printerr ("ref = \"%s\"\n", chkstrref);
      test_printerr ("res = \"%s\"\n", chkstr);

#ifdef VSG_HAVE_MPI
      if (_mpi)
        MPI_Barrier (MPI_COMM_WORLD);
#endif
      test_printerr ("-------------------------------\n");
      vsg_prtree2d_write (tree, stderr);
#ifdef VSG_HAVE_MPI
      if (_mpi)
        MPI_Barrier (MPI_COMM_WORLD);
#endif
      test_printerr ("-------------------------------\n");
      vsg_prtree2d_write (treeref, stderr);
    }

  g_checksum_free (checksum);
  g_checksum_free (checksumref);

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
    vsg_vector2d_destroy (g_ptr_array_index (points_array, i), FALSE,
                          points_array);

  for (i=0; i<pointsref_array->len; i++)
    vsg_vector2d_destroy (g_ptr_array_index (pointsref_array, i), FALSE,
                          pointsref_array);


  g_ptr_array_free (points_array, TRUE);
  g_ptr_array_free (pointsref_array, TRUE);

  g_rand_free (_rand);

  if (_time)
    {
      test_printerr ("times: move=%gs remove/insert=%gs\n", t1, t2);
    }

#ifdef VSG_HAVE_MPI
  if (_mpi)
    MPI_Finalize ();
#endif

  return ret;
}
