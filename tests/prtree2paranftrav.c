/* basic point insertion, removal and traversal on a cloned VsgPRTree2d */

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
  vsg_packed_msg_send_append (pm, &pt->vector, 1, VSG_MPI_TYPE_VECTOR2D);
  vsg_packed_msg_send_append (pm, &pt->weight, 1, MPI_INT);
  vsg_packed_msg_send_append (pm, &pt->count, 1, MPI_LONG);
}

void pt_migrate_unpack (Pt *pt, VsgPackedMsg *pm,
                        gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &pt->vector, 1, VSG_MPI_TYPE_VECTOR2D);
  vsg_packed_msg_recv_read (pm, &pt->weight, 1, MPI_INT);
  vsg_packed_msg_recv_read (pm, &pt->count, 1, MPI_LONG);
}

/* visit forward pack/unpack functions */
/* only the coordinates part is transmitted */
void pt_visit_fw_pack (Pt *pt, VsgPackedMsg *pm,
                       gpointer user_data)
{
  vsg_packed_msg_send_append (pm, &pt->vector, 1, VSG_MPI_TYPE_VECTOR2D);
  vsg_packed_msg_send_append (pm, &pt->weight, 1, MPI_INT);
}

void pt_visit_fw_unpack (Pt *pt, VsgPackedMsg *pm,
                         gpointer user_data)
{
  vsg_packed_msg_recv_read (pm, &pt->vector, 1, VSG_MPI_TYPE_VECTOR2D);
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


void _pt_write (Pt *pt, FILE *file)
{
  gchar *color = "#00FF00";

  if (pt->count != _ref_count) color = "#FF0000";
  else return;

  fprintf (file, "<circle cx=\"%g\" cy=\"%g\" r=\"%g\" " \
           "style=\"stroke-width:0.0001;stroke:#000000;fill:%s;\">\n",
           pt->vector.x, -pt->vector.y, 0.005, color);
  fprintf (file, "<title>w=%d</title>\n", pt->weight);
  fprintf (file, "<desc>count=%ld</desc>\n", pt->count);
  fprintf (file, "</circle>\n");
}

void _traverse_bg_write (VsgPRTree2dNodeInfo *node_info, FILE *file)
{
  /* ugly colormap */
  static const gchar *colors[] = {
    "#800000",
    "#FF0000",
    "#808000",
    "#008000",
    "#00FF00",
    "#008080",
    "#00FFFF",
    "#000080",
    "#0000FF",
    "#800080",
  };

  gdouble x = node_info->lbound.x;
  gdouble y = -node_info->ubound.y;
  gdouble w = node_info->ubound.x - x;
  gdouble h = -node_info->lbound.y - y;
  const gchar *fill = "#FFFFFF";

  if (! VSG_PRTREE2D_NODE_INFO_IS_REMOTE (node_info))
    {
      fprintf (file, "<!-- %d: node ", rk);
      vsg_prtree_key2d_write (&node_info->id, file);
      fprintf (file, " in=%ld out=%ld -->\n",
                  ((NodeCounter *) node_info->user_data)->in_count,
                  ((NodeCounter *) node_info->user_data)->out_count);
    }

  if (!node_info->isleaf) return;

  if (VSG_PRTREE2D_NODE_INFO_IS_REMOTE (node_info))
    {
      gint proc = VSG_PRTREE2D_NODE_INFO_PROC (node_info) %
        (sizeof (colors) / sizeof (gchar *));
      fill = colors[proc];
    }

  fprintf (file, "<rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" " \
           "rx=\"0\" style=\"stroke-width:0.001;stroke:#000000; " \
           "stroke-linejoin:miter; stroke-linecap:butt; fill:%s;\">\n",
           x, y, w, h, fill);
  fprintf (file, "<title>");
  vsg_prtree_key2d_write (&node_info->id, file);
  fprintf (file, "</title>\n");

  if (! VSG_PRTREE2D_NODE_INFO_IS_REMOTE (node_info))
    {
      fprintf (file, "<desc>in=%ld out=%ld</desc>\n",
               ((NodeCounter *) node_info->user_data)->in_count,
               ((NodeCounter *) node_info->user_data)->out_count);
    }
  else
    {
      fprintf (file, "<desc>proc=%d</desc>\n",
               VSG_PRTREE2D_NODE_INFO_PROC (node_info));
    }

  fprintf (file, "</rect>\n");
}

void _traverse_fg_write (VsgPRTree2dNodeInfo *node_info, FILE *file)
{
  fprintf (file, "<!-- %d: node ", rk);
  vsg_prtree_key2d_write (&node_info->id, file);
  fprintf (file, " -->\n");
  g_slist_foreach (node_info->point_list, (GFunc) _pt_write, file);
}

static const gchar *_svg_header = 
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" \
"<!DOCTYPE svg PUBLIC \"-_W3C_DTD SVG 1.0_EN\" \"http://www.w3.org/TR/SVG/DTD/svg10.dtd\" >\n" \
"<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n" \
"   onload=\"Init(evt)\">\n" \
"\n" \
"   <title>prtree2paranftrav %03d</title>\n" \
"\n" \
"   <desc>\n" \
"      SVG tooltip  example inspired by http://svg-whiz.com/samples.html\n" \
"   </desc>\n" \
"\n" \
"   <script type=\"text/ecmascript\"><![CDATA[\n" \
"      var SVGDocument = null;\n" \
"      var SVGRoot = null;\n" \
"      var SVGViewBox = null;\n" \
"      var svgns = \'http://www.w3.org/2000/svg\';\n" \
"      var xlinkns = \'http://www.w3.org/1999/xlink\';\n" \
"      var toolTip = null;\n" \
"      var TrueCoords = null;\n" \
"      var tipBox = null;\n" \
"      var tipText = null;\n" \
"      var tipTitle = null;\n" \
"      var tipDesc = null;\n" \
"\n" \
"      var lastElement = null;\n" \
"      var titleText = \'\';\n" \
"      var titleDesc = \'\';\n" \
"\n" \
"\n" \
"      function Init(evt)\n" \
"      {\n" \
"         SVGDocument = evt.target.ownerDocument;\n" \
"         SVGRoot = SVGDocument.documentElement;\n" \
"         TrueCoords = SVGRoot.createSVGPoint();\n" \
"\n" \
"         toolTip = SVGDocument.getElementById(\'ToolTip\');\n" \
"         tipBox = SVGDocument.getElementById(\'tipbox\');\n" \
"         tipText = SVGDocument.getElementById(\'tipText\');\n" \
"         tipTitle = SVGDocument.getElementById(\'tipTitle\');\n" \
"         tipDesc = SVGDocument.getElementById(\'tipDesc\');\n" \
"         //window.status = (TrueCoords);\n" \
"\n" \
"         //create event for object\n" \
"         SVGRoot.addEventListener(\'mousemove\', ShowTooltip, false);\n" \
"         SVGRoot.addEventListener(\'mouseout\', HideTooltip, false);\n" \
"      };\n" \
"\n" \
"\n" \
"      function GetTrueCoords(evt)\n" \
"      {\n" \
"         // find the current zoom level and pan setting, and adjust the reported\n" \
"         //    mouse position accordingly\n" \
"         var newScale = SVGRoot.currentScale;\n" \
"         var translation = SVGRoot.currentTranslate;\n" \
"         TrueCoords.x = (evt.clientX - translation.x)/newScale;\n" \
"         TrueCoords.y = (evt.clientY - translation.y)/newScale;\n" \
"      };\n" \
"\n" \
"\n" \
"      function HideTooltip( evt )\n" \
"      {\n" \
"         toolTip.setAttributeNS(null, \'visibility\', \'hidden\');\n" \
"      };\n" \
"\n" \
"\n" \
"      function ShowTooltip( evt )\n" \
"      {\n" \
"         GetTrueCoords( evt );\n" \
"\n" \
"         var tipScale = 1/SVGRoot.currentScale;\n" \
"         var textWidth = 0;\n" \
"         var tspanWidth = 0;\n" \
"         var boxHeight = 20;\n" \
"\n" \
"         tipBox.setAttributeNS(null, \'transform\', \'scale(\' + tipScale + \',\' + tipScale + \')\' );\n" \
"         tipText.setAttributeNS(null, \'transform\', \'scale(\' + tipScale + \',\' + tipScale + \')\' );\n" \
"\n" \
"         var titleValue = \'\';\n" \
"         var descValue = \'\';\n" \
"         var targetElement = evt.target;\n" \
"         if ( lastElement != targetElement )\n" \
"         {\n" \
"            var targetTitle = targetElement.getElementsByTagName(\'title\').item(0);\n" \
"            if ( targetTitle )\n" \
"            {\n" \
"               // if there is a \'title\' element, use its contents for the tooltip title\n" \
"               titleValue = targetTitle.firstChild.nodeValue;\n" \
"            }\n" \
"\n" \
"            var targetDesc = targetElement.getElementsByTagName(\'desc\').item(0);\n" \
"            if ( targetDesc )\n" \
"            {\n" \
"               // if there is a \'desc\' element, use its contents for the tooltip desc\n" \
"               descValue = targetDesc.firstChild.nodeValue;\n" \
"\n" \
"               if ( \'\' == titleValue )\n" \
"               {\n" \
"                  // if there is no \'title\' element, use the contents of the \'desc\' element for the tooltip title instead\n" \
"                  titleValue = descValue;\n" \
"                  descValue = \'\';\n" \
"               }\n" \
"            }\n" \
"\n" \
"            // if there is still no \'title\' element, use the contents of the \'id\' attribute for the tooltip title\n" \
"            if ( \'\' == titleValue )\n" \
"            {\n" \
"               titleValue = targetElement.getAttributeNS(null, \'id\');\n" \
"            }\n" \
"\n" \
"            // selectively assign the tooltip title and desc the proper values,\n" \
"            //   and hide those which don\'t have text values\n" \
"            //\n" \
"            var titleDisplay = \'none\';\n" \
"            if ( \'\' != titleValue )\n" \
"            {\n" \
"               tipTitle.firstChild.nodeValue = titleValue;\n" \
"               titleDisplay = \'inline\';\n" \
"            }\n" \
"            tipTitle.setAttributeNS(null, \'display\', titleDisplay );\n" \
"\n" \
"\n" \
"            var descDisplay = \'none\';\n" \
"            if ( \'\' != descValue )\n" \
"            {\n" \
"               tipDesc.firstChild.nodeValue = descValue;\n" \
"               descDisplay = \'inline\';\n" \
"            }\n" \
"            tipDesc.setAttributeNS(null, \'display\', descDisplay );\n" \
"         }\n" \
"\n" \
"         // if there are tooltip contents to be displayed, adjust the size and position of the box\n" \
"         if ( \'\' != titleValue )\n" \
"         {\n" \
"            var xPos = TrueCoords.x + (10 * tipScale);\n" \
"            var yPos = TrueCoords.y + (10 * tipScale);\n" \
"\n" \
"            //return rectangle around text as SVGRect object\n" \
"            var outline = tipText.getBBox();\n" \
"            tipBox.setAttributeNS(null, \'width\', Number(outline.width) + 10);\n" \
"            tipBox.setAttributeNS(null, \'height\', Number(outline.height) + 10);\n" \
"\n" \
"            // update position\n" \
"            toolTip.setAttributeNS(null, \'transform\', \'translate(\' + xPos + \',\' + yPos + \')\');\n" \
"            toolTip.setAttributeNS(null, \'visibility\', \'visible\');\n" \
"         }\n" \
"      };\n" \
"\n" \
"   ]]></script>\n";

void _tree_write (VsgPRTree2d *tree, gchar *prefix)
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

  g_sprintf (fn, "%s%03d.svg", prefix, rk);
  f = fopen (fn, "w");

  fprintf (f, _svg_header, rk);

  fprintf (f, "<rect x=\"0\" y=\"0\" width=\"100%%\" height=\"100%%\" fill=\"white\"/>\n");
  fprintf (f, "<svg x=\"0\" y=\"0\" width=\"500px\" height=\"500px\" " \
           "viewBox=\"%g %g %g %g\">\n", x, y, w, h);
  vsg_prtree2d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree2dFunc) _traverse_bg_write,
                         f);

  vsg_prtree2d_traverse (tree, G_PRE_ORDER,
                         (VsgPRTree2dFunc) _traverse_fg_write,
                         f);
  fprintf (f, "<xi:include xmlns:xi=\"http://www.w3.org/2001/XInclude\" " \
           "href=\"comm-%03d.svg\" xpointer=\"/1\" " \
           "parse=\"xml\" />\n", rk);
  fprintf (f, "</svg>\n");

  fprintf (f, "<g id=\"ToolTip\" opacity=\"0.8\" visibility=\"hidden\" pointer-events=\"none\">\n" \
           "<rect id=\"tipbox\" x=\"0\" y=\"5\" width=\"88\" height=\"20\" rx=\"2\" ry=\"2\" fill=\"white\" stroke=\"black\"/>\n" \
           "<text id=\"tipText\" x=\"5\" y=\"20\" font-family=\"Arial\" font-size=\"12\">\n" \
           "<tspan id=\"tipTitle\" x=\"5\" font-weight=\"bold\"><![CDATA[]]></tspan>\n" \
           "<tspan id=\"tipDesc\" x=\"5\" dy=\"15\" fill=\"blue\"><![CDATA[]]></tspan>\n" \
           "</text>\n</g>\n</svg>\n");

  fclose (f);

}



