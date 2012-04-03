/* basic point insertion, removal and traversal on a cloned VsgPRTree3d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include "glib/gprintf.h"

#include <stdlib.h>
#include <math.h>
#include <glib.h>

/* option variables */
static gboolean _do_write = FALSE;
static gint _np = 12;
static guint32 _random_seed = 0;
static gint _flush_interval = 1000;
static gboolean _hilbert = FALSE;
static gboolean _verbose = FALSE;
static gint _maxbox = 2;
static gint nc_padding = 0;
static gint _far_slowdown = 0;
static gint _near_slowdown = 0;

/* global variables */
static gint rk, sz;
static glong _ref_count = 0;
static gchar *_nc_padding_buffer = NULL;

/* statistics counters */
static gint _near_count = 0;
static gint _far_count = 0;
static gint _fw_count = 0;
static gint _bw_count = 0;

typedef struct _Pt Pt;
typedef struct _NodeCounter NodeCounter;

struct _Pt
{
  VsgVector3d vector;
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

/* migration pack/unpack functions */
void nc_migrate_pack (NodeCounter *nc, VsgPackedMsg *pm,
                      gpointer user_data)
{
  vsg_packed_msg_send_append (pm, nc, 2, MPI_LONG);
}

void nc_migrate_unpack (NodeCounter *nc, VsgPackedMsg *pm,
                        gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, nc, 2, MPI_LONG);
}

/* visit forward pack/unpack functions */
/* only the coordinates part is transmitted */
void nc_visit_fw_pack (NodeCounter *nc, VsgPackedMsg *pm,
                       gpointer user_data)
{
  vsg_packed_msg_send_append (pm, &nc->in_count, 1, MPI_LONG);

  if (nc_padding != 0)
    vsg_packed_msg_send_append (pm, _nc_padding_buffer, nc_padding, MPI_CHAR);

  _fw_count ++;
}

void nc_visit_fw_unpack (NodeCounter *nc, VsgPackedMsg *pm,
                         gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &nc->in_count, 1, MPI_LONG);

  if (nc_padding != 0)
    vsg_packed_msg_recv_read (pm, _nc_padding_buffer, nc_padding, MPI_CHAR);
}

void nc_visit_fw_reduce (NodeCounter *a, NodeCounter *b, gpointer user_data)
{
  b->in_count += a->in_count;
}

/* visit forward pack/unpack functions */
/* only the count part is transmitted */
void nc_visit_bw_pack (NodeCounter *nc, VsgPackedMsg *pm,
                       gpointer user_data)

{
/*   g_printerr ("%d : pack out %d\n", rk, nc->out_count); */
  vsg_packed_msg_send_append (pm, &nc->out_count, 1, MPI_LONG);

  if (nc_padding != 0)
    vsg_packed_msg_send_append (pm, _nc_padding_buffer, nc_padding, MPI_CHAR);

  _bw_count ++;
}

void nc_visit_bw_unpack (NodeCounter *nc, VsgPackedMsg *pm,
                         gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &nc->out_count, 1, MPI_LONG);

  if (nc_padding != 0)
    vsg_packed_msg_recv_read (pm, _nc_padding_buffer, nc_padding, MPI_CHAR);
/*   g_printerr ("%d : unpack out %d (sum=%d)\n", rk, count, nc->out_count); */
}

void nc_visit_bw_reduce (NodeCounter *a, NodeCounter *b, gpointer user_data)
{
  b->out_count += a->out_count;
}

static GPtrArray *points = NULL;

Pt *pt_alloc (gboolean resident, gpointer user_data)
{
  Pt *ret;
  ret = g_malloc (sizeof (Pt));

  if (resident)
    g_ptr_array_add (points, ret);

  return ret;
}

void pt_destroy (Pt *data, gboolean resident,
                 gpointer user_data)
{
  if (resident)
    g_ptr_array_remove (points, data);

  g_free (data);
}

/* migration pack/unpack functions */
void pt_migrate_pack (Pt *pt, VsgPackedMsg *pm,
                      gpointer user_data)
{
  vsg_packed_msg_send_append (pm, &pt->vector, 1, VSG_MPI_TYPE_VECTOR3D);
  vsg_packed_msg_send_append (pm, &pt->weight, 1, MPI_INT);
  vsg_packed_msg_send_append (pm, &pt->count, 1, MPI_LONG);
}

