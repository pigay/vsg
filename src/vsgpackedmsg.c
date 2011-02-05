#include "vsgpackedmsg.h"

#include <vsg-config.h>

#include <string.h>
#include <stdio.h>

#include <glib/gprintf.h>
#include <stdarg.h>

#ifdef VSG_PACKED_MSG_TRACE

typedef gint64 _trace_id;
#define _MPI_TRACE_ID_TYPE MPI_LONG_LONG
#define _PM_ID_SIZE sizeof (_trace_id)
#define _PM_BEGIN_POS _PM_ID_SIZE
#define _TRACE_FMT G_GINT64_FORMAT

FILE *_trace_file = NULL;
static gboolean _trace_active = TRUE;
static _trace_id _msgid = 0;
static _trace_id _msgid_incr = 0;
static GHashTable *_trace_comm_hash = NULL;
/* static gint _trace_id_max = 0; */
static GTimer *_timer = NULL;

static void _trace_atexit ()
{
  if (_trace_file != NULL)
    {
      fclose (_trace_file);
      _trace_file = NULL;

      g_hash_table_destroy (_trace_comm_hash);
      g_timer_destroy (_timer);
    }
}

static MPI_Fint _trace_write_comm (MPI_Comm comm)
{
  MPI_Fint ret = MPI_Comm_c2f (comm);
  MPI_Group world_group, group;
  gint rk, sz, i;
  gint world_rk, world_sz;
  gint *world_rks, *comm_rks;

  MPI_Comm_group (MPI_COMM_WORLD, &world_group);
  MPI_Comm_group (comm, &group);

  MPI_Comm_rank (MPI_COMM_WORLD, &world_rk);
  MPI_Comm_size (MPI_COMM_WORLD, &world_sz);

  MPI_Comm_rank (comm, &rk);
  MPI_Comm_size (comm, &sz);

  world_rks = g_malloc (world_sz * sizeof (int));
  comm_rks = g_malloc (world_sz * sizeof (int));

  for (i=0; i<world_sz; i++)
    world_rks[i] = i;

  MPI_Group_translate_ranks (world_group, world_sz, world_rks, group, comm_rks);

  g_fprintf (_trace_file, "comm=%d rk=%d sz=%d translate={",
	     ret, rk, sz);

  for (i=0; i<world_sz; i++)
    {
      g_fprintf (_trace_file, "%d ", comm_rks[i]);
    }

  g_fprintf (_trace_file, "}\n");

  MPI_Group_free (&world_group);
  MPI_Group_free (&group);

  g_free (world_rks);
  g_free (comm_rks);

  return ret;
}

static gint _trace_comm_id (MPI_Comm comm)
{
  gint *ret = g_hash_table_lookup (_trace_comm_hash, comm);

  if (ret == NULL)
    {
      ret = g_malloc (sizeof (gint));

      *ret = _trace_write_comm (comm);

      g_hash_table_insert (_trace_comm_hash, comm, ret);
    }

  return *ret;
}

static inline void _trace_file_open ()
{
  if (_trace_file == NULL)
    {
      gint rk, sz;
      gchar filename[256];
      GTimeVal now;
      const gchar *trace_dir = g_getenv ("VSG_PACKED_MSG_TRACE_DIR");
      gchar *trace_path;

      MPI_Comm_rank (MPI_COMM_WORLD, &rk);
      MPI_Comm_size (MPI_COMM_WORLD, &sz);

      _msgid = rk+1;
      _msgid_incr = sz;

      g_sprintf (filename, "vsg-packed-msg-%04d.trace", rk);

      if (trace_dir != NULL)
	trace_path = g_build_filename (trace_dir, filename, NULL);
      else
	trace_path = g_strdup (filename);

/*       g_printerr ("%d : opening trace \"%s\"\n", rk, trace_path); */

      _trace_file = fopen (trace_path, "w");

      g_free (trace_path);

      _trace_comm_hash = g_hash_table_new_full (g_direct_hash, g_direct_equal,
						NULL, g_free);

      g_get_current_time (&now);
      _timer = g_timer_new ();

      g_fprintf (_trace_file, "opening processor %d: %ld %ld\n", rk, now.tv_sec, now.tv_usec);

      _trace_comm_id (MPI_COMM_WORLD);
      _trace_comm_id (MPI_COMM_SELF);

      g_atexit (_trace_atexit);
    }
}

