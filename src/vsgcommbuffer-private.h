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

#ifndef __VSG_COMMBUFFER_PRIVATE_H__ /* __VSG_COMMBUFFER_RIVATE_H__ */
#define __VSG_COMMBUFFER_RIVATE_H__

#include <vsg/vsgcommbuffer.h>

G_BEGIN_DECLS;

VsgPackedMsg * vsg_comm_buffer_get_send_buffer (VsgCommBuffer *cb, gint dst);

VsgPackedMsg * vsg_comm_buffer_get_recv_buffer (VsgCommBuffer *cb, gint src);

G_END_DECLS;

#endif /* __VSG_COMMBUFFER_RIVATE_H__ */