void pt_migrate_unpack (Pt *pt, VsgPackedMsg *pm,
                        gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &pt->vector, 1, VSG_MPI_TYPE_VECTOR3D);
  vsg_packed_msg_recv_read (pm, &pt->weight, 1, MPI_INT);
  vsg_packed_msg_recv_read (pm, &pt->count, 1, MPI_LONG);
}

/* visit forward pack/unpack functions */
/* only the coordinates part is transmitted */
void pt_visit_fw_pack (Pt *pt, VsgPackedMsg *pm,
                       gpointer user_data)
{
  vsg_packed_msg_send_append (pm, &pt->vector, 1, VSG_MPI_TYPE_VECTOR3D);
  vsg_packed_msg_send_append (pm, &pt->weight, 1, MPI_INT);
}

void pt_visit_fw_unpack (Pt *pt, VsgPackedMsg *pm,
                         gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &pt->vector, 1, VSG_MPI_TYPE_VECTOR3D);
  vsg_packed_msg_recv_read (pm, &pt->weight, 1, MPI_INT);
  pt->count = 0;
}

/* visit forward pack/unpack functions */
/* only the count part is transmitted */
void pt_visit_bw_pack (Pt *pt, VsgPackedMsg *pm,
                       gpointer user_data)
{
  vsg_packed_msg_send_append (pm, &pt->count, 1, MPI_LONG);
}

void pt_visit_bw_unpack (Pt *pt, VsgPackedMsg *pm,
                         gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &pt->count, 1, MPI_LONG);
}

void pt_visit_bw_reduce (Pt *a, Pt *b, gpointer user_data)
{
  b->count += a->count;
}

void empty_array (gpointer var, gpointer data)
{
  g_free (var);
}


static VsgPRTreeParallelConfig pconfig = {
  /* Point VTable */
  {(VsgMigrableAllocDataFunc) pt_alloc, NULL,
   (VsgMigrableDestroyDataFunc) pt_destroy, NULL,
   {(VsgMigrablePackDataFunc) pt_migrate_pack, NULL,
    (VsgMigrableUnpackDataFunc) pt_migrate_unpack, NULL,
    NULL, NULL},
   {(VsgMigrablePackDataFunc) pt_visit_fw_pack, NULL,
    (VsgMigrableUnpackDataFunc) pt_visit_fw_unpack, NULL,
    NULL, NULL},
   {(VsgMigrablePackDataFunc) pt_visit_bw_pack, NULL,
    (VsgMigrableUnpackDataFunc) pt_visit_bw_unpack, NULL,
    (VsgMigrableReductionDataFunc) pt_visit_bw_reduce, NULL},
  },
  /* Region VTable */
  {NULL, NULL, NULL, NULL,
   {NULL, NULL, NULL, NULL, NULL, NULL},
   {NULL, NULL, NULL, NULL, NULL, NULL},
   {NULL, NULL, NULL, NULL, NULL, NULL},
  },
  /* NodeData VTable */
  {node_counter_alloc, NULL,
   node_counter_destroy, NULL,
   {(VsgMigrablePackDataFunc) nc_migrate_pack, NULL,
    (VsgMigrableUnpackDataFunc) nc_migrate_unpack, NULL,
    NULL, NULL},
   {(VsgMigrablePackDataFunc) nc_visit_fw_pack, NULL,
    (VsgMigrableUnpackDataFunc) nc_visit_fw_unpack, NULL,
    (VsgMigrableReductionDataFunc) nc_visit_fw_reduce, NULL},
   {(VsgMigrablePackDataFunc) nc_visit_bw_pack, NULL,
    (VsgMigrableUnpackDataFunc) nc_visit_bw_unpack, NULL,
    (VsgMigrableReductionDataFunc) nc_visit_bw_reduce, NULL},
  },
  /*communicator */
  MPI_COMM_WORLD,
};

static void _pt_write (Pt *pt, FILE *file)
{
  fprintf (file, "%g %g %g\n",
           pt->vector.x, pt->vector.y, pt->vector.z);
}

