#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

static gdouble epsilon = 1.e-8;

static gint mcheck (const gchar *label, VsgMatrix3d *res, VsgMatrix3d *ref)
{
  gdouble tmp;
  gdouble r, refr;
  gint i;

  r = 0.;
  refr = 0.;
  for (i=0; i<9; i ++)
    {
      tmp= res->components[i] - ref->components[i];
      r += tmp*tmp;
      refr += ref->components[i]*ref->components[i];
    }

  r = sqrt (r);
  refr = sqrt (refr);

  if (refr > epsilon) r /= refr;

  if (r > epsilon)
    {
      g_printerr ("Error \"%s\":\n", label);
      g_printerr ("|%12g %12g %12g|    |%12g %12g %12g|\n",
                  ref->components[0], ref->components[3], ref->components[6],
                  res->components[0], res->components[3], res->components[6]);
      g_printerr ("|%12g %12g %12g| != |%12g %12g %12g|\n",
                  ref->components[1], ref->components[4], ref->components[7],
                  res->components[1], res->components[4], res->components[7]);
      g_printerr ("|%12g %12g %12g|    |%12g %12g %12g|\n",
                  ref->components[2], ref->components[5], ref->components[8],
                  res->components[2], res->components[5], res->components[8]);

      return 1;
    }

  return 0;
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;
  VsgMatrix3d v, w, res, ref;

  if (argc > 1 && g_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  vsg_matrix3d_set (&v,
                    1., 0., 0.,
                    0., 1., 0.,
                    0., 0., 1.);
  vsg_matrix3d_set (&w,
                    1., 0., 0.,
                    0., 1., 0.,
                    0., 0., 1.);
  vsg_matrix3d_add (&v, &w, &res);
  vsg_matrix3d_set (&ref,
                    2., 0., 0.,
                    0., 2., 0.,
                    0., 0., 2.);
  ret += mcheck ("Id+Id == 2Id", &res, &ref);

  vsg_matrix3d_set (&v,
                    1., 0., 0.,
                    1., 1., 0.,
                    1., 1., 1.);
  vsg_matrix3d_set (&w,
                    1., 1., 1.,
                    0., 1., 1.,
                    0., 0., 1.);
  vsg_matrix3d_add (&v, &w, &res);
  vsg_matrix3d_set (&ref,
                    2., 1., 1.,
                    1., 2., 1.,
                    1., 1., 2.);
  ret += mcheck ("L+U", &res, &ref);

  vsg_matrix3d_set (&v,
                    1., 2., 3.,
                    4., 5., 6.,
                    7., 8., 9.);
  vsg_matrix3d_set (&w,
                    9., 8., 7.,
                    6., 5., 4.,
                    3., 2., 1.);
  vsg_matrix3d_add (&v, &w, &res);
  vsg_matrix3d_set (&ref,
                    10., 10., 10.,
                    10., 10., 10.,
                    10., 10., 10.);
  ret += mcheck ("non-symmetric", &res, &ref);

  return ret;
}
