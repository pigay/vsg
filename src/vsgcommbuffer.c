#include "vsgcommbuffer.h"
#include "vsgcommbuffer-private.h"

#include "string.h"

/**
 * VsgCommBuffer:
 *
 * An opaque structure. Manipulate only through following functions.
 */
struct _VsgCommBuffer
{
  MPI_Comm communicator;

  gint numtasks;
  gint mytid;

  VsgPackedMsg *send;
  VsgPackedMsg *recv;
};

/**
 * vsg_comm_buffer_new:
 * @comm: a MPI communicator.
 *
 * Allocates a new #VsgCommBuffer.
 *
 * Returns: a #VsgCommBuffer
 */
VsgCommBuffer *vsg_comm_buffer_new (MPI_Comm comm)
{
  VsgCommBuffer *ret;

  ret = g_malloc (sizeof (VsgCommBuffer));

  ret->communicator = comm;

  MPI_Comm_size (comm, &ret->numtasks);
  MPI_Comm_rank (comm, &ret->mytid);

  if (ret->numtasks > 0)
    {
      gint i;

      ret->send = g_malloc0 (ret->numtasks * sizeof (VsgPackedMsg));
      ret->recv = g_malloc0 (ret->numtasks * sizeof (VsgPackedMsg));

      for (i=0; i<ret->numtasks; i++)
	{
	  vsg_packed_msg_init (&ret->send[i], comm);
	  vsg_packed_msg_init (&ret->recv[i], comm);
	}
    }

  return ret;
}

/**
 * vsg_comm_buffer_send_append:
 * @cb: A #VsgCommBuffer instance.
 * @dst: the destination task id.
 * @buf: pointer to the beginning of data to be stored.
 * @count: number of @type data to store.
 * @type: type of the data to be stored.
 *
 * Appends some data to be sent with @cb. If @dst is negative, @buf
 * will be appended to all processor buffers
 */
void vsg_comm_buffer_send_append (VsgCommBuffer *cb, gint dst, gpointer buf,
				  gint count, MPI_Datatype type)
{
  g_return_if_fail (cb != NULL);

  g_return_if_fail (dst < cb->numtasks);

  /* negative value means broadcast */
  if (dst < 0)
    {
      gint i;

      for (i=0; i<cb->numtasks; i++)
	vsg_packed_msg_send_append (&cb->send[dst], buf, count, type);

      return;
    }
  else
    vsg_packed_msg_send_append (&cb->send[dst], buf, count, type);

}

/**
 * vsg_comm_buffer_recv_read:
 * @cb: A #VsgCommBuffer instance.
 * @src: the source task id. Can be %MPI_ANY_SOURCE.
 * @buf: pointer to the beginning of data to be read.
 * @count: number of @type data to read.
 * @type: type of the data to be read.
 *
 * Reads some data from a particular receive bufferin @cb.
 */
void vsg_comm_buffer_recv_read (VsgCommBuffer *cb, gint src, gpointer buf,
				gint count, MPI_Datatype type)
{
  g_return_if_fail (cb != NULL);

  g_return_if_fail (src >= 0);

  g_return_if_fail (src < cb->numtasks);

  vsg_packed_msg_recv_read (&cb->recv[src], buf, count, type);
}

/**
 * vsg_comm_buffer_share:
 * @cb: A #VsgCommBuffer instance.
 *
 * Performs all send and receive operations recorded in @cb.
 */
void vsg_comm_buffer_share (VsgCommBuffer *cb)
{
  gint i;
  gint numtasks = cb->numtasks;
  gint mytid = cb->mytid;

  g_return_if_fail (cb != NULL);

/*   g_printerr ("step=%d proc=%d src=%d dst=%d\n", 0, mytid, mytid, mytid); */
  memcpy (&cb->recv[mytid], &cb->send[mytid], sizeof (VsgPackedMsg));
  cb->recv[mytid].position = vsg_packed_msg_header_size ();
  memset (&cb->send[mytid], 0, sizeof (VsgPackedMsg));

  for (i=1; i<numtasks; i++)
    {
      MPI_Request request;
      gint src = (mytid - i + numtasks) % numtasks;
      gint dst = (mytid + i + numtasks) % numtasks;
      gint tag;

/*       g_printerr ("step=%d proc=%d src=%d dst=%d sz=%d\n", i, mytid, src, dst, */
/* 		  cb->send[dst].position); */

      /* send */
      tag = i*numtasks + mytid;

      vsg_packed_msg_isend (&cb->send[dst], dst, tag, &request);
/*       g_printerr ("step=%d proc=%d src=%d dst=%d send ok\n", i, mytid, src, dst); */

      /* recv */
      tag = i*numtasks + src;

      vsg_packed_msg_recv (&cb->recv[src], src, tag);
/*       g_printerr ("step=%d proc=%d src=%d dst=%d recv ok\n", i, mytid, src, dst); */

      vsg_packed_msg_wait (&cb->send[dst], &request);

      vsg_packed_msg_drop_buffer (&cb->send[dst]);

    }
}