void vsg_packed_msg_trace_set_active (gboolean active)
{
  _trace_active = active;
}

gboolean vsg_packed_msg_trace_get_active ()
{
  return _trace_active;
}

static void _trace_write_msg (gchar *name, MPI_Comm comm, _trace_id msgid,
			      gint remote, gint tag, gint size)
{
  gdouble seconds;
  gulong microseconds;
  gint commid;

  if (! _trace_active) return;

  commid = _trace_comm_id (comm);

  seconds = g_timer_elapsed (_timer, NULL);
  microseconds = (gulong) (seconds * 1.e6);
  g_fprintf (_trace_file, "%lu: msg=%s msgid=%"_TRACE_FMT" comm=%d "
             "remote=%d tag=%d size=%d\n",
             microseconds, name,
             msgid, commid, remote, tag,
             (gint) (size - sizeof (_trace_id)));

  fflush (_trace_file);
}

void vsg_packed_msg_trace (gchar *format, ...)
{
  gdouble seconds;
  gulong microseconds;
  va_list ap;

  if (! _trace_active) return;

  _trace_file_open ();

  seconds = g_timer_elapsed (_timer, NULL);
  microseconds = (glong) (seconds * 1.e6);

  g_fprintf (_trace_file, "%lu: # ", microseconds);

  va_start (ap, format);
  g_vfprintf (_trace_file, format, ap);
  va_end (ap);

  g_fprintf (_trace_file, "\n");

  fflush (_trace_file);
}

static inline void _trace_set_msgid (VsgPackedMsg *msg, _trace_id msgid)
{
  gint oldpos = msg->position;

  msg->position = 0;

  if (msg->buffer == NULL)
    {
      msg->buffer = g_malloc0 (_PM_ID_SIZE);
      msg->size = _PM_ID_SIZE;
    }

  MPI_Pack (&msgid, 1, _MPI_TRACE_ID_TYPE, msg->buffer, msg->size,
	    &msg->position, msg->communicator);

  msg->position = MAX (msg->position, oldpos);

  _msgid += _msgid_incr;
}

static void _trace_write_msg_send (VsgPackedMsg *msg, gchar *mode, gint dst,
				   gint tag)
{
  _trace_id msgid;

  _trace_file_open ();

  msgid = _msgid;

  if (msg->own_buffer)
    {
      /* set the msg id at the beginning of the message buffer */
      _trace_set_msgid (msg, _msgid);
    }
  else
    {
      /* negative msgid for shared buffer messages */
      msgid = - _msgid;

      /* set the msg id at the beginning of the message buffer */
      _trace_set_msgid (msg, msgid);
    }

  _trace_write_msg (mode, msg->communicator, msgid, dst, tag, msg->position);
}

static void _trace_write_msg_recv (VsgPackedMsg *msg, gchar *mode, gint src,
				   gint tag)
{
  _trace_id msgid = 0;

  _trace_file_open ();

  msg->position = 0;

  vsg_packed_msg_recv_read (msg, &msgid, 1, _MPI_TRACE_ID_TYPE);

  _trace_write_msg (mode, msg->communicator, msgid, src, tag, msg->size);
}

G_CONST_RETURN gint vsg_packed_msg_header_size ()
{
  return _PM_ID_SIZE;
}

#else

#define _PM_ID_SIZE (0)
#define _PM_BEGIN_POS (0)

#define _trace_file_open() /* noop */
#define _trace_write_msg_send(msg, mode, dst, tag) /* noop */
#define _trace_write_msg_recv(msg, mode, src, tag) /* noop */

void vsg_packed_msg_trace (gchar *format, ...)
{
  /* noop */
}

void vsg_packed_msg_trace_set_active (gboolean active)
{
  /* noop */
}

gboolean vsg_packed_msg_trace_get_active ()
{
  return FALSE;
}

G_CONST_RETURN gint vsg_packed_msg_header_size ()
{
  return 0;
}

#endif

/**
 * VSG_PACKED_MSG_STATIC_INIT:
 * @comm: a #MPI_Comm
 *
 * Performs static initialization of a #VsgPackedMsg structure.
 */

/**
 * VsgPackedMsg:
 * @communicator: the MPI comunicator on which the #VsgPackedMsg is to be used.
 *
 * Stores heterogenous MPI message data.
 */