static void _traverse_bg_write (VsgPRTree3dNodeInfo *node_info, FILE *file)
{
  if (! VSG_PRTREE3D_NODE_INFO_IS_REMOTE (node_info))
    {
      gdouble x = node_info->center.x;
      gdouble y = node_info->center.y;
      gdouble z = node_info->center.z;
      gdouble dx = node_info->ubound.x - node_info->center.x;
      gdouble dy = node_info->ubound.y - node_info->center.y;
      gdouble dz = node_info->ubound.z - node_info->center.z;

      fprintf (file, "%g %g %g\n", x - dx, y, z);
      fprintf (file, "%g %g %g\n", x + dx, y, z);

      fprintf (file, "%g %g %g\n", x, y - dy, z);
      fprintf (file, "%g %g %g\n", x, y + dy, z);

      fprintf (file, "%g %g %g\n", x, y, z - dz);
      fprintf (file, "%g %g %g\n", x, y, z + dz);
    }
}

static void _traverse_fg_write (VsgPRTree3dNodeInfo *node_info, FILE *file)
{
  g_slist_foreach (node_info->point_list, (GFunc) _pt_write, file);
}

static void _traverse_count_local_nodes (VsgPRTree3dNodeInfo *node_info,
                                         gint *count)
{
  if (! VSG_PRTREE3D_NODE_INFO_IS_REMOTE (node_info))
    {
      (*count) ++;
    }
}

static void _tree_write (VsgPRTree3d *tree, gchar *prefix)
{
  gchar fn[1024];
  FILE *f;
  gint np = vsg_prtree3d_point_count (tree);
  gint nn = 0;
  gint i;

  g_sprintf (fn, "%s%03d.vtp", prefix, rk);
  f = fopen (fn, "w");

  vsg_prtree3d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree3dFunc) _traverse_count_local_nodes,
                         &nn);

  fprintf (f, "<?xml version=\"1.0\" ?>\n" \
           "<VTKFile type=\"PolyData\" version=\"0.1\">\n" \
           "<PolyData>\n" \
           "<Piece NumberOfPoints=\"%d\" NumberOfVerts=\"%d\" " \
           "NumberOfLines=\"%d\" NumberOfStrips=\"0\" " \
           "NumberOfPolys=\"0\">\n",
           np + 6*nn, np, 3*nn);

  fprintf (f, "<Points>\n");
  fprintf (f, "<DataArray type=\"Float64\" NumberOfComponents=\"3\" " \
           "format=\"ascii\">\n");
  vsg_prtree3d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree3dFunc) _traverse_fg_write,
                         f);
  vsg_prtree3d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree3dFunc) _traverse_bg_write,
                         f);
  fprintf (f, "</DataArray>\n</Points>\n");

  fprintf (f, "<Verts>\n");
  fprintf (f, "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n");
  for (i=0; i<np; i++) fprintf (f, "%d ", i);
  fprintf (f, "\n</DataArray>\n");
  fprintf (f, "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\" >");
  for (i=0; i<np; i++) fprintf (f, "%d ", i+1);
  fprintf (f, "\n</DataArray>\n</Verts>\n");

  fprintf (f, "<Lines>\n");
  fprintf (f, "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n");
  for (i=0; i<3*nn; i++) fprintf (f, "%d %d ", np + 2*i, np + 2*i+1);
  fprintf (f, "\n</DataArray>\n");
  fprintf (f, "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\" >");
  for (i=0; i<3*nn; i++) fprintf (f, "%d ", 2*(i+1));
  fprintf (f, "\n</DataArray>\n</Lines>\n");



  fprintf (f, "\n</Piece>\n");
  fprintf (f, "</PolyData>\n</VTKFile>\n");
  fclose (f);

}

static void (*_distribute) (VsgPRTree3d *tree) =
  vsg_prtree3d_distribute_contiguous_leaves;

void _random_fill (VsgPRTree3d *tree, guint np);

static void (*_fill) (VsgPRTree3d *tree, guint np) = _random_fill;