/**
 * vsg_comm_buffer_set_bcast:
 * @cb: A #VsgCommBuffer instance.
 * @model: the message data to pass to other processors.
 *
 * Sets @cb to be like a broadcast send without extra copying of the
 * send data.
 */
void vsg_comm_buffer_set_bcast (VsgCommBuffer *cb, VsgPackedMsg *model)
{
  gint dst;
  g_return_if_fail (cb != NULL);

  if (model == NULL) return;

  for (dst=0; dst<cb->numtasks; dst++)
    {
      if (dst == cb->mytid) continue;

      vsg_packed_msg_set_reference (&cb->send[dst], model);
    }
}

/**
 * vsg_comm_buffer_get_send_buffer:
 * @cb: a #VsgCommBuffer
 * @dst: desired buffer processor number
 *
 * Requests one of the send buffers of @cb.
 *
 * Returns: a handle to the send buffer.
 */
VsgPackedMsg * vsg_comm_buffer_get_send_buffer (VsgCommBuffer *cb, gint dst)
{
  g_return_val_if_fail (cb != NULL, NULL);

  g_return_val_if_fail (dst >= 0, NULL);

  g_return_val_if_fail (dst < cb->numtasks, NULL);

  return &cb->send[dst];
}

/**
 * vsg_comm_buffer_get_recv_buffer:
 * @cb: a #VsgCommBuffer
 * @src: desired buffer processor number
 *
 * Requests one of the receive buffers of @cb.
 *
 * Returns: a handle to the receive buffer.
 */
VsgPackedMsg * vsg_comm_buffer_get_recv_buffer (VsgCommBuffer *cb, gint src)
{
  g_return_val_if_fail (cb != NULL, NULL);

  g_return_val_if_fail (src >= 0, NULL);

  g_return_val_if_fail (src < cb->numtasks, NULL);

  return &cb->recv[src];
}

/**
 * vsg_comm_buffer_drop_send_buffer:
 * @cb: A #VsgCommBuffer instance.
 * @dst: the task id which buffer is to be deleted.
 *
 * Deletes memory associated with one particular send buffer.
 */
void vsg_comm_buffer_drop_send_buffer (VsgCommBuffer *cb, gint dst)
{
  g_return_if_fail (cb != NULL);

  g_return_if_fail (dst >= 0);

  g_return_if_fail (dst < cb->numtasks);

  vsg_packed_msg_drop_buffer (&cb->send[dst]);

}

/**
 * vsg_comm_buffer_drop_recv_buffer:
 * @cb: A #VsgCommBuffer instance.
 * @src: the task id which buffer is to be deleted.
 *
 * Deletes memory associated with one particular receive buffer.
 */
void vsg_comm_buffer_drop_recv_buffer (VsgCommBuffer *cb, gint src)
{
  g_return_if_fail (cb != NULL);

  g_return_if_fail (src >= 0);

  g_return_if_fail (src < cb->numtasks);

  vsg_packed_msg_drop_buffer (&cb->recv[src]);

}

/**
 * vsg_comm_buffer_drop_send_buffers:
 * @cb: A #VsgCommBuffer instance.
 *
 * Deletes memory associated with all send buffers in @cb.
 */
void vsg_comm_buffer_drop_send_buffers (VsgCommBuffer *cb)
{
  gint i;

  g_return_if_fail (cb != NULL);

  for (i=0; i<cb->numtasks; i++)
    {
      vsg_comm_buffer_drop_send_buffer (cb, i);
    }
}

/**
 * vsg_comm_buffer_drop_recv_buffers:
 * @cb: A #VsgCommBuffer instance.
 *
 * Deletes memory associated with all receive buffers in @cb.
 */
void vsg_comm_buffer_drop_recv_buffers (VsgCommBuffer *cb)
{
  gint i;

  g_return_if_fail (cb != NULL);

  for (i=0; i<cb->numtasks; i++)
    {
      vsg_comm_buffer_drop_recv_buffer (cb, i);
    }
}

/**
 * vsg_comm_buffer_drop_buffers:
 * @cb: A #VsgCommBuffer instance.
 *
 * Deletes memory associated with all buffers in @cb.
 */
void vsg_comm_buffer_drop_buffers (VsgCommBuffer *cb)
{
  vsg_comm_buffer_drop_send_buffers (cb);
  vsg_comm_buffer_drop_recv_buffers (cb);
}

/**
 * vsg_comm_buffer_free:
 * @cb: A #VsgCommBuffer instance.
 *
 * Frees @cb.
 */
void vsg_comm_buffer_free (VsgCommBuffer *cb)
{
  vsg_comm_buffer_drop_buffers (cb);

  if (cb->numtasks > 0)
    {
      g_free (cb->send);
      g_free (cb->recv);
    }

  g_free (cb);
}