/**
 * vsg_packed_msg_new:
 * @comm: a MPI communicator.
 *
 * Creates a new packed message buffer for use inside @comm.
 *
 * Returns: newly allocated #VsgPackedMsg.
 */
VsgPackedMsg *vsg_packed_msg_new (MPI_Comm comm)
{
  VsgPackedMsg *ret;

  ret = g_malloc (sizeof (VsgPackedMsg));

  vsg_packed_msg_init (ret, comm);

  return ret;
}

/**
 * vsg_packed_msg_init:
 * @pm: a #VsgPackedMsg.
 * @comm: a MPI communicator.
 *
 * Initializes @pm for holding @comm messages. Will free all previously stored
 * message data.
 */
void vsg_packed_msg_init (VsgPackedMsg *pm, MPI_Comm comm)
{
  g_return_if_fail (pm != NULL);

  pm->communicator = comm;

#ifdef VSG_PACKED_MSG_TRACE
  pm->buffer = g_malloc0 (sizeof (_trace_id));
#else
  pm->buffer = NULL;
#endif
  pm->position = _PM_BEGIN_POS;
  pm->size = _PM_ID_SIZE;
  pm->own_buffer = TRUE;
}

/**
 * vsg_packed_msg_reset:
 * @pm: a #VsgPackedMsg.
 *
 * 
 */
void vsg_packed_msg_reset (VsgPackedMsg *pm)
{
  g_return_if_fail (pm != NULL);

  pm->position = _PM_BEGIN_POS;
}

/**
 * vsg_packed_msg_set_reference:
 * @pm: a #VsgPackedMsg.
 * @model: the message to point to.
 *
 * Sets @pm to be a reference to the data contained in @model without data
 * copy. @model must exist as long as another message points to its data.
 */
void vsg_packed_msg_set_reference (VsgPackedMsg *pm, VsgPackedMsg *model)
{
  g_return_if_fail (pm != NULL);

  vsg_packed_msg_drop_buffer (pm);

  if (model == NULL) return;

  memcpy (pm, model, sizeof (VsgPackedMsg));

  pm->own_buffer = FALSE;
}

/**
 * vsg_packed_msg_send_append:
 * @pm: a #VsgPackedMsg.
 * @buf: pointer to the beginning of data to be stored.
 * @count: number of @type data to store.
 * @type: type of the data to be stored.
 *
 * Appends @count instances of @type data to the message buffer.
 */
void vsg_packed_msg_send_append (VsgPackedMsg *pm, gpointer buf,
                                 gint count, MPI_Datatype type)
{
  gint pos, size, addsize;
  gint ierr;

  g_return_if_fail (pm != NULL);

  g_assert (pm->own_buffer == TRUE);

#ifdef VSG_PACKED_MSG_TRACE
  /* init msg id */
  if (pm->position == 0)
    {
      if (pm->buffer == NULL) pm->buffer = g_malloc0 (_PM_ID_SIZE);
      pm->size = MAX (pm->size, _PM_ID_SIZE);
      pm->position = _PM_BEGIN_POS;
    }
#endif

  pos = pm->position;
  size = pm->size;

  /* compute size of this new message part */
  MPI_Pack_size (count, type, pm->communicator, &addsize);

  /* allocate enough memory in message msg to store this message */
  if ((addsize + pos) > size)
    {
      size = MAX (size + 1024, addsize+pos);

      pm->buffer = g_realloc (pm->buffer, size * sizeof (char));

      pm->size = size;
    }

  ierr = MPI_Pack (buf, count, type, pm->buffer, size,
		   &pm->position, pm->communicator);

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);

}

/**
 * vsg_packed_msg_recv_read:
 * @pm: a #VsgPackedMsg.
 * @buf: pointer to the beginning of data to be read.
 * @count: number of @type data to read.
 * @type: type of the data to be read.
 *
 * Reads @count instance of @type data from the current position in the buffer.
 * The position then is updated to the first byte after read data.
 */
void vsg_packed_msg_recv_read (VsgPackedMsg *pm, gpointer buf,
                               gint count, MPI_Datatype type)
{
  gint size;
  gint ierr;

  g_return_if_fail (pm != NULL);

  size = pm->size;

/*   { */
/*     gint mytid; */
/*     MPI_Comm_rank (pm->communicator, &mytid); */
/*     g_printerr ("%d: unpacking %d data at %d\n", mytid, count, pm->position); */
/*   } */

  ierr = MPI_Unpack (pm->buffer, size, &pm->position, 
		     buf, count, type,  pm->communicator);
  
  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);

}