void _random_fill (VsgPRTree3d *tree, guint np)
{
  gint i;
  Pt *pt;
  VsgVector3d lb, ub;
  GRand *rand = g_rand_new_with_seed (_random_seed);

  vsg_prtree3d_get_bounds (tree, &lb, &ub);

  _ref_count = 0;

  for (i=0; i< np; i++)
    {
      gdouble x1, y1, z1;
      gint c;

      x1 = g_rand_double_range (rand, lb.x, ub.x);
      y1 = g_rand_double_range (rand, lb.y, ub.y);
      z1 = g_rand_double_range (rand, lb.z, ub.z);

      c = i+1;

      _ref_count += c;

      if (i%_flush_interval == 0)
        {
          vsg_prtree3d_migrate_flush (tree);
          if (i%(_flush_interval*10) == 0)
            {
              if (_verbose && rk == 0)
                g_printerr ("%d: contiguous dist before %dth point\n", rk, i);
              _distribute (tree);
            }
        }

      if (i%sz != rk) continue;

      if (i % 10000 == 0 && _verbose)
        g_printerr ("%d: insert %dth point\n", rk, i);

      pt = pt_alloc (TRUE, NULL);
      pt->vector.x = x1;
      pt->vector.y = y1;
      pt->vector.z = z1;
      pt->weight = c;

      vsg_prtree3d_insert_point (tree, pt);
    }

  vsg_prtree3d_migrate_flush (tree);

  _distribute (tree);

  g_rand_free (rand);
}

/* fadster algorithm for random distribution */
static void _random2_fill (VsgPRTree3d *tree, guint np)
{
  gint i;
  Pt *pt;
  VsgVector3d lb, ub;
  GRand *rand = g_rand_new_with_seed (_random_seed);

  vsg_prtree3d_get_bounds (tree, &lb, &ub);

  pt = pt_alloc (TRUE, NULL);

  for (i=0; i< np; i++)
    {
      gdouble x1, y1, z1;
      gint c;

      x1 = g_rand_double_range (rand, lb.x, ub.x);
      y1 = g_rand_double_range (rand, lb.y, ub.y);
      z1 = g_rand_double_range (rand, lb.z, ub.z);
      c = i+1;

      pt->vector.x = x1;
      pt->vector.y = y1;
      pt->vector.z = z1;
      pt->weight = c;

      _ref_count += c;

      if (vsg_prtree3d_insert_point_local (tree, pt))
        {
          if (i % 10000 == 0 && _verbose)
            g_printerr ("%d: insert %dth point\n", rk, i);

          pt = pt_alloc (TRUE, NULL);
        }

      if (i%(_flush_interval*10) == 0)
        {
          if (_verbose && rk == 0)
            g_printerr ("%d: contiguous dist before %dth point\n", rk, i);
          _distribute (tree);
        }
    }

  pt_destroy (pt, TRUE, NULL);

  _distribute (tree);

  g_rand_free (rand);
}

/* UV sphere fill */
static void _uvsphere_fill (VsgPRTree3d *tree, guint np)
{
  gint i;
  Pt *pt;
  VsgVector3d lb, ub;
  GRand *rand = g_rand_new_with_seed (_random_seed);
  gdouble r;

  vsg_prtree3d_get_bounds (tree, &lb, &ub);
  r = MIN (lb.x, ub.x);

  pt = pt_alloc (TRUE, NULL);

  for (i=0; i< np; i++)
    {
      gdouble theta, phi;
      gint c;

      theta = g_rand_double_range (rand, 0.01 * G_PI, 0.99 * G_PI);
      phi = g_rand_double_range (rand, 0., 2.*G_PI);

      c = i+1;

      vsg_vector3d_from_spherical (&pt->vector, r, theta, phi);
      pt->weight = c;

      _ref_count += c;

      if (vsg_prtree3d_insert_point_local (tree, pt))
        {
          if (i % 10000 == 0 && _verbose)
            g_printerr ("%d: insert %dth point\n", rk, i);

          pt = pt_alloc (TRUE, NULL);
        }

      if (i%(_flush_interval*10) == 0)
        {
          if (_verbose && rk == 0)
            g_printerr ("%d: contiguous dist before %dth point\n", rk, i);
          _distribute (tree);
        }
    }

  pt_destroy (pt, TRUE, NULL);

  _distribute (tree);

  g_rand_free (rand);
}

