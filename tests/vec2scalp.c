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

  if (refr > 0.) r /= refr;

  if (r > epsilon && refr > epsilon)
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
  VsgVector2d v, res, ref;
  gdouble w;

  if (argc > 1 && g_ascii_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  vsg_vector2d_set (&v, 0, 1);
  w = 3;
  vsg_vector2d_scalp (&v, w, &res);
  vsg_vector2d_set (&ref, 0, 3);
  ret += vcheck ("3 * [0, 1] == [0, 3]", &res, &ref);

  vsg_vector2d_set (&v, 0.1, 0.2);
  w = 1.2;
  vsg_vector2d_scalp (&v, w, &res);
  vsg_vector2d_set (&ref, 0.12, 0.24);
  ret += vcheck ("1.2 * [0.1, 0.2] == [0.12, 0.24]", &res, &ref);

  vsg_vector2d_set (&v, 1.2e-2,  0.);
  w = 1.e+3;
  vsg_vector2d_scalp (&v, w, &res);
  vsg_vector2d_set (&ref, 12, 0);
  ret += vcheck ("[1.2e-2,  0.] * 1.e+3 == [12, 0]", &res, &ref);

  vsg_vector2d_set (&v, 123, 456);
  w = 0;
  vsg_vector2d_scalp (&v, w, &res);
  vsg_vector2d_set (&ref, 0, 0);
  ret += vcheck ("[123, 456] * 0. == [0, 0]", &res, &ref);

  return ret;
}