/**
 * vsg_packed_msg_send:
 * @pm: a #VsgPackedMsg.
 * @dst: the destination task id.
 * @tag: an integer message tag.
 *
 * Sends stored message to the specified destination with the specified tag.
 */
void vsg_packed_msg_send (VsgPackedMsg *pm, gint dst, gint tag)
{
  gint ierr;

  _trace_write_msg_send (pm, "send", dst, tag);

  ierr = MPI_Send (pm->buffer, pm->position, MPI_PACKED, dst, tag,
                   pm->communicator);

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_bsend:
 * @pm: a #VsgPackedMsg.
 * @dst: the destination task id.
 * @tag: an integer message tag.
 *
 * Sends stored message to the specified destination with the specified tag.
 */
void vsg_packed_msg_bsend (VsgPackedMsg *pm, gint dst, gint tag)
{
  gint ierr;

  _trace_write_msg_send (pm, "bsend", dst, tag);

  ierr = MPI_Bsend (pm->buffer, pm->position, MPI_PACKED, dst, tag,
                    pm->communicator);

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_rsend:
 * @pm: a #VsgPackedMsg.
 * @dst: the destination task id.
 * @tag: an integer message tag.
 *
 * Sends stored message to the specified destination with the specified tag.
 */
void vsg_packed_msg_rsend (VsgPackedMsg *pm, gint dst, gint tag)
{
  gint ierr;

  _trace_write_msg_send (pm, "rsend", dst, tag);

  ierr = MPI_Rsend (pm->buffer, pm->position, MPI_PACKED, dst, tag,
                    pm->communicator);

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_ssend:
 * @pm: a #VsgPackedMsg.
 * @dst: the destination task id.
 * @tag: an integer message tag.
 *
 * Sends stored message to the specified destination with the specified tag.
 */
void vsg_packed_msg_ssend (VsgPackedMsg *pm, gint dst, gint tag)
{
  gint ierr;

  _trace_write_msg_send (pm, "ssend", dst, tag);

  ierr = MPI_Ssend (pm->buffer, pm->position, MPI_PACKED, dst, tag,
                    pm->communicator);

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_isend:
 * @pm: a #VsgPackedMsg.
 * @dst: the destination task id.
 * @tag: an integer message tag.
 * @request: the corresponding request object
 *
 * Sends stored message to the specified destination with the specified tag in
 * a non blocking mode. @request is provided for output.
 */
void vsg_packed_msg_isend (VsgPackedMsg *pm, gint dst, gint tag,
                           MPI_Request *request)
{
  gint ierr;

  _trace_write_msg_send (pm, "isend", dst, tag);

  ierr = MPI_Isend (pm->buffer, pm->position, MPI_PACKED, dst, tag,
                    pm->communicator, request);

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_ibsend:
 * @pm: a #VsgPackedMsg.
 * @dst: the destination task id.
 * @tag: an integer message tag.
 * @request: the corresponding request object
 *
 * Sends stored message to the specified destination with the specified tag in
 * a non blocking mode. @request is provided for output.
 */
void vsg_packed_msg_ibsend (VsgPackedMsg *pm, gint dst, gint tag,
                            MPI_Request *request)
{
  gint ierr;

  _trace_write_msg_send (pm, "ibsend", dst, tag);

  ierr = MPI_Ibsend (pm->buffer, pm->position, MPI_PACKED, dst, tag,
                     pm->communicator, request);

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_irsend:
 * @pm: a #VsgPackedMsg.
 * @dst: the destination task id.
 * @tag: an integer message tag.
 * @request: the corresponding request object
 *
 * Sends stored message to the specified destination with the specified tag in
 * a non blocking mode. @request is provided for output.
 */
void vsg_packed_msg_irsend (VsgPackedMsg *pm, gint dst, gint tag,
                            MPI_Request *request)
{
  gint ierr;

  _trace_write_msg_send (pm, "irsend", dst, tag);

  ierr = MPI_Irsend (pm->buffer, pm->position, MPI_PACKED, dst, tag,
                     pm->communicator, request);

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_issend:
 * @pm: a #VsgPackedMsg.
 * @dst: the destination task id.
 * @tag: an integer message tag.
 * @request: the corresponding request object
 *
 * Sends stored message to the specified destination with the specified tag in
 * a non blocking mode. @request is provided for output.
 */
void vsg_packed_msg_issend (VsgPackedMsg *pm, gint dst, gint tag,
                            MPI_Request *request)
{
  gint ierr;

  _trace_write_msg_send (pm, "issend", dst, tag);

  ierr = MPI_Issend (pm->buffer, pm->position, MPI_PACKED, dst, tag,
                     pm->communicator, request);

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_wait:
 * @pm: a #VsgPackedMsg.
 * @request: the corresponding request object
 *
 * Waits until the message corresponding to @request is completed
 */
void vsg_packed_msg_wait (VsgPackedMsg *pm, MPI_Request *request)
{
  gint ierr;
  MPI_Status status;

  ierr = MPI_Wait (request, &status);

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_recv:
 * @pm: a #VsgPackedMsg.
 * @src: the source task id. Can be %MPI_ANY_SOURCE.
 * @tag: an integer message tag. Can be %MPI_ANY_TAG.
 *
 * Receives a message from source @src with @tag message tag and stores it in
 * @pm. any previously stored data will be lost.
 */
void vsg_packed_msg_recv (VsgPackedMsg *pm, gint src, gint tag)
{
  MPI_Status status;
  gint ierr;
  gint rsize = 0;

  g_assert (pm->own_buffer == TRUE);

  MPI_Probe (src, tag, pm->communicator, &status);

  MPI_Get_count (&status, MPI_PACKED, &rsize);

  pm->buffer = g_realloc (pm->buffer, rsize);
  pm->size = rsize;

  ierr = MPI_Recv (pm->buffer, rsize, MPI_PACKED, src, tag,
                   pm->communicator, &status);

  _trace_write_msg_recv (pm, "recv", src, tag);

  pm->position = _PM_BEGIN_POS;

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_bcast:
 * @pm: a #VsgPackedMsg.
 * @src: the source task id. Can be %MPI_ANY_SOURCE.
 * @tag: an integer message tag. Can be %MPI_ANY_TAG.
 *
 * Performs a MPI_Bcast on @pm. @pm must be of the same size across
 * all processes (ie. similar calls to vsg_packed_msg_send_append()
 * must have been previously issued on every processor).
 */
void vsg_packed_msg_bcast (VsgPackedMsg *pm, gint src)
{
  gint ierr;
  gint rk;

  g_assert (pm->own_buffer == TRUE);

  MPI_Comm_rank (pm->communicator, &rk);

  if (rk == src)
    _trace_write_msg_send (pm, "bcast-send", src, -1);

  ierr = MPI_Bcast (pm->buffer, pm->position, MPI_PACKED, src,
                    pm->communicator);

  if (rk != src)
    _trace_write_msg_recv (pm, "bcast-recv", src, -1);

  pm->position = _PM_BEGIN_POS;

  if (ierr != MPI_SUCCESS) vsg_mpi_error_output (ierr);
}

/**
 * vsg_packed_msg_recv_new:
 * @comm: a MPI communicator
 * @src: the source task id. Can be %MPI_ANY_SOURCE.
 * @tag: an integer message tag. Can be %MPI_ANY_TAG.
 *
 * Receives a message from source @src with @tag message tag and stores it in
 * a newly allocated #VsgPackedMsg.
 *
 * Returns: the received message.
 */
VsgPackedMsg * vsg_packed_msg_recv_new (MPI_Comm comm, gint src, gint tag)
{
  VsgPackedMsg *ret = vsg_packed_msg_new (comm);

  vsg_packed_msg_recv (ret, src, tag);

  return ret;
}

/**
 * vsg_packed_msg_drop_buffer:
 * @pm: a #VsgPackedMsg.
 *
 * Drops data stored in @pm buffer.
 */
void vsg_packed_msg_drop_buffer (VsgPackedMsg *pm)
{
  g_return_if_fail (pm != NULL);

  if (pm->buffer != NULL && pm->own_buffer)
    g_free (pm->buffer);

  pm->buffer = NULL;
  pm->position = 0;
  pm->size = 0;
  pm->own_buffer = TRUE;
}

/**
 * vsg_packed_msg_free:
 * @pm: a #VsgPackedMsg.
 *
 * deallocates @pm and all data associated with it.
 */
void vsg_packed_msg_free (VsgPackedMsg *pm)
{
  g_return_if_fail (pm != NULL);

  vsg_packed_msg_drop_buffer (pm);

  g_free (pm);
}