/* faster algorithm for circle distribution */
void _circle_fill (VsgPRTree3d *tree, guint np)
{
  gint i;
  Pt *pt;
  const gdouble r = 0.95;
  const gdouble sqrt_2 = sqrt (2.);
  gdouble dtheta = 2. * G_PI / (np-1) / sz;
  gdouble theta0 = 2. * rk * G_PI / sz;

  for (i=0; i<np; i++)
    {
      gint c;

      c = i+1;

      _ref_count += c;

      if (i%_flush_interval == 0)
        {
          vsg_prtree3d_migrate_flush (tree);
          if (i%(_flush_interval*10) == 0)
            {
              if (_verbose && rk == 0)
                g_printerr ("%d: contiguous dist before %dth point\n", rk, i);
              _distribute (tree);
            }
        }
      if (i%sz != rk) continue;

      if (i % 10000 == 0 && _verbose)
        g_printerr ("%d: insert %dth point\n", rk, i);

      pt = pt_alloc (TRUE, NULL);

      pt->vector.x = r * cos (theta0 + i * dtheta);
      pt->vector.y = r * sin (theta0 + i * dtheta) / sqrt_2;
      pt->vector.z = r * sin (theta0 + i * dtheta) / sqrt_2;
      pt->weight = c;

      vsg_prtree3d_insert_point (tree, pt);
    }

  vsg_prtree3d_migrate_flush (tree);
  _distribute (tree);
}

void _circle2_fill (VsgPRTree3d *tree, guint np)
{
  gint i;
  Pt *pt;
  const gdouble r = 0.95;
  const gdouble sqrt_2 = sqrt (2.);
  gdouble dtheta = 2. * G_PI / (np-1);
  gdouble theta0 = 0.;

  pt = pt_alloc (TRUE, NULL);

  for (i=0; i<np; i++)
    {
      gint c;

      c = i+1;

      _ref_count += c;

      pt->vector.x = r * cos (theta0 + i * dtheta);
      pt->vector.y = r * sin (theta0 + i * dtheta) / sqrt_2;
      pt->vector.z = r * sin (theta0 + i * dtheta) / sqrt_2;
      pt->weight = c;

      if (vsg_prtree3d_insert_point_local (tree, pt))
        {
          if (i % 10000 == 0 && _verbose)
            g_printerr ("%d: insert %dth point\n", rk, i);

          pt = pt_alloc (TRUE, NULL);
        }

      if (i%(_flush_interval*10) == 0)
        {
          if (_verbose && rk == 0)
            g_printerr ("%d: contiguous dist before %dth point\n", rk, i);
          _distribute (tree);
        }
    }

  pt_destroy (pt, TRUE, NULL);

  _distribute (tree);
}

