#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

#define CHECK(test) \
if (!(test)) {g_printerr ("Error \"%s\"\n", #test); ret += 1;}


gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  if (argc > 1 && g_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  VsgVector3d a = {0, 0, 0};
  VsgVector3d b = {1, 0, 0};
  VsgVector3d c = {0, 1, 0};
  VsgVector3d d = {0, 0, 1};
  VsgVector3d e = {.9, .9, .9};
  VsgVector3d f = {1.1, 1.1, 1.1};

  CHECK (vsg_vector3d_orient (&a, &b, &c, &d) > 0.);
  CHECK (vsg_vector3d_orient (&a, &b, &d, &c) < 0.);
  CHECK (vsg_vector3d_insphere (&a, &b, &c, &d, &e) > 0.);
  CHECK (vsg_vector3d_insphere (&a, &b, &c, &d, &f) < 0.);

  return ret;
}
