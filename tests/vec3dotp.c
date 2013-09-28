#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

static gdouble epsilon = 1.e-8;

static gint dcheck (const gchar *label, gdouble res, gdouble ref)
{
  gdouble r;

  r = res - ref;

  if (ref > 0.) r /= ref;

  if (fabs (r) > epsilon && fabs (ref) > epsilon)
    {
      g_printerr ("Error \"%s\" ref(%f) != res(%f)\n",
                  label,
                  ref, r);

      return 1;
    }

  return 0;
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;
  VsgVector3d v, w;
  gdouble res, ref;

  if (argc > 1 && g_ascii_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  vsg_vector3d_set (&v, 1, 0, 1);
  vsg_vector3d_set (&w, 0, 1, 0);
  res = vsg_vector3d_dotp (&v, &w);
  ref = 0;
  ret += dcheck ("[1, 0, 1] . [0, 1, 0] == 0", res, ref);

  vsg_vector3d_set (&v, 1, 2, 3);
  vsg_vector3d_set (&w, 0.1, 0.2, 0.3);
  res = vsg_vector3d_dotp (&v, &w);
  ref = 1.4;
  ret += dcheck ("[1, 2, 3] . [0.1, 0.2, 0.3] == 1.4", res, ref);

  vsg_vector3d_set (&v, 1.2e-2, 0, 0);
  vsg_vector3d_set (&w, 1.2e-3, 0, 0);
  res = vsg_vector3d_dotp (&v, &w);
  ref = 1.44e-05;
  ret += dcheck ("[1.2e-2, 0, 0] . [1.2e-3, 0, 0] == 1.44e-05", res, ref);

  vsg_vector3d_set (&v, 0, 0, 0);
  vsg_vector3d_set (&w, 0, 0, 0);
  res = vsg_vector3d_dotp (&v, &w);
  ref = 0;
  ret += dcheck ("[0, 0, 0] . [0, 0, 0] == 0", res, ref);

  return ret;
}
