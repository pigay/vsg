#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <math.h>

#define CHECK(test) \
if (!(test)) {g_printerr ("Error \"%s\"\n", #test); ret += 1;}


gint main (gint argc, gchar ** argv)
{
  gint ret = 0;

  if (argc > 1 && g_ascii_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  VsgVector2d a = {0, 0};
  VsgVector2d b = {1, 0};
  VsgVector2d c = {0, 1};
  VsgVector2d d = {.9, .9};
  VsgVector2d e = {1.1, 1.1};

  CHECK (vsg_vector2d_orient (&a,&b,&c) > 0.);
  CHECK (vsg_vector2d_orient (&a,&c,&b) < 0.);
  CHECK (vsg_vector2d_incircle (&a,&b,&c,&d) > 0.);
  CHECK (vsg_vector2d_incircle (&a,&b,&c,&e) < 0.);

  return ret;
}
