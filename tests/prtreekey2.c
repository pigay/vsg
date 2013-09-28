/* basic point insertion, removal and traversal on a cloned VsgPRTree2d */

#include "vsg-config.h"

#include "vsg/vsgd.h"

#include <string.h>

typedef struct _TestData TestData;

struct _TestData {
  VsgPRTreeKey2d k1;
  VsgPRTreeKey2d k2;
  guint8 ldi_ref;
  VsgPRTreeKey2d dpa_ref;
  vsgloc2 loc1_ref;
  vsgloc2 loc2_ref;
  guint64 dist_ref;
};

static const TestData _test_data_sentinel = {
  {0x0, 0x0, 0}, {0x0, 0x0, 0},  0, {0x0, 0x0, 0}, 0, 0, 0x0
};

static TestData _test_data[] = {
  {{0x132, 0x0, 10}, {0x132, 0x0, 10}, 0, {0x132, 0x0, 10}, 0, 0, 0x0},
  {{0x132, 0x0, 10}, {0x133, 0x0, 10}, 1, {0x99, 0x0, 9}, 0, 1, 0x1},
  {{0x132, 0x0, 10}, {0x72, 0x0, 10}, 9, {0x0, 0x0, 1}, 1, 0, 0xc0},
  {{0x72, 0x0, 10}, {0x132, 0x0, 10}, 9, {0x0, 0x0, 1}, 0, 1, 0xc0},
  {{0x0, 0x132, 10}, {0x0, 0x132, 10}, 0, {0x0, 0x132, 10}, 0, 0, 0x0},
  {{0x0, 0x132, 10}, {0x0, 0x72, 10}, 9, {0x0, 0x0, 1}, 2, 0, 0xc0},
  {{0x0aab5, 0x1579b, 17}, {0x555aa, 0xabcde, 20},  3, {0xaab5, 0x1579b, 17}, 0, 3, },

  {{0x0, 0x0, 2}, {0x1, 0x2, 2},  2, {0x0, 0x0, 0}, 0, 3, 0x2},
  {{0x0, 0x0, 2}, {0x1, 0x0, 2},  1, {0x0, 0x0, 1}, 0, 1, 0x1},

  {{0xff, 0xef, 8}, {0x70, 0xef, 8},  8, {0x0, 0x0, 0}, 1, 0, 0x8f},
  {{0xffaa, 0xefaa, 16}, {0x70aa, 0xefaa, 16},  16, {0x0, 0x0, 0}, 1, 0, 0x8f00},

  {{0x1, 0x0, 1}, {0x2, 0x0, 2},  0, {0x2, 0x0, 2}, 0, 0, 0x0},
  {{0x0, 0x1, 1}, {0x0, 0x2, 2},  0, {0x0, 0x2, 2}, 0, 0, 0x0},
  {{0x1, 0x0, 1}, {0x4, 0x0, 3},  0, {0x4, 0x0, 3}, 0, 0, 0x0},
  {{0x0, 0x1, 1}, {0x0, 0x4, 3},  0, {0x0, 0x4, 3}, 0, 0, 0x0},

  {{0x2, 0x0, 2}, {0x1, 0x0, 1},  0, {0x2, 0x0, 2}, 0, 0, 0x0},
  {{0x0, 0x2, 2}, {0x0, 0x1, 1},  0, {0x0, 0x2, 2}, 0, 0, 0x0},
  {{0x4, 0x0, 3}, {0x1, 0x0, 1},  0, {0x4, 0x0, 3}, 0, 0, 0x0},
  {{0x0, 0x4, 3}, {0x0, 0x1, 1},  0, {0x0, 0x4, 3}, 0, 0, 0x0},
  //{{0x, 0x, }, {0x, 0x, },  , {0x, 0x, }, , },
  {{0x0, 0x0, 0}, {0x0, 0x0, 0},  0, {0x0, 0x0, 0}, 0, 0, 0x0}, // sentinel
};

static gboolean _check_test_data (TestData *data)
{
  gint8 ldi;
  VsgPRTreeKey2d dpa = {0};
  vsgloc2 loc1, loc2;
  guint64 dist;

  ldi = vsg_prtree_key2d_first_different_index (&data->k1, &data->k2);

  if (ldi != data->ldi_ref)
    {
      g_printerr ("Error on first_different_index:\n");
      g_printerr ("k1: ");
      vsg_prtree_key2d_write (&data->k1, stderr);
      g_printerr ("\nk2: ");
      vsg_prtree_key2d_write (&data->k2, stderr);

      g_printerr ("\ngot %d, should be %d\n\n", ldi, data->ldi_ref);
    }

  vsg_prtree_key2d_deepest_common_ancestor(&data->k1, &data->k2, &dpa);

  if (memcmp (&data->dpa_ref, &dpa, sizeof (VsgPRTreeKey2d)) != 0)
    {
      g_printerr ("Error on deepest_common_ancestor:\n");
      g_printerr ("k1: ");
      vsg_prtree_key2d_write (&data->k1, stderr);
      g_printerr ("\nk2: ");
      vsg_prtree_key2d_write (&data->k2, stderr);

      g_printerr ("\ngot (");
      vsg_prtree_key2d_write (&dpa, stderr);
      g_printerr (")\nshould be (");
      vsg_prtree_key2d_write (&data->dpa_ref, stderr);
      g_printerr (")\n\n");
    }

  loc1 = vsg_prtree_key2d_loc2 (&data->k1, &data->k2);
  if (loc1 != data->loc1_ref)
    {
      g_printerr ("Error on loc2 (k1, k2):\n");
      g_printerr ("k1: ");
      vsg_prtree_key2d_write (&data->k1, stderr);
      g_printerr ("\nk2: ");
      vsg_prtree_key2d_write (&data->k2, stderr);

      g_printerr ("\ngot %d, should be %d\n\n", loc1, data->loc1_ref);
    }

  loc2 = vsg_prtree_key2d_loc2 (&data->k2, &data->k1);
  if (loc2 != data->loc2_ref)
    {
      g_printerr ("Error on loc2 (k2, k1):\n");
      g_printerr ("k1: ");
      vsg_prtree_key2d_write (&data->k1, stderr);
      g_printerr ("\nk2: ");
      vsg_prtree_key2d_write (&data->k2, stderr);

      g_printerr ("\ngot %d, should be %d\n\n", loc2, data->loc2_ref);
    }

  dist = vsg_prtree_key2d_distance (&data->k1, &data->k2);
  if (dist != data->dist_ref)
    {
      g_printerr ("Error on dist (k1, k2):\n");
      g_printerr ("k1: ");
      vsg_prtree_key2d_write (&data->k1, stderr);
      g_printerr ("\nk2: ");
      vsg_prtree_key2d_write (&data->k2, stderr);

      g_printerr ("\ngot %#"G_GINT64_MODIFIER"x, should be %#"G_GINT64_MODIFIER"x\n\n", dist, data->dist_ref);

    }

  return memcmp (data, &_test_data_sentinel, sizeof (TestData)) == 0;
}

gint main (gint argc, gchar ** argv)
{
  gint ret = 0;
  gint i = 0;
  vsg_init_gdouble ();

  if (argc > 1 && g_ascii_strncasecmp (argv[1], "--version", 9) == 0)
    {
      g_print ("%s\n", PACKAGE_VERSION);
      return 0;
    }

  while (! _check_test_data (&_test_data[i]))
    i ++;

  return ret;
}