static
void parse_args (int argc, char **argv)
{
  int iarg = 1;
  char *arg;

  while (iarg < argc)
    {
      arg = argv[iarg];

      if (g_ascii_strncasecmp (arg, "--np", 4) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            _np = tmp;
	  else
	    g_printerr ("Invalid particles number (--np %s)\n", arg);
	}
      else if (g_ascii_strncasecmp (arg, "--seed", 6) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            _random_seed = tmp;
	  else
	    g_printerr ("Invalid random seed (--seed %s)\n", arg);
	}
      else if (g_ascii_strncasecmp (arg, "--fill", 6) == 0)
	{
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (g_ascii_strncasecmp (arg, "uvsphere", 8) == 0)
            {
              _fill = _uvsphere_fill;      
            }
          else if (g_ascii_strncasecmp (arg, "random2", 7) == 0)
            {
              _fill = _random2_fill;      
            }
          else if (g_ascii_strncasecmp (arg, "circle2", 7) == 0)
            {
              _fill = _circle2_fill;
            }
           else if (g_ascii_strncasecmp (arg, "random", 6) == 0)
            {
              _fill = _random_fill;      
            }
          else if (g_ascii_strncasecmp (arg, "circle", 6) == 0)
            {
              _fill = _circle_fill;      
            }
          else 
            {
              g_printerr ("Invalid fill function name \"%s\"\n", arg);
            }
	}
      else if (g_ascii_strncasecmp (arg, "--dist", 6) == 0)
	{
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (g_ascii_strncasecmp (arg, "contiguous", 11) == 0)
            {
              _distribute = vsg_prtree3d_distribute_contiguous_leaves;
            }
          else if (g_ascii_strncasecmp (arg, "scatter", 7) == 0)
            {
              _distribute = vsg_prtree3d_distribute_scatter_leaves;
            }
          else
            {
              g_printerr ("Invalid distribution function name \"%s\"\n", arg);
            }
	}
      else if (g_strncasecmp (arg, "--hilbert", 9) == 0)
        {
          _hilbert = TRUE;
        }
      else if (g_strncasecmp (arg, "--write", 7) == 0)
        {
          _do_write = TRUE;
        }
      else if (g_strncasecmp (arg, "-v", 2) == 0 ||
               g_strncasecmp (arg, "--verbose", 9) == 0)
        {
          _verbose = TRUE;
        }
      else if (g_ascii_strncasecmp (arg, "--maxbox", 4) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            _maxbox = tmp;
	  else
	    g_printerr ("Invalid maximum particles / box number " \
                        "(--maxbox %s)\n", arg);
	}
      else if (g_ascii_strncasecmp (arg, "--nc-size", 9) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            nc_padding = MAX (0, tmp - sizeof (NodeCounter));
	  else
	    g_printerr ("Invalid value for NodeCounter padding "
                        "(--nc-size %s)\n", arg);
	}
      else if (g_ascii_strncasecmp (arg, "--far-slowdown", 14) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            _far_slowdown = MAX (0, tmp);
	  else
	    g_printerr ("Invalid value for far interaction slowdown factor "
                        "(--far-slowdown %s)\n", arg);
	}
      else if (g_ascii_strncasecmp (arg, "--near-slowdown", 15) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            _near_slowdown = MAX (0, tmp);
	  else
	    g_printerr ("Invalid value for near interaction slowdown factor "
                        "(--near-slowdown %s)\n", arg);
	}
      else if (g_ascii_strncasecmp (arg, "--nf-slowdown", 15) == 0)
	{
	  guint tmp = 0;
	  iarg ++;

	  arg = (iarg<argc) ? argv[iarg] : NULL;

	  if (sscanf (arg, "%u", &tmp) == 1)
            {
              _near_slowdown = MAX (0, tmp);
              _far_slowdown = MAX (0, tmp);
            }
	  else
	    g_printerr ("Invalid value for near/far interaction slowdown "
                        "factor (--nf-slowdown %s)\n", arg);
	}
      else if (g_ascii_strcasecmp (arg, "--version") == 0)
	{
	  g_printerr ("%s version %s\n", argv[0], PACKAGE_VERSION);
	  exit (0);
	}
      else
	{
	  g_printerr ("Invalid argument \"%s\"\n", arg);
	}

      iarg ++;
    }
}

void pt_get_weight (Pt *pt, glong *count)
{
  *count += pt->weight;
}

void pt_add_count (Pt *pt, glong *count)
{
  pt->count += *count;
}

