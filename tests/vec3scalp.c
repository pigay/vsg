#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

static gdouble epsilon = 1.e-8;

static gint vcheck (const gchar *label, VsgVector3d *res, VsgVector3d *ref)
{
  VsgVector3d tmp;
  gdouble r, refr;

  tmp.x = res->x - ref->x;
  tmp.y = res->y - ref->y;
  tmp.z = res->z - ref->z;

  r = sqrt (tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z);
  refr = sqrt (ref->x*ref->x + ref->y*ref->y + ref->z*ref->z);

  if (refr > epsilon) r /= refr;

  if (r > epsilon)
    {
      g_printerr ("Error \"%s\" ref(%f, %f, %f) != res(%f, %f, %f)\n",
                  label,
                  ref->x, ref->y, ref->z,
                  res->x, res->y, res->z);

      return 1;
    }

  return 0;
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;
  VsgVector3d v, res, ref;
  gdouble w;

  if (argc > 1 && g_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  vsg_vector3d_set (&v, 0, 1, 2);
  w = 3;
  vsg_vector3d_scalp (&v, w, &res);
  vsg_vector3d_set (&ref, 0, 3, 6);
  ret += vcheck ("3 * [0, 1, 2] == [0, 3, 6]", &res, &ref);

  vsg_vector3d_set (&v, 0.1, 0.2, 0.3);
  w = 1.2;
  vsg_vector3d_scalp (&v, w, &res);
  vsg_vector3d_set (&ref, 0.12, 0.24, 0.36);
  ret += vcheck ("1.2 * [0.1, 0.2, 0.3] == [0.12, 0.24, 0.36]", &res, &ref);

  vsg_vector3d_set (&v, 1.2e-2, 0, 0);
  w = 1.e+3;
  vsg_vector3d_scalp (&v, w, &res);
  vsg_vector3d_set (&ref, 12, 0, 0);
  ret += vcheck ("[1.2e-2, 0, 0] * 1.e+3 == [12, 0, 0]", &res, &ref);

  vsg_vector3d_set (&v, 123, 456, 789);
  w = 0.;
  vsg_vector3d_scalp (&v, w, &res);
  vsg_vector3d_set (&ref, 0, 0, 0);
  ret += vcheck ("[123, 456, 789] * 0. == [0, 0, 0]", &res, &ref);


  return ret;
}
