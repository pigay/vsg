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
  VsgVector3d v, w, res, ref;

  if (argc > 1 && g_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  vsg_vector3d_set (&v, 1, 0, 0);
  vsg_vector3d_set (&w, 0, 1, 0);
  vsg_vector3d_vecp (&v, &w, &res);
  vsg_vector3d_set (&ref, 0, 0, 1);
  ret += vcheck ("[1, 0, 0] ^ [0, 1, 0] == [0, 0, 1]", &res, &ref);

  vsg_vector3d_set (&v, 1, 2, 3);
  vsg_vector3d_set (&w, 0.1, 0.2, 0.3);
  vsg_vector3d_vecp (&v, &w, &res);
  vsg_vector3d_set (&ref, 0, 0, 0);
  ret += vcheck ("[1, 2, 3] ^ [0.1, 0.2, 0.3] == [0, 0, 0]", &res, &ref);

  vsg_vector3d_set (&v, 1.2e-2, 0, 0);
  vsg_vector3d_set (&w, 1.2e-3, 0, 0);
  vsg_vector3d_vecp (&v, &w, &res);
  vsg_vector3d_set (&ref, 0, 0, 0);
  ret += vcheck ("[1.2e-2, 0, 0] ^ [1.2e-3, 0, 0] == [0, 0, 0]", &res, &ref);

  vsg_vector3d_set (&v, 0, 0, 0);
  vsg_vector3d_set (&w, 0, 0, 0);
  vsg_vector3d_vecp (&v, &w, &res);
  vsg_vector3d_set (&ref, 0, 0, 0);
  ret += vcheck ("[0, 0, 0] ^ [0, 0, 0] == [0, 0, 0]", &res, &ref);

  vsg_vector3d_set (&v, 1, 1, 1);
  vsg_vector3d_set (&w, 1, 2.123, 0);
  vsg_vector3d_vecp (&v, &w, &res);
  vsg_vector3d_set (&ref, -2.123, 1, 1.123);
  ret += vcheck ("[1, 1, 1] ^ [1, 2.123, 0] == [-2.123, 1, 1.123]", &res,
                 &ref);

  return ret;
}
