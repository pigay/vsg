/* LIBVSG - Visaurin Geometric Library
 * Copyright (C) 2006-2008 Pierre Gay
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __VSG_TIMING_H__ /* __VSG_TIMING_H__ */
#define __VSG_TIMING_H__

#ifdef VSG_TIMING_OUTPUT
#include <glib/gprintf.h>
#endif

G_BEGIN_DECLS;

#ifdef VSG_TIMING_OUTPUT

#ifdef VSG_HAVE_MPI

#define _VSG_TIMING_PREFIX(key, communicator)                           \
  gint key ##_rk = 0;                                                   \
  if ((communicator) != MPI_COMM_NULL) MPI_Comm_rank((communicator), &key ##_rk); \
  g_sprintf( key ## _timing_prefix, "%d: ", key ## _rk);

#else

#define _VSG_TIMING_PREFIX(key, communicator)                \
  g_sprintf( key ## _timing_prefix, "%d: ", 0);

#endif /* VSG_HAVE_MPI */

#define VSG_TIMING_START(key, communicator) { \
  GTimer *vsg_timer_ ## key = g_timer_new (); \
  gchar key ## _timing_prefix[10]; \
  _VSG_TIMING_PREFIX (key, (communicator));

#define VSG_TIMING_END(key, file)                                       \
  if (g_getenv ("VSG_TIMING_SUPPRESS_OUPUT") == NULL)                                              \
    g_fprintf (file,  "%s" #key " elapsed=%f seconds\n",                \
               key ## _timing_prefix,                                   \
               g_timer_elapsed (vsg_timer_ ## key, NULL));              \
  g_timer_destroy (vsg_timer_ ## key);                                  \
}

#else

#define VSG_TIMING_START(key, communicator)
#define VSG_TIMING_END(key, file)

#endif /* VSG_TIMING_OUTPUT */

G_END_DECLS;

#endif /* __VSG_TIMING_H__ */
