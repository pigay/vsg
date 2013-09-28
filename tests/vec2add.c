#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

static gdouble epsilon = 1.e-8;

static gint vcheck (const gchar *label, VsgVector2d *res, VsgVector2d *ref)
{
  VsgVector2d tmp;
  gdouble r, refr;

  tmp.x = res->x - ref->x;
  tmp.y = res->y - ref->y;

  r = sqrt (tmp.x*tmp.x + tmp.y*tmp.y);
  refr = sqrt (ref->x*ref->x + ref->y*ref->y);

  if (refr > epsilon) r /= refr;

  if (r > epsilon)
    {
      g_printerr ("Error \"%s\" ref(%f, %f) != res(%f, %f)\n",
                  label,
                  ref->x, ref->y,
                  res->x, res->y);

      return 1;
    }

  return 0;
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;
  VsgVector2d v, w, res, ref;

  if (argc > 1 && g_ascii_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  vsg_vector2d_set (&v, 1, 0);
  vsg_vector2d_set (&w, 0, 1);
  vsg_vector2d_add (&v, &w, &res);
  vsg_vector2d_set (&ref, 1, 1);
  ret += vcheck ("[1,0] + [0,1]) == [1,1]", &res, &ref);

  vsg_vector2d_set (&v, 1, 2);
  vsg_vector2d_set (&w, 0.1, 0.2);
  vsg_vector2d_add (&v, &w, &res);
  vsg_vector2d_set (&ref, 1.1, 2.2);
  ret += vcheck ("[1, 2] + [0.1, 0.2]) == [1.1, 2.2]", &res, &ref);

  vsg_vector2d_set (&v, 1.2e-2, 0.);
  vsg_vector2d_set (&w, 1.2e-3, 0);
  vsg_vector2d_add (&v, &w, &res);
  vsg_vector2d_set (&ref, 0.0132, 0);
  ret += vcheck ("[1.2e-2, 0.] + [1.2e-3, 0] == [0.0132, 0]", &res, &ref);

  vsg_vector2d_set (&v, 0, 0);
  vsg_vector2d_set (&w, 0, 0);
  vsg_vector2d_add (&v, &w, &res);
  vsg_vector2d_set (&ref, 0, 0);
  ret += vcheck ("[0, 0] + [0, 0]) == [0, 0]", &res, &ref);

  return ret;
}
