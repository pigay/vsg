/* LIBVSG - Visaurin Geometric Library
 * Copyright (C) 2006-2007 Pierre Gay
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "vsgd.h"

#include <math.h>
#include <stdio.h>

#include <glib-object.h>

static void foreach_vector2d_print (VsgVector2d *vector,
                                    gpointer user_data)
{
  vsg_vector2d_write (vector, stderr); g_printerr ("\n");
}

typedef struct _Circle Circle;
struct _Circle {
  VsgVector2d center;
  gdouble radius;
};

static vsgrloc2 circle_loc2 (const Circle *circle,
                             const VsgVector2d *center)
{
  VsgVector2d tmp;
  gdouble dist;
  vsgrloc2 result = 0;
  vsgloc2 center_pos = 0;

  vsg_vector2d_sub (center, &circle->center, &tmp);

  dist = vsg_vector2d_norm (&tmp);

  if (dist <= circle->radius) return VSG_RLOC2_MASK;

  center_pos = vsg_vector2d_vector2d_locfunc (&circle->center, center);

  result |= VSG_RLOC2_COMP (center_pos);

  if (fabs(tmp.x) <= circle->radius)
    {
      vsgloc2 itmp = (center_pos & ~ VSG_LOC2_X) | (~ center_pos & VSG_LOC2_X);
      result |= VSG_RLOC2_COMP (itmp);
    }
  if (fabs(tmp.y) <= circle->radius)
    {
      vsgloc2 itmp = (center_pos & ~ VSG_LOC2_Y) | (~ center_pos & VSG_LOC2_Y);
      result |= VSG_RLOC2_COMP (itmp);
    }
  return result;
}

static void foreach_circle_print (Circle *circle,
                                  gpointer user_data)
{
  if (circle)
    {
      vsg_vector2d_write (&(circle->center), stderr);
      g_printerr (" r=%lf\n", circle->radius);
    }
  else
    g_printerr ("None\n");
}

static gint user_count = 0;

static gint *user_copy (gint *src)
{
  gint *ret;

  if (src == NULL) return NULL;

  ret = g_malloc (sizeof (gint));

  *ret = *src;

  g_printerr ("user_copy (%d)\n", ++ user_count);

  return ret;
}

static void user_free (gint *boxed)
{
   if (boxed == NULL) return;

   g_free (boxed);

   g_printerr ("user_free (%d)\n", -- user_count);
}

static void near_interaction (VsgPRTree2dNodeInfo *one_info,
                              VsgPRTree2dNodeInfo *other_info,
			      gpointer user_data)
{
  g_printerr ("near interaction {");
  vsg_vector2d_write (&one_info->lbound, stderr);
  g_printerr (" ");
  vsg_vector2d_write (&one_info->center, stderr);
  g_printerr (" ");
  vsg_vector2d_write (&one_info->ubound, stderr);
  g_printerr ("}<->{");
  vsg_vector2d_write (&other_info->lbound, stderr);
  g_printerr (" ");
  vsg_vector2d_write (&other_info->center, stderr);
  g_printerr (" ");
  vsg_vector2d_write (&other_info->ubound, stderr);
  g_printerr ("}\n");
}

static void far_interaction (VsgPRTree2dNodeInfo *one_info,
			     VsgPRTree2dNodeInfo *other_info,
			     gpointer user_data)
{
  g_printerr ("far interaction {");
  vsg_vector2d_write (&one_info->lbound, stderr);
  g_printerr (" ");
  vsg_vector2d_write (&one_info->center, stderr);
  g_printerr (" ");
  vsg_vector2d_write (&one_info->ubound, stderr);
  g_printerr ("}<->{");
  vsg_vector2d_write (&other_info->lbound, stderr);
  g_printerr (" ");
  vsg_vector2d_write (&other_info->center, stderr);
  g_printerr (" ");
  vsg_vector2d_write (&other_info->ubound, stderr);
  g_printerr ("}\n");
}

static void traversal (const VsgPRTree2dNodeInfo *node_info,
		       gpointer user_data)
{
  g_printerr ("traversal {");
  vsg_vector2d_write (&node_info->lbound, stderr);
  g_printerr (" ");
  vsg_vector2d_write (&node_info->center, stderr);
  g_printerr (" ");
  vsg_vector2d_write (&node_info->ubound, stderr);
  g_printerr ("}\n");
}

int main ()
{
  int i;
  VsgPRTree2d *prtree2d;
  VsgVector2d lbound = {-1., -1.};
  VsgVector2d ubound = {1., 1.};
  VsgVector2d deep_search = {-0.5,0.5};
  VsgVector2d pts[] = {
    {.5, .5},
    {.5, .75},
    {-.5,-.5},
    {-.75,-.5},
  };

  Circle circle = {{-0.45, -0.45}, .15};
  Circle c1 = {{-.5,.5},.25};

  vsg_init_gdouble ();

  prtree2d =
    vsg_prtree2d_new_full (&lbound, &ubound,
                           (VsgPoint2dLocFunc) vsg_vector2d_vector2d_locfunc,
                           (VsgPoint2dDistFunc) vsg_vector2d_dist,
                           (VsgRegion2dLocFunc) circle_loc2, 1);

  for (i=0; i<4; i++)
    {
      vsg_prtree2d_insert_point(prtree2d, &pts[i]);
    }

  g_printerr ("after insert\n");
  vsg_prtree2d_write (prtree2d, stderr);
  g_printerr ("prtree2d depth=%u\n", vsg_prtree2d_depth (prtree2d));

  g_printerr ("checking find\n");
  for (i=0; i<4; i++)
    {
      if (vsg_prtree2d_find_point (prtree2d, &pts[i]) == NULL)
        g_printerr ("error finding point #%d\n", i);
    }
  if (vsg_prtree2d_find_point (prtree2d, &lbound) != NULL)
    g_printerr ("error finding point lbound\n");

  vsg_prtree2d_remove_point (prtree2d, &pts[0]);

  g_printerr ("after remove 1\n");
  vsg_prtree2d_write (prtree2d, stderr);
  g_printerr ("prtree2d depth=%u\n", vsg_prtree2d_depth (prtree2d));

  g_printerr ("foreach point\n");
  vsg_prtree2d_foreach_point (prtree2d,
                                (GFunc) foreach_vector2d_print,
                                NULL);

  g_printerr ("foreach custom\n");
  vsg_prtree2d_foreach_point_custom (prtree2d,
                                       &circle,
                                       (VsgRegion2Point2LocFunc) circle_loc2,
                                       (GFunc) foreach_vector2d_print,
                                       NULL);

  g_printerr ("insert region\n");
  vsg_prtree2d_insert_point(prtree2d, &c1.center);
  vsg_prtree2d_insert_region(prtree2d, &c1);
  vsg_prtree2d_write (prtree2d, stderr);

  g_printerr ("foreach region\n");
  vsg_prtree2d_foreach_region (prtree2d, (GFunc) foreach_circle_print,
                              NULL);

  g_printerr ("deep region find\n");
  foreach_circle_print (vsg_prtree2d_find_deep_region (prtree2d,
                                                      &deep_search,
                                                      NULL, NULL),
                        NULL);

  g_printerr ("remove region\n");
  vsg_prtree2d_remove_region(prtree2d, &c1);
  vsg_prtree2d_remove_point(prtree2d, &c1.center);
  vsg_prtree2d_write (prtree2d, stderr);

  g_printerr ("manipulating user data types\n");

  {
    gint i = 8;
    GType user_data_type =
      g_boxed_type_register_static ("UserData",
				    (GBoxedCopyFunc) user_copy,
				    (GBoxedFreeFunc) user_free);

    vsg_prtree2d_set_node_data (prtree2d, user_data_type, &i);

    vsg_prtree2d_insert_point (prtree2d, &pts[0]);

    g_printerr ("near/far interaction traversal\n");

    vsg_prtree2d_near_far_traversal (prtree2d,
                                      far_interaction, near_interaction,
                                      NULL);

    vsg_prtree2d_write (prtree2d, stderr);
    vsg_prtree2d_traverse (prtree2d, G_PRE_ORDER, traversal, NULL);
  }

  g_printerr ("copying\n");
  {
    VsgPRTree2d *copy = vsg_prtree2d_clone (prtree2d);
    vsg_prtree2d_free (copy);
  }

  g_printerr ("deleting\n");
  vsg_prtree2d_free (prtree2d);

  return 0;
}