void _near (VsgPRTree3dNodeInfo *one_info,
            VsgPRTree3dNodeInfo *other_info,
            gint *err)
{
  glong one_count = 0, other_count = 0;

  if (!(one_info->isleaf && other_info->isleaf))
    {
      g_printerr ("ERROR: call for near_func on non leaf node [");
      vsg_prtree_key3d_write (&one_info->id, stderr);
      g_printerr ("]=%d / [", one_info->isleaf);
      vsg_prtree_key3d_write (&other_info->id, stderr);
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

/*   if (_do_write && VSG_PRTREE3D_NODE_INFO_IS_REMOTE (one_info)) */
/*     { */
/*       gchar fn[1024]; */
/*       FILE *f; */
/*       sprintf (fn, "comm-%03d.svg", rk); */
/*       f = fopen (fn, "a"); */

/*       fprintf (f, "<!--s=%d--><polyline points=\"%g,%g %g,%g\" "        \ */
/*                "style=\"stroke:#00FF00;\"/>\n", */
/*                rk, */
/*                one_info->center.x, -one_info->center.y, */
/*                other_info->center.x, -other_info->center.y); */
/*       fclose (f); */
/*     } */

  {
    long i, j = 0;
    for (i = 0; i< _near_slowdown; i++)
      {
        j = j + i;
      }
    if (j != _near_slowdown*(_near_slowdown-1)/2) g_printerr ("oops\n");
  }
  _near_count ++;
}

void _far (VsgPRTree3dNodeInfo *one_info,
           VsgPRTree3dNodeInfo *other_info,
           gint *err)
{
  ((NodeCounter *) one_info->user_data)->out_count +=
    ((NodeCounter *) other_info->user_data)->in_count;

  ((NodeCounter *) other_info->user_data)->out_count +=
    ((NodeCounter *) one_info->user_data)->in_count;

  if ((one_info->point_count == 0 &&
       VSG_PRTREE3D_NODE_INFO_IS_LOCAL (one_info)) ||
      (other_info->point_count == 0 &&
       VSG_PRTREE3D_NODE_INFO_IS_LOCAL (other_info)))
    g_printerr ("%d : unnecessary far call\n", rk);

/*   if (_do_write && VSG_PRTREE3D_NODE_INFO_IS_REMOTE (one_info)) */
/*     { */
/*       gchar fn[1024]; */
/*       FILE *f; */
/*       sprintf (fn, "comm-%03d.svg", rk); */
/*       f = fopen (fn, "a"); */

/*       fprintf (f, "<!--s=%d--><polyline points=\"%g,%g %g,%g\" "        \ */
/*                "style=\"stroke:#0000FF;\"/>\n", */
/*                rk, */
/*                one_info->center.x, -one_info->center.y, */
/*                other_info->center.x, -other_info->center.y); */
/*       fclose (f); */
/*     } */

  _far_count ++;

  {
    long i, j = 0;
    for (i = 0; i< _far_slowdown; i++)
      {
        j = j + i;
      }
  }
}

void _up (VsgPRTree3dNodeInfo *node_info, gpointer data)
{
  if (! VSG_PRTREE3D_NODE_INFO_IS_REMOTE (node_info))
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

void _zero (VsgPRTree3dNodeInfo *node_info, gpointer data)
{
  if (! VSG_PRTREE3D_NODE_INFO_IS_REMOTE (node_info))
    {
      ((NodeCounter *) node_info->user_data)->in_count = 0;
      ((NodeCounter *) node_info->user_data)->out_count = 0;

      if (node_info->isleaf)
        g_slist_foreach (node_info->point_list, (GFunc) _zero_pt, NULL);
    }
}

void _do_upward_pass (VsgPRTree3d *tree)
{
  /* zero counts  */
  vsg_prtree3d_traverse (tree, G_POST_ORDER, (VsgPRTree3dFunc) _zero, NULL);

  /* accumulate from leaves to top */
  vsg_prtree3d_traverse (tree, G_POST_ORDER, (VsgPRTree3dFunc) _up, NULL);

  /* gather shared in_counts */
  vsg_prtree3d_shared_nodes_allreduce (tree, &pconfig.node_data.visit_forward);
}

void _down (VsgPRTree3dNodeInfo *node_info, gpointer data)
{
  glong count;

  if (VSG_PRTREE3D_NODE_INFO_IS_REMOTE (node_info)) return;

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
      vsg_vector3d_write (&pt->vector, stderr);
      g_printerr (" weight=%d count=%ld (should be %ld)\n", pt->weight,
                  pt->count, *ref);
    }
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  VsgPRTree3d *tree;

  VsgVector3d lb;
  VsgVector3d ub;
  GTimer *timer = NULL;

  MPI_Init (&argc, &argv);

  MPI_Comm_size (MPI_COMM_WORLD, &sz);
  MPI_Comm_rank (MPI_COMM_WORLD, &rk);

  vsg_init_gdouble ();

  parse_args (argc, argv);

  if (nc_padding > 0)
    {
      if (_verbose && rk == 0)
        g_printerr ("%d: NodeCounter padding: %d\n", rk, nc_padding);
      _nc_padding_buffer = g_malloc (nc_padding * sizeof (char));
    }

  points = g_ptr_array_new ();

  lb.x = -1.; lb.y = -1.; lb.z = -1.;
  ub.x = 1.; ub.y = 1.; ub.z = 1.;

  /* create the tree */
  tree =
    vsg_prtree3d_new_full (&lb, &ub,
                           (VsgPoint3dLocFunc) vsg_vector3d_vector3d_locfunc,
                           (VsgPoint3dDistFunc) vsg_vector3d_dist,
                           NULL, _maxbox);

  if (_hilbert)
    {
      /* configure for hilbert curve order traversal */
      vsg_prtree3d_set_children_order_hilbert (tree);
    }

  if (_verbose)
    {
      MPI_Barrier (MPI_COMM_WORLD);
      g_printerr ("%d: set_parallel begin\n", rk);
    }

  vsg_prtree3d_set_parallel (tree, &pconfig);

  if (_verbose)
    {
      MPI_Barrier (MPI_COMM_WORLD);
      g_printerr ("%d: set_parallel ok\n", rk);
    }

  if (_verbose)
    {
      MPI_Barrier (MPI_COMM_WORLD);
      g_printerr ("%d: fill begin\n", rk);
    }

  _fill (tree, _np);

  if (_verbose)
    {
      MPI_Barrier (MPI_COMM_WORLD);
      g_printerr ("%d: fill ok\n", rk);
    }

/*   if (_do_write) */
/*     { */
/*       gchar fn[1024]; */
/*       FILE *f; */

/*       sprintf (fn, "comm-%03d.svg", rk); */
/*       f = fopen (fn, "w"); */

/*       fprintf (f, "\n<g style=\"stroke-width:0.01; stroke:black; " \ */
/*                "fill:none\">\n"); */
/*       fclose (f); */
/*     } */

  if (_verbose)
    {
      g_printerr ("%d: near/far traversal begin\n", rk);
          MPI_Barrier (MPI_COMM_WORLD);
      timer = g_timer_new ();
    }

  /* accumulate the point counts across the tree */
  _do_upward_pass (tree);

  /* do some near/far traversal */
  vsg_prtree3d_near_far_traversal (tree, (VsgPRTree3dFarInteractionFunc) _far,
                                   (VsgPRTree3dInteractionFunc) _near,
                                   &ret);
  /* accumulate from top to leaves */
  vsg_prtree3d_traverse (tree, G_PRE_ORDER, (VsgPRTree3dFunc) _down, NULL);

  if (_verbose)
    {
      MPI_Barrier (MPI_COMM_WORLD);

      g_printerr ("%d: near/far traversal ok elapsed=%f seconds\n", rk,
                  g_timer_elapsed (timer, NULL));

      g_timer_destroy (timer);
    }

  if (_do_write)
    {
      gchar fn[1024];
      FILE *f;

      MPI_Barrier (MPI_COMM_WORLD);

      g_sprintf (fn, "prtree3parallel-%03d.txt", rk);
      f = fopen (fn, "w");
      vsg_prtree3d_write (tree, f);
      fclose (f);

      _tree_write (tree, "prtree3parallel-");
    }

  if (_do_write)
    {
      gchar fn[1024];
      FILE *f;

      sprintf (fn, "comm-%03d.svg", rk);
      f = fopen (fn, "a");
      fprintf (f, "</g>\n");
      fclose (f);
    }

  if (_verbose)
    {
      gint near_count_sum, far_count_sum;
      MPI_Barrier (MPI_COMM_WORLD);
      g_printerr ("%d: processor msg stats fw=%d bw=%d\n",
                  rk, _fw_count, _bw_count);
      g_printerr ("%d: processor call stats near=%d far=%d\n",
                  rk, _near_count, _far_count);

      MPI_Reduce (&_near_count, &near_count_sum, 1, MPI_INT, MPI_SUM, 0,
                  MPI_COMM_WORLD);
      MPI_Reduce (&_far_count, &far_count_sum, 1, MPI_INT, MPI_SUM, 0,
                  MPI_COMM_WORLD);
      if (rk == 0)
        {
          g_printerr ("%d: mean call stats near=%f far=%f\n",
                      rk, (1.*near_count_sum)/sz, (1.*far_count_sum)/sz);
        }
    }

  /* check correctness of results */
  g_ptr_array_foreach (points, (GFunc) _check_pt_count,
                       &_ref_count);

  /* destroy the points */
  g_ptr_array_foreach (points, empty_array, NULL);
  g_ptr_array_free (points, TRUE);

  /* destroy the tree */
  vsg_prtree3d_free (tree);

  if (nc_padding > 0) g_free (_nc_padding_buffer);

  MPI_Finalize ();

  return ret;
}
