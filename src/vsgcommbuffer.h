/* LIBVSG - Visaurin Geometric Library
 * Copyright (C) 2006-2007 Pierre Gay
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

#ifndef __VSG_COMMBUFFER_H__ /* __VSG_COMMBUFFER_H__ */
#define __VSG_COMMBUFFER_H__

#include <glib.h>
#include <glib-object.h>

#include <vsg/vsgmpi.h>
#include <vsg/vsgpackedmsg.h>

G_BEGIN_DECLS;

typedef struct _VsgCommBuffer VsgCommBuffer;

VsgCommBuffer *vsg_comm_buffer_new (MPI_Comm comm);


void vsg_comm_buffer_send_append (VsgCommBuffer *cb, gint dst, gpointer buf,
				  gint count, MPI_Datatype type);


void vsg_comm_buffer_recv_read (VsgCommBuffer *cb, gint src, gpointer buf,
				gint count, MPI_Datatype type);

void vsg_comm_buffer_share (VsgCommBuffer *cb);

void vsg_comm_buffer_set_bcast (VsgCommBuffer *cb, VsgPackedMsg *model);

void vsg_comm_buffer_drop_send_buffer (VsgCommBuffer *cb, gint dst);

void vsg_comm_buffer_drop_recv_buffer (VsgCommBuffer *cb, gint src);

void vsg_comm_buffer_drop_send_buffers (VsgCommBuffer *cb);

void vsg_comm_buffer_drop_recv_buffers (VsgCommBuffer *cb);

void vsg_comm_buffer_drop_buffers (VsgCommBuffer *cb);

void vsg_comm_buffer_free (VsgCommBuffer *cb);

G_END_DECLS;

#endif /* __VSG_COMMBUFFER_H__ */