static void (*_distribute) (VsgPRTree2d *tree) =
  vsg_prtree2d_distribute_contiguous_leaves;

void _random_fill (VsgPRTree2d *tree, guint np);

static void (*_fill) (VsgPRTree2d *tree, guint np) = _random_fill;

void _random_fill (VsgPRTree2d *tree, guint np)
{
  gint i;
  Pt *pt;
  VsgVector2d lb, ub;
  GRand *rand = g_rand_new_with_seed (_random_seed);

  vsg_prtree2d_get_bounds (tree, &lb, &ub);

  _ref_count = 0;

  for (i=0; i< np; i++)
    {
      gdouble x1, y1;
      gint c;

      x1 = g_rand_double_range (rand, lb.x, ub.x);
      y1 = g_rand_double_range (rand, lb.y, ub.y);

      c = i+1;

      _ref_count += c;

      if (i%_flush_interval == 0)
        {
          vsg_prtree2d_migrate_flush (tree);
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
      pt->weight = c;

      vsg_prtree2d_insert_point (tree, pt);
    }

  vsg_prtree2d_migrate_flush (tree);

  _distribute (tree);

  g_rand_free (rand);
}

/* fadster algorithm for random distribution */
static void _random2_fill (VsgPRTree2d *tree, guint np)
{
  gint i;
  Pt *pt;
  VsgVector2d lb, ub;
  GRand *rand = g_rand_new_with_seed (_random_seed);

  vsg_prtree2d_get_bounds (tree, &lb, &ub);

  pt = pt_alloc (TRUE, NULL);

  for (i=0; i< np; i++)
    {
      gdouble x1, y1;
      gint c;

      x1 = g_rand_double_range (rand, lb.x, ub.x);
      y1 = g_rand_double_range (rand, lb.y, ub.y);
      c = i+1;

      pt->vector.x = x1;
      pt->vector.y = y1;
      pt->weight = c;

      _ref_count += c;

      if (vsg_prtree2d_insert_point_local (tree, pt))
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
void _circle_fill (VsgPRTree2d *tree, guint np)
{
  gint i;
  Pt *pt;
  const gdouble r = 0.95;
  gdouble dtheta = 2. * G_PI / (np-1) / sz;
  gdouble theta0 = 2. * rk * G_PI / sz;

  for (i=0; i<np; i++)
    {
      gint c;

      c = i+1;

      _ref_count += c;

      if (i%_flush_interval == 0)
        {
          vsg_prtree2d_migrate_flush (tree);
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
      pt->vector.y = r * sin (theta0 + i * dtheta);
      pt->weight = c;

      vsg_prtree2d_insert_point (tree, pt);
    }

  vsg_prtree2d_migrate_flush (tree);
  _distribute (tree);
}

void _circle2_fill (VsgPRTree2d *tree, guint np)
{
  gint i;
  Pt *pt;
  const gdouble r = 0.95;
  gdouble dtheta = 2. * G_PI / (np-1);
  gdouble theta0 = 0.;

  pt = pt_alloc (TRUE, NULL);

  for (i=0; i<np; i++)
    {
      gint c;

      c = i+1;

      _ref_count += c;

      pt->vector.x = r * cos (theta0 + i * dtheta);
      pt->vector.y = r * sin (theta0 + i * dtheta);
      pt->weight = c;

      if (vsg_prtree2d_insert_point_local (tree, pt))
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

	  if (g_ascii_strncasecmp (arg, "random2", 7) == 0)
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
              _distribute = vsg_prtree2d_distribute_contiguous_leaves;
            }
          else if (g_ascii_strncasecmp (arg, "scatter", 7) == 0)
            {
              _distribute = vsg_prtree2d_distribute_scatter_leaves;
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

  if (_do_write && VSG_PRTREE2D_NODE_INFO_IS_REMOTE (one_info))
    {
      gchar fn[1024];
      FILE *f;
      sprintf (fn, "comm-%03d.svg", rk);
      f = fopen (fn, "a");

      fprintf (f, "<!--s=%d--><polyline points=\"%g,%g %g,%g\" "        \
               "style=\"stroke:#00FF00;\"/>\n",
               rk,
               one_info->center.x, -one_info->center.y,
               other_info->center.x, -other_info->center.y);
      fclose (f);
    }

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

  if (_do_write && VSG_PRTREE2D_NODE_INFO_IS_REMOTE (one_info))
    {
      gchar fn[1024];
      FILE *f;
      sprintf (fn, "comm-%03d.svg", rk);
      f = fopen (fn, "a");

      fprintf (f, "<!--s=%d--><polyline points=\"%g,%g %g,%g\" "        \
               "style=\"stroke:#0000FF;\"/>\n",
               rk,
               one_info->center.x, -one_info->center.y,
               other_info->center.x, -other_info->center.y);
      fclose (f);
    }

  _far_count ++;

  {
    long i, j = 0;
    for (i = 0; i< _far_slowdown; i++)
      {
        j = j + i;
      }
  }
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

  /* gather shared in_counts */
  vsg_prtree2d_shared_nodes_allreduce (tree, &pconfig.node_data.visit_forward);
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

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  VsgPRTree2d *tree;

  VsgVector2d lb;
  VsgVector2d ub;
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

  lb.x = -1.; lb.y = -1.;
  ub.x = 1.; ub.y = 1.;

  /* create the tree */
  tree =
    vsg_prtree2d_new_full (&lb, &ub,
                           (VsgPoint2dLocFunc) vsg_vector2d_vector2d_locfunc,
                           (VsgPoint2dDistFunc) vsg_vector2d_dist,
                           NULL, _maxbox);

  if (_hilbert)
    {
      /* configure for hilbert curve order traversal */
      vsg_prtree2d_set_children_order_hilbert (tree);
    }

  if (_verbose)
    {
      MPI_Barrier (MPI_COMM_WORLD);
      g_printerr ("%d: set_parallel begin\n", rk);
    }

  vsg_prtree2d_set_parallel (tree, &pconfig);

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

/*   /\* try one exterior point *\/ */
/*   if (sz > 1) */
/*     { */
/*       if (rk == 1) */
/*         { */
/*           Pt *pt; */

/*           _ref_count += _np+1; */

/*           pt = pt_alloc (TRUE, NULL); */
/*           pt->vector.x = ub.x+1.; */
/*           pt->vector.y = ub.y+1.; */
/*           pt->weight = _np+1; */

/*           vsg_prtree2d_insert_point (tree, pt); */
/*         } */
/*       else */
/*         { */
/*           _ref_count += _np+1; */
/*         } */

/*       vsg_prtree2d_migrate_flush (tree); */
/*       _distribute (tree); */
/*     } */


  if (_verbose)
    {
      MPI_Barrier (MPI_COMM_WORLD);
      g_printerr ("%d: fill ok\n", rk);
    }

  if (_do_write)
    {
      gchar fn[1024];
      FILE *f;

      sprintf (fn, "comm-%03d.svg", rk);
      f = fopen (fn, "w");

      fprintf (f, "\n<g style=\"stroke-width:0.01; stroke:black; " \
               "fill:none\">\n");
      fclose (f);
    }

  if (_verbose)
    {
      g_printerr ("%d: near/far traversal begin\n", rk);
          MPI_Barrier (MPI_COMM_WORLD);
      timer = g_timer_new ();
    }

  /* accumulate the point counts across the tree */
  _do_upward_pass (tree);

  /* do some near/far traversal */
  vsg_prtree2d_near_far_traversal (tree, (VsgPRTree2dFarInteractionFunc) _far,
                                   (VsgPRTree2dInteractionFunc) _near,
                                   &ret);
  /* accumulate from top to leaves */
  vsg_prtree2d_traverse (tree, G_PRE_ORDER, (VsgPRTree2dFunc) _down, NULL);

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

      g_sprintf (fn, "prtree2parallel-%03d.txt", rk);
      f = fopen (fn, "w");
      vsg_prtree2d_write (tree, f);
      fclose (f);

      _tree_write (tree, "prtree2parallel-");
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
  vsg_prtree2d_free (tree);

  if (nc_padding > 0) g_free (_nc_padding_buffer);

  MPI_Finalize ();

  return ret;
}
