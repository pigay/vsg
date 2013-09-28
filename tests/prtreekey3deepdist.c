/* basic point insertion, removal and traversal on a cloned VsgPRTree3d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <string.h>
#include <stdlib.h>

gboolean _verbose = FALSE;

typedef struct _TestData TestData;

struct _TestData {
  gchar *k1;
  gchar *k2;
  guint64 ref;
};

void read_key (gchar *str, VsgPRTreeKey3d *key)
{
  guint index = 0;
  gboolean done = FALSE;
  guint8 coord = 0;
  vsgloc3 loc = 0;

  *key = vsg_prtree_key3d_root;

  while (! done)
    {
      gchar c = str[index];

      switch (c) {
      case '0':
        if (coord > 2)
          {
            g_printerr ("Syntax error: coordinates too long");
            exit(-1);
          }
        coord ++;
        break;
      case '1':
        if (coord > 2)
          {
            g_printerr ("Syntax error: coordinates too long");
            exit(-1);
          }
        loc |= VSG_LOC3_AXIS (coord);
        coord ++;
        break;
      case ',':
        vsg_prtree_key3d_build_child (key, loc, key);
        loc = 0;
        coord = 0;
        break;
      case '\0':
        if (coord > 0)
          vsg_prtree_key3d_build_child (key, loc, key);
        done = TRUE;
        break;
      case ' ':
        break;
      default:
        g_printerr ("Syntax error: unknown character in key: \"%c\"", c);
        exit(-1);
      };

      index ++;
    }
}

gint check_deepest_dist (TestData *td)
{
  guint64 res;
  VsgPRTreeKey3d k1, k2;

  read_key (td->k1, &k1);
  read_key (td->k2, &k2);

  res = vsg_prtree_key3d_deepest_distance (&k1, &k2);

  if (res != td->ref)
    {
      g_printerr ("Error - k1 = \"%s\" -> (", td->k1);
      vsg_prtree_key3d_write (&k1, stderr);
      g_printerr (") k2 = \"%s\" -> (", td->k2);
      vsg_prtree_key3d_write (&k2, stderr);
      g_printerr (") ref=%ld != res=%ld ", td->ref, res);
      g_printerr ("\n");

      return 1;
    }
  else if (_verbose)
    {
      g_printerr ("OK - k1 = \"%s\"", td->k1);
      g_printerr (" k2 = \"%s\"", td->k2);
      g_printerr (" ref=%ld", td->ref);
      g_printerr ("\n");

    }

  return 0;
}

static
void parse_args (int argc, char **argv)
{
  int iarg = 1;
  char *arg;

  while (iarg < argc)
    {
      arg = argv[iarg];

      if (g_ascii_strncasecmp (arg, "-v", 2) == 0 ||
               g_ascii_strncasecmp (arg, "--verbose", 9) == 0)
        {
          _verbose = TRUE;
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

TestData test_data[] = {
  {"000,", "000,", 0},
  {"001,000", "000,", 1},
  {"000,001,000,001", "001,000,", 3},
  {"000,001,000,000", "001,000,", 4},
  {"001,001,000,001", "000,000,", 10},
  {"000,000,", "001,001,000,001", 10},
  {"111,101,010", "111,101,010,111", 0},
  {"111,101,010", "111,101,000,111", 1},
  {"111,101,000", "111,101,010,111", 2},
  {"110,100,001", "111,101,010,111", 10},
  {"000,", "100,100,100,100", 8},
  {NULL, NULL, 0},
};

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;
  gint i = 0;
  vsg_init_gdouble ();

  parse_args (argc, argv);

  while (test_data[i].k1 != NULL)
    {
      check_deepest_dist (&test_data[i]);
      i ++;
    }
  return ret;
}
