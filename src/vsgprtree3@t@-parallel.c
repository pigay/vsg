#include "vsgvector3@t@-inline.h"

#include "vsgprtree3@t@-parallel.h"
#include "vsgprtree3@t@-private.h"

#include "vsgcommbuffer.h"
#include "vsgcommbuffer-private.h"

#include "string.h"

#define DIM VSG_DIM3
#define CN VSG_PRTREE3_CN
#define HALF_CN VSG_PRTREE3_HALF_CN

/**
 * VsgPRTree3@t@DistributionFunc:
 * @node_info: a #VsgPRTree3@t@NodeInfo
 * @user_data: user provided arbitrary data
 *
 * allows user to choose which processor number a tree node
 * presented with @node_info is given to.
 *
 * WARNING: only available when VSG_HAVE_MPI is defined.
 *
 * Returns: destination processor number
 */

typedef struct _ForeachPackData ForeachPackData;

struct _ForeachPackData
{
  VsgParallelMigrateVTable *migrate;

  VsgMigrableAllocDataFunc alloc;
  gpointer alloc_data;

  VsgMigrableDestroyDataFunc destroy;
  gpointer destroy_data;

  gboolean resident;

  VsgPackedMsg *msg;
};

#define FPD_STATIC_INIT(vtable, msg_type, resident, msg) {     \
    &((vtable)->msg_type),                                              \
      (vtable)->alloc,                                                  \
      (vtable)->alloc_data,                                             \
      (vtable)->destroy,                                                \
      (vtable)->destroy_data,                                           \
      (resident),                                                       \
      (msg),                                                            \
      }

#define FPD_DATA_MIGRATE(pconfig, msg)                 \
 FPD_STATIC_INIT (&(pconfig)->node_data, migrate, TRUE, msg)

#define FPD_DATA_VISIT_FORWARD(pconfig, msg)                   \
  FPD_STATIC_INIT (&(pconfig)->node_data, visit_forward, FALSE, msg)

#define FPD_DATA_VISIT_BACKWARD(pconfig, msg)                  \
  FPD_STATIC_INIT (&(pconfig)->node_data, visit_backward, FALSE, msg)

#define FPD_POINT_MIGRATE(pconfig, msg)                 \
  FPD_STATIC_INIT (&(pconfig)->point, migrate, TRUE, msg)

#define FPD_POINT_VISIT_FORWARD(pconfig, msg)                   \
  FPD_STATIC_INIT (&(pconfig)->point, visit_forward, FALSE, msg)

#define FPD_POINT_VISIT_BACKWARD(pconfig, msg)                  \
  FPD_STATIC_INIT (&(pconfig)->point, visit_backward, FALSE, msg)

#define FPD_REGION_MIGRATE(pconfig, msg)                \
  FPD_STATIC_INIT (&(pconfig)->region, migrate, TRUE, msg)

#define FPD_REGION_VISIT_FORWARD(pconfig, msg)                  \
  FPD_STATIC_INIT (&(pconfig)->region, visit_forward, FALSE, msg)

#define FPD_REGION_VISIT_BACKWARD(pconfig, msg)                 \
  FPD_STATIC_INIT (&(pconfig)->region, visit_backward, FALSE, msg)

static void _foreach_pack (gpointer obj, ForeachPackData *fpd)
{
  fpd->migrate->pack (obj, fpd->msg, fpd->migrate->pack_data);
}

static void _foreach_pack_and_destroy (gpointer obj, ForeachPackData *fpd)
{
  fpd->migrate->pack (obj, fpd->msg, fpd->migrate->pack_data);
  fpd->destroy (obj, fpd->resident, fpd->destroy_data);
}

static gpointer _alloc_and_unpack (ForeachPackData *fpd)
{
  gpointer obj = fpd->alloc (fpd->resident, fpd->alloc_data);

  fpd->migrate->unpack (obj, fpd->msg, fpd->migrate->unpack_data);

  return obj;
}

static GSList *_alloc_and_unpack_list (ForeachPackData *fpd, gint length)
{
  GSList *objlist = NULL;
  gint i;

  for (i=0; i<length; i ++)
    {
      gpointer obj = _alloc_and_unpack (fpd);

      /* Don't use g_slist_prepend here since it is mandatory to keep
         point list in the same order between fw and bw transfers */
      objlist  = g_slist_append (objlist, obj);
    }

  return objlist;
}

static void _unpack_and_reduce (gpointer obj, ForeachPackData *fpd,
                                gpointer tmp_obj)
{
  fpd->migrate->unpack (tmp_obj, fpd->msg, fpd->migrate->unpack_data);
  fpd->migrate->reduce (tmp_obj, obj, fpd->migrate->reduce_data);
}

static void _unpack_and_reduce_list (GSList *objlist, ForeachPackData *fpd,
                                     gpointer tmp_obj)
{
  while (objlist != NULL)
    {
      gpointer obj = objlist->data;

      _unpack_and_reduce (obj, fpd, tmp_obj);

      objlist = g_slist_next (objlist);
    }
}

typedef struct _NodePackData NodePackData;

struct _NodePackData
{
  ForeachPackData data_fpd;
  ForeachPackData pt_fpd;
  ForeachPackData rg_fpd;
};

#define NPD_STATIC_INIT(pconfig, msg_type, resident, msg) {             \
    FPD_STATIC_INIT(&(pconfig)->node_data, msg_type, resident, msg),    \
      FPD_STATIC_INIT(&(pconfig)->point, msg_type, resident, msg),      \
      FPD_STATIC_INIT(&(pconfig)->region, msg_type, resident, msg),     \
}

#define NPD_MIGRATE(pconfig, msg)               \
  NPD_STATIC_INIT (pconfig, migrate, TRUE, msg)

#define NPD_VISIT_FORWARD(pconfig, msg)                 \
  NPD_STATIC_INIT (pconfig, visit_forward, FALSE, msg)

#define NPD_VISIT_BACKWARD(pconfig, msg)                \
  NPD_STATIC_INIT (pconfig, visit_backward, FALSE, msg)

static void _migrate_node_pack_header (VsgPRTree3@t@NodeInfo *info,
                                       VsgPackedMsg *msg, gint dst,
                                       VsgPRTree3@t@Config *config)
{
  vsg_packed_msg_send_append (msg, &info->id, 1, VSG_MPI_TYPE_PRTREE_KEY3@T@);
  vsg_packed_msg_send_append (msg, &dst, 1, MPI_INT);
}

static void _node_pack (VsgPRTree3@t@Node *node, NodePackData *npd)
{
  VsgPackedMsg *msg = npd->pt_fpd.msg;                        
  gint datapresent, point_count, region_count;

  datapresent = npd->data_fpd.migrate->pack != NULL && node->user_data != NULL;
  point_count = 0;
  if (PRTREE3@T@NODE_ISLEAF (node) && npd->pt_fpd.migrate->pack != NULL)
    point_count = node->point_count;

  region_count = 0;
  if (npd->rg_fpd.migrate->pack != NULL)
    region_count = g_slist_length (node->region_list); /* use region number
                                                        * at this level only.
                                                        */

  /* pack message definition: */
  vsg_packed_msg_send_append (msg, &datapresent, 1, MPI_INT);
  vsg_packed_msg_send_append (msg, &point_count, 1, MPI_INT);
  vsg_packed_msg_send_append (msg, &region_count, 1, MPI_INT);

  /* pack the node data */
  if (datapresent && npd->data_fpd.migrate->pack != NULL)
    {
      _foreach_pack (node->user_data, &npd->data_fpd);
    }

  /* pack the points */
  if (point_count > 0 && npd->pt_fpd.migrate->pack != NULL)
    {
      g_slist_foreach (PRTREE3@T@NODE_LEAF (node).point,
                       (GFunc) _foreach_pack,
                       &npd->pt_fpd);
    }

  /* pack the regions */
  if (region_count > 0 && npd->rg_fpd.migrate->pack != NULL)
    {
      g_slist_foreach (node->region_list, (GFunc) _foreach_pack, &npd->rg_fpd);
    }
}

static void _node_pack_and_destroy (VsgPRTree3@t@Node *node, NodePackData *npd)
{
  VsgPackedMsg *msg = npd->pt_fpd.msg;                        
  gint datapresent, point_count, region_count;

  datapresent = npd->data_fpd.migrate->pack != NULL && node->user_data != NULL;
  point_count = 0;
  if (npd->pt_fpd.migrate->pack != NULL && PRTREE3@T@NODE_ISLEAF (node))
    point_count = node->point_count;

  region_count = 0;
  if (npd->rg_fpd.migrate->pack != NULL)
    region_count = g_slist_length (node->region_list); /* use region number
                                                        * at this level only.
                                                        */

  /* pack message definition: */
  vsg_packed_msg_send_append (msg, &datapresent, 1, MPI_INT);
  vsg_packed_msg_send_append (msg, &point_count, 1, MPI_INT);
  vsg_packed_msg_send_append (msg, &region_count, 1, MPI_INT);

  /* pack the node data */
  if (datapresent)
    {
      if (npd->data_fpd.migrate->pack != NULL)
        _foreach_pack_and_destroy (node->user_data, &npd->data_fpd);

      node->user_data = NULL;
    }

  /* pack the points */
  if (point_count > 0)
    {
      if (npd->pt_fpd.migrate->pack != NULL)
        g_slist_foreach (PRTREE3@T@NODE_LEAF (node).point,
                         (GFunc) _foreach_pack_and_destroy,
                         &npd->pt_fpd);

      g_slist_free (PRTREE3@T@NODE_LEAF (node).point);
      PRTREE3@T@NODE_LEAF (node).point = NULL;
      node->point_count = 0;
    }

  /* pack the regions */
  if (region_count > 0)
    {
      if (npd->rg_fpd.migrate->pack != NULL)
        g_slist_foreach (node->region_list, (GFunc) _foreach_pack_and_destroy,
                         &npd->rg_fpd);

      g_slist_free (node->region_list);
      node->region_list = NULL;
      node->region_count = 0;
    }
}

static void _node_unpack (VsgPRTree3@t@Node *node, NodePackData *npd)
{
  VsgPackedMsg *msg = npd->pt_fpd.msg;
  gint datapresent, point_count, region_count;

  /* load node contents */

  vsg_packed_msg_recv_read (msg, &datapresent, 1, MPI_INT);
  vsg_packed_msg_recv_read (msg, &point_count, 1, MPI_INT);
  vsg_packed_msg_recv_read (msg, &region_count, 1, MPI_INT);

  if (datapresent)
    {
      if (node->user_data != NULL)
        {
          npd->data_fpd.destroy (node->user_data, npd->data_fpd.resident,
                                 npd->data_fpd.destroy_data);
        }
      else
        {
          // *** PRIVATE-REMOTE
          g_assert (PRTREE3@T@NODE_IS_PRIVATE_REMOTE (node));
        }

      if (npd->data_fpd.migrate->unpack != NULL)
        node->user_data = _alloc_and_unpack (&npd->data_fpd);
    }

  if (point_count > 0)
    {
      g_assert (PRTREE3@T@NODE_ISLEAF (node));

      if (npd->pt_fpd.migrate->unpack != NULL)
        PRTREE3@T@NODE_LEAF (node).point = _alloc_and_unpack_list (&npd->pt_fpd,
                                                                   point_count);
      else
        PRTREE3@T@NODE_LEAF (node).point = NULL;

      node->point_count = point_count;
    }

  if (region_count > 0)
    {
      if (npd->rg_fpd.migrate->unpack != NULL)
        node->region_list = _alloc_and_unpack_list (&npd->rg_fpd, region_count);
      else
        node->region_list = NULL;
      node->region_count = region_count;
    }
}

static void _node_unpack_and_reduce (VsgPRTree3@t@Node *node, NodePackData *npd,
                                     VsgNFConfig3@t@ *nfc)
{
  VsgPackedMsg *msg = npd->pt_fpd.msg;
  gint datapresent, point_count, region_count;

  /* load node contents */
  vsg_packed_msg_recv_read (msg, &datapresent, 1, MPI_INT);
  vsg_packed_msg_recv_read (msg, &point_count, 1, MPI_INT);
  vsg_packed_msg_recv_read (msg, &region_count, 1, MPI_INT);

  if (datapresent)
    {
      if (npd->data_fpd.migrate->unpack != NULL)
        _unpack_and_reduce (node->user_data, &npd->data_fpd,
                            nfc->tmp_node_data);
    }

  if (point_count > 0)
    {
      g_assert (PRTREE3@T@NODE_ISLEAF (node));
      g_assert (point_count == node->point_count);

      if (npd->pt_fpd.migrate->unpack != NULL)
        _unpack_and_reduce_list (PRTREE3@T@NODE_LEAF (node).point, &npd->pt_fpd,
                                 nfc->tmp_point);

    }

  if (region_count > 0)
    {
      g_assert (region_count == g_slist_length (node->region_list));

      if (npd->rg_fpd.migrate->unpack != NULL)
        _unpack_and_reduce_list (node->region_list, &npd->rg_fpd,
                                 nfc->tmp_region);
    }
}

/**
 * vsg_prtree3@t@_set_parallel: 
 * @tree: a #VsgPRTree3@t@
 * @pconfig: the #VsgPRTreeParallelConfig to use with @tree
 *
 * performs necessary transformations and communications to set @tree
 * in a parallel state. This means points and region merging to
 * processor #0.
 *
 * WARNING: only available when VSG_HAVE_MPI is defined.
 *
 * NOTE: this is a collective communication function.
 */
void vsg_prtree3@t@_set_parallel (VsgPRTree3@t@ *tree,
                                  VsgPRTreeParallelConfig *pconfig)
{
  MPI_Comm comm;
  VsgParallelVTable *pt_vtable;
  VsgParallelVTable *rg_vtable;
  
  VsgVector3@t@ bounds[2];
  gint rk, sz;

  g_return_if_fail (tree != NULL);
  g_return_if_fail (pconfig != NULL);

  comm = pconfig->communicator;
  pt_vtable = &pconfig->point;
  rg_vtable = &pconfig->region;
  
  MPI_Comm_rank (comm, &rk);
  MPI_Comm_size (comm, &sz);

  if (comm != tree->config.parallel_config.communicator)
    {
      if (rk != 0)
        {
          gint cnt;
          VsgPackedMsg pt_send = VSG_PACKED_MSG_STATIC_INIT (comm);
          VsgPackedMsg rg_send = VSG_PACKED_MSG_STATIC_INIT (comm);
          ForeachPackData pt_fpd =
            FPD_POINT_MIGRATE (pconfig, &pt_send);
          ForeachPackData rg_fpd =
            FPD_REGION_MIGRATE (pconfig, &rg_send);

          /* send points to 0 */
          cnt = vsg_prtree3@t@_point_count (tree);
          vsg_packed_msg_send_append (&pt_send, &cnt, 1, MPI_INT);
          vsg_prtree3@t@_foreach_point (tree, (GFunc) _foreach_pack_and_destroy,
                                        &pt_fpd);

          vsg_packed_msg_send (&pt_send, 0, 1001);
          vsg_packed_msg_drop_buffer (&pt_send);

          /* get 0's new bounds */
          MPI_Bcast (bounds, 2, VSG_MPI_TYPE_VECTOR3@T@, 0, comm);

/*       g_printerr ("%d: bounds", rk); */
/*       vsg_vector3d_write (&bounds[0], stderr); */
/*       vsg_vector3d_write (&bounds[1], stderr); */
/*       g_printerr ("\n"); */

          /* send regions to 0 */
          cnt = vsg_prtree3@t@_region_count (tree);
          vsg_packed_msg_send_append (&rg_send, &cnt, 1, MPI_INT);
          vsg_prtree3@t@_foreach_region (tree,
                                         (GFunc) _foreach_pack_and_destroy,
                                         &rg_fpd);

          vsg_packed_msg_send (&rg_send, 0, 1002);
          vsg_packed_msg_drop_buffer (&rg_send);

          /* Transform root node to an empty remote */
          vsg_prtree3@t@node_free (tree->node, &tree->config);

          tree->node = vsg_prtree3@t@node_alloc_no_data (&bounds[0],
                                                         &bounds[1]);

          // *** PRIVATE-REMOTE
          tree->node->parallel_status.storage = VSG_PARALLEL_PRIVATE_REMOTE;
          tree->node->parallel_status.proc = 0;
        }
      else
        {
          gint src;
          VsgPackedMsg pt_recv = VSG_PACKED_MSG_STATIC_INIT (comm);
          VsgPackedMsg rg_recv = VSG_PACKED_MSG_STATIC_INIT (comm);

          /* receive other procs points */
          for (src=1; src<sz; src++)
            {
              gint i, cnt;

              vsg_packed_msg_recv (&pt_recv, src, 1001);
              vsg_packed_msg_recv_read (&pt_recv, &cnt, 1, MPI_INT);

              for (i=0; i<cnt; i++)
                {
                  VsgPoint3 *pt = pt_vtable->alloc (TRUE,
                                                    pt_vtable->alloc_data);

                  pt_vtable->migrate.unpack (pt, &pt_recv,
                                             pt_vtable->migrate.unpack_data);
                  vsg_prtree3@t@_insert_point (tree, pt);
                }

              vsg_packed_msg_drop_buffer (&pt_recv);
            }

          /* communicate new tree bounds to other processors */
          vsg_prtree3@t@_get_bounds (tree, &bounds[0], &bounds[1]);
          MPI_Bcast (bounds, 2, VSG_MPI_TYPE_VECTOR3@T@, 0, comm);

/*       g_printerr ("%d: bounds", rk); */
/*       vsg_vector3d_write (&bounds[0], stderr); */
/*       vsg_vector3d_write (&bounds[1], stderr); */
/*       g_printerr ("\n"); */

          /* receive other procs regions */
          for (src=1; src<sz; src++)
            {
              gint i, cnt;

              vsg_packed_msg_recv (&rg_recv, src, 1002);
              vsg_packed_msg_recv_read (&rg_recv, &cnt, 1, MPI_INT);

              for (i=0; i<cnt; i++)
                {
                  VsgRegion3 rg = rg_vtable->alloc (TRUE,
                                                    rg_vtable->alloc_data);

                  rg_vtable->migrate.unpack (rg, &rg_recv,
                                             rg_vtable->migrate.unpack_data);
                  vsg_prtree3@t@_insert_region (tree, rg);
                }

              vsg_packed_msg_drop_buffer (&rg_recv);
            }

        }
    }

  vsg_prtree3@t@_set_node_data_vtable (tree, &pconfig->node_data);

  memcpy (&tree->config.parallel_config, pconfig,
          sizeof (VsgPRTreeParallelConfig));

}

/**
 * vsg_prtree3@t@_get_parallel:
 * @tree: a #VsgPRTree3@t@
 * @pconfig: the #VsgPRTreeParallelConfig to copy to
 *
 * inquires the parallel configuration of @tree and copies it to @pconfig.
 *
 * WARNING: only available when VSG_HAVE_MPI is defined.
 */
void vsg_prtree3@t@_get_parallel (VsgPRTree3@t@ *tree,
                                  VsgPRTreeParallelConfig *pconfig)
{
  g_return_if_fail (tree != NULL);
  g_return_if_fail (pconfig != NULL);

  memcpy (pconfig, &tree->config.parallel_config,
          sizeof (VsgPRTreeParallelConfig));
}


/**
 * vsg_prtree3@t@_get_communicator:
 * 
 *
 * Returns the #MPI_Communicator that shares this particular PRTree
 */
MPI_Comm
vsg_prtree3@t@_get_communicator (VsgPRTree3@t@ *tree)
{
  g_return_val_if_fail (tree != NULL, NULL);

  return vsg_prtree_parallel_config_get_communicator (&tree->config.parallel_config);
}


/* selector function used in traverse_custom_internal to avoid
   traversing all local nodes */
static vsgrloc3 _selector_skip_local_nodes (VsgRegion3 *selector,
                                            VsgPRTree3@t@NodeInfo *node_info,
                                            gpointer data)
{
  // *** PRIVATE-LOCAL
  if (VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_LOCAL (node_info)) return 0x0;

  return VSG_RLOC3_MASK;
}

typedef struct _MigrateData MigrateData;
struct _MigrateData
{
  VsgPRTreeParallelConfig *pconfig;
  gint rk;
  VsgCommBuffer *cb;
};

static void _migrate_traverse_point_send (VsgPRTree3@t@Node *node,
                                          VsgPRTree3@t@NodeInfo *node_info,
                                          MigrateData *md)
{
  /* check for points in remote nodes */
  // *** PRIVATE-REMOTE
  if (PRTREE3@T@NODE_IS_PRIVATE_REMOTE (node) &&
      node_info->parallel_status.proc != md->rk)
    {
      VsgPackedMsg *pm = 
        vsg_comm_buffer_get_send_buffer (md->cb, node->parallel_status.proc);
      ForeachPackData fpd = FPD_POINT_MIGRATE (md->pconfig, pm);

      g_slist_foreach (PRTREE3@T@NODE_LEAF (node).point,
                       (GFunc) _foreach_pack_and_destroy, &fpd);
      g_slist_free (PRTREE3@T@NODE_LEAF (node).point);
      PRTREE3@T@NODE_LEAF (node).point = NULL;
    }
}

static void _migrate_traverse_region_send (VsgPRTree3@t@Node *node,
                                           VsgPRTree3@t@NodeInfo *node_info,
                                           MigrateData *md)
{
  /* check for regions in remote nodes */
  // *** PRIVATE-REMOTE
  if (PRTREE3@T@NODE_IS_PRIVATE_REMOTE (node) &&
      node_info->parallel_status.proc != md->rk)
    {
      VsgPackedMsg *pm = 
        vsg_comm_buffer_get_send_buffer (md->cb, node->parallel_status.proc);
      ForeachPackData fpd = FPD_REGION_MIGRATE (md->pconfig, pm);

      g_slist_foreach (node->region_list, (GFunc) _foreach_pack_and_destroy,
                       &fpd);
      g_slist_free (node->region_list);
      node->region_list = NULL;
    }
}

static void _copy_node (VsgPRTree3@t@Node *dst, VsgPRTree3@t@Node *src)
{
  memcpy (dst, src, sizeof (VsgPRTree3@t@Node));
}

typedef void (*VsgPRTree3@t@NodeFunc) (VsgPRTree3@t@Node *node,
                                       gpointer user_data);

static void _node_insert_child (VsgPRTree3@t@Node *node,
                                const VsgPRTree3@t@Config *config,
                                VsgParallelStorage storage,
                                gint dst,
                                VsgPRTreeKey3@t@ key,
                                VsgPRTree3@t@NodeFunc replace_func,
                                gpointer replace_data);

/**
 * vsg_prtree3@t@_migrate_flush:
 * @tree: a #VsgPRTree3@t@
 *
 * verifies if points or region were inserted in remote regions of the
 * tree and performs all needed communications (migrations) to ensure
 * that all those nodes and regions become resident of their correct
 * node (ie: processor which owns their home node).
 *
 * NOTE: this is a collective communication function.
 *
 * WARNING: only available when VSG_HAVE_MPI is defined.
 */
void vsg_prtree3@t@_migrate_flush (VsgPRTree3@t@ *tree)
{
  VsgPRTreeParallelConfig *pconfig = &tree->config.parallel_config;
  MPI_Comm comm;
  VsgCommBuffer *cb;
  VsgPackedMsg pm = VSG_PACKED_MSG_NULL;
  MigrateData md;
  VsgParallelVTable *pt_vtable;
  VsgParallelVTable *rg_vtable;
  gint src, rk, sz;
  VsgPRTreeKey3@t@ extk;
  VsgVector3@t@ lbound, ubound;

  g_return_if_fail (tree != NULL);
  comm = pconfig->communicator;
  g_return_if_fail (comm != MPI_COMM_NULL);
  
  MPI_Comm_rank (comm, &rk);
  MPI_Comm_size (comm, &sz);

  cb = vsg_comm_buffer_new (comm);
  vsg_packed_msg_init (&pm, comm);

  pt_vtable = &pconfig->point;
  rg_vtable = &pconfig->region;

  md.pconfig = pconfig;
  md.rk = rk;
  md.cb = cb;

  /* send exterior points to proc 0 */
  if (rk != 0 && tree->pending_exterior_points != NULL)
    {
      VsgPackedMsg *pmext = 
        vsg_comm_buffer_get_send_buffer (cb, 0);
      ForeachPackData fpd = FPD_POINT_MIGRATE (pconfig, pmext);

      g_slist_foreach (tree->pending_exterior_points,
                       (GFunc) _foreach_pack_and_destroy,
                       &fpd);

      g_slist_free (tree->pending_exterior_points);
      tree->pending_exterior_points = NULL;
    }

  /* send the pending VsgPoint to their remote location */

  vsg_prtree3@t@_traverse_custom_internal (tree, G_PRE_ORDER,
                                           (VsgRegion3@t@InternalLocDataFunc)
                                           _selector_skip_local_nodes,
                                           NULL, NULL,
                                           (VsgPRTree3@t@InternalFunc)
                                           _migrate_traverse_point_send,
                                           &md);

  vsg_comm_buffer_share (cb);

  /* get local VsgPoint from remote processors */

  for (src=0; src<sz; src++)
    {
      VsgPackedMsg *recv;

      if (src == rk) continue;

      recv = vsg_comm_buffer_get_recv_buffer (cb, src);

      /* Reception buffers are allocated at the exact size of the message.
       * We can therefore use the advance of successive unpack to control the
       * number of received VsgPoint.
       */
      while (recv->position < recv->size)
        {
          VsgPoint3 *pt = pt_vtable->alloc (TRUE, pt_vtable->alloc_data);

          pt_vtable->migrate.unpack (pt, recv, pt_vtable->migrate.unpack_data);

          vsg_prtree3@t@_insert_point (tree, pt);
          
        }

      vsg_comm_buffer_drop_recv_buffer (cb, src);
    }

  /* build new tree from gathered exterior points */

  extk = vsg_prtree_key3@t@_root;

  if (rk == 0)
    {
      /* now all exterior points are stored in tree->pending_exterior_points */
      if (tree->pending_exterior_points)
        {
          GSList *ext_list = tree->pending_exterior_points;

          /* extend tree with local leaves to absorb new bounds */
          while (ext_list != NULL)
            {
              vsg_prtree3@t@_bounds_extend (tree, ext_list->data, &extk);

              ext_list = g_slist_next (ext_list);
            }

          /* insert exterior points in local leaves */
          vsg_prtree3@t@node_insert_point_list(tree->node,
                                               tree->pending_exterior_points,
                                               &tree->config);

          tree->pending_exterior_points = NULL;

          /* set new tree bounds to communicate */
          vsg_prtree3@t@_get_bounds (tree, &lbound, &ubound);

/*           g_printerr ("%d: ext key ", rk); */
/*           vsg_prtree_key3@t@_write (&extk, stderr); */
/*           g_printerr ("\n"); */
/*           g_printerr ("%d: bounds ", rk); */
/*           vsg_vector3@t@_write (&lbound, stderr); */
/*           g_printerr (" " ); */
/*           vsg_vector3@t@_write (&ubound, stderr); */
/*           g_printerr ("\n"); */

        }
    }

  /* pack tree extension msg even on non-root procs to get msg size */
  vsg_packed_msg_send_append (&pm, &lbound, 1, VSG_MPI_TYPE_VECTOR3@T@);
  vsg_packed_msg_send_append (&pm, &ubound, 1, VSG_MPI_TYPE_VECTOR3@T@);
  vsg_packed_msg_send_append (&pm, &extk, 1, VSG_MPI_TYPE_PRTREE_KEY3@T@);

  /* proc 0 tells new tree bounds and old root position */
  vsg_packed_msg_bcast (&pm, 0);

  if (rk != 0)
    {
      /* unpack extension definition */
      vsg_packed_msg_recv_read (&pm, &lbound, 1, VSG_MPI_TYPE_VECTOR3@T@);
      vsg_packed_msg_recv_read (&pm, &ubound, 1, VSG_MPI_TYPE_VECTOR3@T@);
      vsg_packed_msg_recv_read (&pm, &extk, 1, VSG_MPI_TYPE_PRTREE_KEY3@T@);

      if (extk.depth > 0)
        {
          VsgPRTree3@t@Node *new_root;

          /* allocate new root node for the tree */
          new_root = vsg_prtree3@t@node_alloc_no_data (&lbound, &ubound);

          /* exterior points are stored at proc 0 */
          // *** PRIVATE-REMOTE
          new_root->parallel_status.storage = VSG_PARALLEL_PRIVATE_REMOTE;
          new_root->parallel_status.proc = 0;

          /* place old root inside new root tree */
          _node_insert_child (new_root, &tree->config,
                              tree->node->parallel_status.storage,
                              tree->node->parallel_status.proc,
                              extk,
                              (VsgPRTree3@t@NodeFunc) _copy_node,
                              tree->node);

          /* destroy old root */
          vsg_prtree3@t@node_dealloc (tree->node);

          /* place new root */
          tree->node = new_root;

/*           vsg_prtree3@t@_get_bounds (tree, &lbound, &ubound); */
/*           g_printerr ("%d: ext key ", rk); */
/*           vsg_prtree_key3@t@_write (&extk, stderr); */
/*           g_printerr ("\n"); */
/*           g_printerr ("%d: bounds ", rk); */
/*           vsg_vector3@t@_write (&lbound, stderr); */
/*           g_printerr (" " ); */
/*           vsg_vector3@t@_write (&ubound, stderr); */
/*           g_printerr ("\n"); */

        }

      vsg_packed_msg_reset (&pm);
    }

  if (rg_vtable->migrate.pack != NULL)
    {
      ForeachPackData fpd = FPD_REGION_MIGRATE (pconfig, &pm);
      /* send the pending shared VsgRegion to every other processor */
      vsg_packed_msg_init (&pm, comm);

/*       g_printerr ("%d: pending shared regions\n", rk); */

      g_slist_foreach (tree->pending_shared_regions,
                       (GFunc) _foreach_pack, &fpd);

      g_slist_free (tree->pending_shared_regions);
      tree->pending_shared_regions = NULL;

      vsg_comm_buffer_set_bcast (cb, &pm);

      vsg_comm_buffer_share (cb);

      /* send the pending (in "remote" nodes) VsgRegion to their destination */
      md.rk = rk;
      md.cb = cb;

      vsg_prtree3@t@_traverse_custom_internal (tree, G_PRE_ORDER,
                                               (VsgRegion3@t@InternalLocDataFunc)
                                               _selector_skip_local_nodes,
                                               NULL, NULL,
                                               (VsgPRTree3@t@InternalFunc)
                                               _migrate_traverse_region_send,
                                               &md);

      vsg_comm_buffer_share (cb);

      /* get local VsgRegion from remote processors */

      for (src=0; src<sz; src++)
        {
          VsgPackedMsg *recv;

          if (src == rk) continue;

          recv = vsg_comm_buffer_get_recv_buffer (cb, src);

          /* Reception buffers are allocated at the exact size of the message.
           * We can therefore use the advance of successive unpack to control
           * the number of received VsgRegion.
           */
          while (recv->position < recv->size)
            {
              VsgRegion3 *rg = rg_vtable->alloc (TRUE, rg_vtable->alloc_data);

              rg_vtable->migrate.unpack (rg, recv,
                                         rg_vtable->migrate.unpack_data);
              vsg_prtree3@t@_insert_region (tree, rg);
            }

          vsg_comm_buffer_drop_recv_buffer (cb, src);
        }
    }

  vsg_packed_msg_drop_buffer (&pm);

  vsg_comm_buffer_free (cb);

  /* remote trees depth may have changed */
  tree->config.remote_depth_dirty = TRUE;
}

/* For fixing point and region counts in a interior node */
static void _prtree3@t@node_fix_counts_local (VsgPRTree3@t@Node *node)
{
  /* *warning*: not a recursive function. Children counts are assumed valid */

  gint pcnt = 0;
  gint rcnt = 0;

  if (PRTREE3@T@NODE_ISINT (node))
    {
      vsgloc3 i;
      for (i=0; i<CN; i++)
        {
          pcnt += PRTREE3@T@NODE_INT (node).children[i]->point_count;
          rcnt += PRTREE3@T@NODE_INT (node).children[i]->region_count;
        }
    }
  else
    {
      pcnt = g_slist_length (PRTREE3@T@NODE_LEAF (node).point);;
    }

  node->point_count = pcnt;
  node->region_count = rcnt + g_slist_length (node->region_list);
}

#define STATUS_CMP_REMOTE(one, other) (                          \
(~ ((one).storage ^ (other).storage)) & VSG_PARALLEL_REMOTE_MASK \
)

#define STATUS_CMP_SHARED(one, other) (                          \
(~ ((one).storage ^ (other).storage)) & VSG_PARALLEL_SHARED_MASK \
)

static void
_prtree3@t@node_get_status_from_children (VsgPRTree3@t@Node *node,
                                          VsgParallelStatus *status)
{
  vsgloc3 i;
  VsgPRTree3@t@Node *children[CN];

  g_assert (PRTREE3@T@NODE_ISINT (node));

  memcpy (children, PRTREE3@T@NODE_INT (node).children,
          CN * sizeof (VsgPRTree3@t@Node *));

  *status = children[0]->parallel_status;

  if (VSG_PARALLEL_STATUS_IS_SHARED_LOCAL (*status))
    return;


  for (i=1; i<CN; i++)
    {
      // remote+local
      if (! STATUS_CMP_REMOTE (children[i]->parallel_status, *status))
        {
          status->storage = VSG_PARALLEL_SHARED_LOCAL;
          return;
        }

      // shared+private
      if (! STATUS_CMP_SHARED (children[i]->parallel_status, *status))
        {
          status->storage |= VSG_PARALLEL_SHARED_MASK;

          if (VSG_PARALLEL_STATUS_IS_LOCAL (*status)) return;
        }
      // both private-remote
      else if (VSG_PARALLEL_STATUS_IS_PRIVATE_REMOTE (*status))
        {
          if (PRTREE3@T@NODE_PROC (children[i]) != status->proc)
            status->storage |= VSG_PARALLEL_SHARED_MASK;
        }
    }
}

static void _flatten_remote (VsgPRTree3@t@Node *node,
                             const VsgPRTree3@t@Config *config)
{
  /* destroy remaining children */
  if (PRTREE3@T@NODE_ISINT (node))
    {
      gint i;

      for (i=0; i<CN; i++)
        {
          vsg_prtree3@t@node_free (PRTREE3@T@NODE_CHILD (node, i), config);
          PRTREE3@T@NODE_CHILD (node, i) = NULL;
        }
    }

  /* remote nodes aren't aware of points and regions stored on another
   * processor */
  g_slist_foreach (node->region_list, 
                   (GFunc) config->parallel_config.region.destroy,
                   config->parallel_config.region.destroy_data);
  g_slist_free (node->region_list);
  node->region_list = NULL;

  /* remove precedent user_data: not needed for a remote node */
  if (node->user_data != NULL)
    {
      const VsgPRTreeParallelConfig *pc = &config->parallel_config;

      if (pc->node_data.destroy)
        pc->node_data.destroy (node->user_data, TRUE,
                               pc->node_data.destroy_data);

      node->user_data = NULL;
    }

  node->point_count = 0;
  node->region_count = 0;
}

static void _traverse_flatten_remote (VsgPRTree3@t@Node *node,
                                      VsgPRTree3@t@NodeInfo *node_info,
                                      const VsgPRTree3@t@Config *config)
{
  // *** PRIVATE-REMOTE
  if (PRTREE3@T@NODE_IS_PRIVATE_REMOTE (node))
    {
      _flatten_remote (node, config);
    }
}

typedef struct _DistributeData DistributeData;
struct _DistributeData {
  VsgPRTree3@t@DistributionFunc func;
  gpointer user_data;
  gint rk;
  gint sz;
  VsgCommBuffer *cb;
  VsgPackedMsg *bcast;
  VsgPRTree3@t@Config *config;
};

static void _traverse_distribute_nodes (VsgPRTree3@t@Node *node,
                                        VsgPRTree3@t@NodeInfo *node_info,
                                        DistributeData *dd)
{
  VsgParallelStatus old_status = node->parallel_status;
  VsgParallelStatus new_status;

  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      gint dst;

      // *** == PRIVATE
      g_assert (VSG_PARALLEL_STATUS_IS_PRIVATE (old_status));

      dst = dd->func (node_info, dd->user_data);

      if (dst<0 || dst>=dd->sz) return; /* don't know: do nothing */

      new_status.storage = (dst == dd->rk) ? VSG_PARALLEL_PRIVATE_LOCAL :
        VSG_PARALLEL_PRIVATE_REMOTE;
      new_status.proc = dst;
    }
  else
    {
      _prtree3@t@node_get_status_from_children (node, &new_status);
    }

  if (new_status.proc != dd->rk || (old_status.storage != new_status.storage))
    {
      // *** PRIVATE-LOCAL
      if (old_status.storage == VSG_PARALLEL_PRIVATE_LOCAL)
        {
          gint dst = new_status.proc;

          if (VSG_PARALLEL_STATUS_IS_SHARED (new_status)) dst = - dst - 1;

          /* tell all processors about the migration. */
          _migrate_node_pack_header (node_info, dd->bcast, dst, dd->config);

/*           g_printerr ("%d : packing dst=%d id=", dd->rk, dst); */
/*           vsg_prtree_key3@t@_write (&node_info->id, stderr); */
/*           g_printerr ("\n"); */

          /* choose between ptp communication or bcast */
          // *** == SHARED
          if (VSG_PARALLEL_STATUS_IS_SHARED (new_status))
            {
              VsgPackedMsg *msg = dd->bcast;
              NodePackData npd = NPD_MIGRATE (&(dd->config->parallel_config),
                                              msg);

              /* pack node to destination */
              _node_pack (node, &npd);
            }
          else
            {
              VsgPackedMsg *msg =
                vsg_comm_buffer_get_send_buffer (dd->cb, new_status.proc);
              NodePackData npd = NPD_MIGRATE (&(dd->config->parallel_config),
                                              msg);

              /* pack node to destination */
              _node_pack_and_destroy (node, &npd);
            }
        }

    }

  // *** PRIVATE-REMOTE
  if (old_status.storage == VSG_PARALLEL_PRIVATE_REMOTE)
    return; /* check migrations later */

  // *** PRIVATE-REMOTE
  if (new_status.storage == VSG_PARALLEL_PRIVATE_REMOTE)
    {
      _flatten_remote (node, dd->config);
    }
  // *** SHARED-*
  else if (VSG_PARALLEL_STATUS_IS_SHARED (new_status))
    {
      _prtree3@t@node_fix_counts_local (node);
    }

  /* update node's parallel status */
  node->parallel_status = new_status;
}

/*
 * Replaces a sub-node from a given tree by executing specified 
 * function (@replace_func) at some arbitrary position (given by @key),
 * eventually creating a whole hierarchy.
 */
static void _node_insert_child (VsgPRTree3@t@Node *node,
                                const VsgPRTree3@t@Config *config,
                                VsgParallelStorage storage,
                                gint dst,
                                VsgPRTreeKey3@t@ key,
                                VsgPRTree3@t@NodeFunc replace_func,
                                gpointer replace_data)
{
  if (key.depth > 0)
    {
      vsgloc3 child = vsg_prtree_key3@t@_child (&key);
      VsgParallelStatus new_status;

      if (PRTREE3@T@NODE_ISLEAF (node))
        {
          /* formerly remote nodes need a new user_data to be allocated */
          // *** PRIVATE-REMOTE
          if (PRTREE3@T@NODE_IS_PRIVATE_REMOTE (node))
            {
              const VsgPRTreeParallelConfig *pc = &config->parallel_config;

              g_assert (node->user_data == NULL);

              if (pc->node_data.alloc != NULL)
                {
                  node->user_data =
                    pc->node_data.alloc (TRUE, pc->node_data.alloc_data);
                }
            }

          /* transform into interior node */
          vsg_prtree3@t@node_make_int (node, config);
        }

      key.depth --;

      _node_insert_child (PRTREE3@T@NODE_CHILD (node, child),
                          config, storage, dst, key, replace_func,
                          replace_data);

      _prtree3@t@node_fix_counts_local (node);

      _prtree3@t@node_get_status_from_children (node, &new_status);

      if (VSG_PARALLEL_STATUS_IS_PRIVATE (new_status))
        {
          g_assert (VSG_PARALLEL_STATUS_PROC (new_status) == dst);

          // *** PRIVATE-REMOTE
          if (storage == VSG_PARALLEL_PRIVATE_REMOTE)
            {
              _flatten_remote (node, config);
            }
        }

      node->parallel_status = new_status;

      return;
    }

  // *** PRIVATE-REMOTE
  if (storage == VSG_PARALLEL_PRIVATE_REMOTE)
    {
      _flatten_remote (node, config);
    }
  else
    {
      if (replace_func) replace_func (node, replace_data);
    }

  _prtree3@t@node_fix_counts_local (node);

  node->parallel_status.storage = storage;
  node->parallel_status.proc = dst;
}

/**
 * vsg_prtree3@t@_distribute_nodes:
 * @tree: a #VsgPRTree3@t@
 * @func: distribution function to use
 * @user_data: data to pass to @func
 *
 * performs the parallel distribution of nodes specified with
 * @func. @func will be called once on every local leaf and its result
 * will determine the destination processor of this node.
 *
 * NOTE: this is a collective communication function.
 *
 * WARNING: only available when VSG_HAVE_MPI is defined.
 */
void vsg_prtree3@t@_distribute_nodes (VsgPRTree3@t@ *tree,
                                      VsgPRTree3@t@DistributionFunc func,
                                      gpointer user_data)
{
  MPI_Comm comm;
  VsgCommBuffer *cb;
  VsgCommBuffer *bcastcb;
  VsgPackedMsg bcast = VSG_PACKED_MSG_NULL;
  VsgPRTreeParallelConfig *pc;
  DistributeData dd;
  gint rk, sz, src;

  g_return_if_fail (tree != NULL);
  pc = &tree->config.parallel_config;
  comm = pc->communicator;
  g_return_if_fail (comm != MPI_COMM_NULL);
  
  MPI_Comm_rank (comm, &rk);
  MPI_Comm_size (comm, &sz);

  cb = vsg_comm_buffer_new (comm);
  bcastcb = vsg_comm_buffer_new (comm);
  vsg_packed_msg_init (&bcast, comm);

  dd.func = func;
  dd.user_data = user_data;
  dd.rk = rk;
  dd.sz = sz;
  dd.cb = cb;
  dd.bcast = &bcast;
  dd.config = &tree->config;

/*   g_printerr ("%d: before gather\n", rk); */

  /* gather all migration messages */
  vsg_prtree3@t@_traverse_custom_internal (tree, G_POST_ORDER, NULL, NULL, NULL,
                                           (VsgPRTree3@t@InternalFunc)
                                           _traverse_distribute_nodes, &dd);

/*   g_printerr ("%d: after gather\n", rk); */

  /* send/receive pending messages */
  vsg_comm_buffer_set_bcast (bcastcb, &bcast);

/*   g_printerr ("%d: before share (bcast = %dBytes)\n", rk, bcast.position); */

  vsg_comm_buffer_share (bcastcb);

/*   g_printerr ("%d: bcast ok\n%d: before private msg\n", rk, rk); */

  vsg_comm_buffer_share (cb);

  vsg_packed_msg_drop_buffer (&bcast);

/*   g_printerr ("%d: after share\n", rk); */

  /* decode received messages */
  for (src=0; src<sz; src++)
    {
      VsgPackedMsg *hdrmsg, *msg;

      if (src == rk) continue;

      hdrmsg = vsg_comm_buffer_get_recv_buffer (bcastcb, src);
      msg = vsg_comm_buffer_get_recv_buffer (cb, src);

      while (hdrmsg->position < hdrmsg->size)
        {
          VsgPRTreeKey3@t@ id;
          gint dst;

          /* unpack the header */
          vsg_packed_msg_recv_read (hdrmsg, &id, 1,
                                    VSG_MPI_TYPE_PRTREE_KEY3@T@);
          vsg_packed_msg_recv_read (hdrmsg, &dst, 1, MPI_INT);

/*           g_printerr ("%d: unpacking src=%d id=[", rk, src); */
/*           vsg_prtree_key3@t@_write (&id, stderr); */
/*           g_printerr ("] dst=%d\n", dst); */

          if (dst == rk || dst < 0)
            { /* we are destination of this message (bcast or not) */
              VsgParallelStorage storage =
                // *** >= 0 : PRIVATE-LOCAL
                // *** == -x : SHARED-LOCAL
                // *** == -x : SHARED-REMOTE
                (dst >= 0) ? VSG_PARALLEL_PRIVATE_LOCAL :
                VSG_PARALLEL_SHARED_LOCAL; /* force all shared to be local */
              VsgPackedMsg *unpack = (dst >= 0) ? msg : hdrmsg;
              NodePackData npd = NPD_MIGRATE (pc, unpack);

              if (dst < 0) dst = -dst - 1;

              /* load node contents */

              _node_insert_child (tree->node, &tree->config,
                                  storage, dst, id,
                                  (VsgPRTree3@t@NodeFunc) _node_unpack, &npd);

            }
          else
            {
              /* we just witness the migration but we have to keep track of
               * remote node locations.
               */

              /* insert the node in remote mode */
              // *** PRIVATE-REMOTE
              _node_insert_child (tree->node, &tree->config,
                                  VSG_PARALLEL_PRIVATE_REMOTE, dst,
                                  id, NULL, NULL);
            }

/*           g_printerr ("%d: unpacking done\n", rk); */
        }
    }

  vsg_comm_buffer_free (bcastcb);
  vsg_comm_buffer_free (cb);

  /* fix all remote nodes: remove remaining subtree and locally stored
   * regions */
  vsg_prtree3@t@_traverse_custom_internal (tree, G_POST_ORDER,
                                           (VsgRegion3@t@InternalLocDataFunc)
                                           _selector_skip_local_nodes,
                                           NULL, NULL,
                                           (VsgPRTree3@t@InternalFunc)
                                           _traverse_flatten_remote,
                                           &tree->config);

  /* remote trees depth may have changed */
  tree->config.remote_depth_dirty = TRUE;
}

#define DIRECTION_FORWARD (0)
#define DIRECTION_BACKWARD (1)

/* messages tags */
#define VISIT_FORWARD_TAG (100)
#define VISIT_BACKWARD_TAG (101)
#define END_FW_TAG (102)
#define VISIT_SHARED_TAG (103)

/* send mode */
#define _nf_msg_isend vsg_packed_msg_issend

static gint _packed_msg_max_size = G_MAXINT;

/*
 * Holds a visiting node while it waits to be sent back to its original
 * processor.
 */
typedef struct _WaitingVisitor WaitingVisitor;
struct _WaitingVisitor {
  VsgPRTree3@t@Node *node;
  VsgPRTreeKey3@t@ id;
  WaitingVisitor *next;
  gint src;
  gint8 flag;
  guint8 semifar_threshold_height;
  gboolean fake_leaf;
};

#define _USE_G_SLICES GLIB_CHECK_VERSION (2, 10, 0)

#if ! _USE_G_SLICES
static GMemChunk *_waiting_visitor_mem_chunk = 0;

/* static functions: */
static void _waiting_visitor_finalize ();
static WaitingVisitor *_waiting_visitor_alloc ();

static void _waiting_visitor_finalize ()
{
  if (_waiting_visitor_mem_chunk)
    {
      g_mem_chunk_destroy (_waiting_visitor_mem_chunk);
      _waiting_visitor_mem_chunk = 0;
    }
}

static WaitingVisitor *_waiting_visitor_alloc ()
{
  if (!_waiting_visitor_mem_chunk)
    {
      _waiting_visitor_mem_chunk = g_mem_chunk_create (WaitingVisitor,
                                                       16, G_ALLOC_ONLY);
      g_atexit (_waiting_visitor_finalize);

    }
  return g_chunk_new (WaitingVisitor, _waiting_visitor_mem_chunk);
}

#endif

static WaitingVisitor *_waiting_visitor_new (VsgPRTree3@t@Node *node,
                                             VsgPRTreeKey3@t@ *id,
                                             gboolean fake_leaf,
                                             gint src,
                                             gint8 semifar_threshold_height)
{
#if _USE_G_SLICES
  WaitingVisitor *wv = g_slice_new (WaitingVisitor);
#else
  WaitingVisitor *wv = _waiting_visitor_alloc ();
#endif

  wv->node = node;
  wv->id = *id;
  wv->next = NULL;
  wv->src = src;
  wv->flag = 0;
  wv->semifar_threshold_height = semifar_threshold_height;
  wv->fake_leaf = fake_leaf;

  return wv;
}

static void _waiting_visitor_free (WaitingVisitor *wv)
{
  if (wv->next != NULL) _waiting_visitor_free (wv->next);

#if _USE_G_SLICES
  g_slice_free (WaitingVisitor, wv);
#else
  g_chunk_free (wv, _waiting_visitor_mem_chunk);
#endif
}

static void _wv_free_gfunc (WaitingVisitor *wv, gpointer data)
{
  _waiting_visitor_free (wv);
}

/*
 * Holds a message and the request associated with a particular transfer (send
 * or receive).
 */
typedef struct _VsgNFProcMsg VsgNFProcMsg;
struct _VsgNFProcMsg {
  VsgPackedMsg send_pm;
  gint dropped_visitors;
  GSList *forward_pending;
  GSList *backward_pending;
};

static void vsg_nf_proc_msg_init (VsgNFProcMsg *nfpm, MPI_Comm comm)
{
  vsg_packed_msg_init (&nfpm->send_pm, comm);

  nfpm->dropped_visitors = 0;

  nfpm->forward_pending = NULL;
  nfpm->backward_pending = NULL;
}

static VsgNFProcMsg *vsg_nf_proc_msg_new (MPI_Comm comm)
{
#if _USE_G_SLICES
  VsgNFProcMsg *nfpm = g_slice_new0 (VsgNFProcMsg);
#else
  VsgNFProcMsg *nfpm = g_new0 (VsgNFProcMsg, 1);
#endif

  vsg_nf_proc_msg_init(nfpm, comm);

  return nfpm;
}

static void vsg_nf_proc_msg_free (VsgNFProcMsg *nfpm)
{
  vsg_packed_msg_drop_buffer (&nfpm->send_pm);

  g_slist_foreach (nfpm->forward_pending, (GFunc) _wv_free_gfunc, NULL);
  g_slist_free (nfpm->forward_pending);
  g_slist_foreach (nfpm->backward_pending, (GFunc) _wv_free_gfunc, NULL);
  g_slist_free (nfpm->backward_pending);

#if _USE_G_SLICES
  g_slice_free (VsgNFProcMsg, nfpm);
#else
  g_free (nfpm);
#endif
}

/*
 * Initializes a VsgNFConfig3@t@.
 */
void vsg_nf_config3@t@_init (VsgNFConfig3@t@ *nfc,
                             VsgPRTree3@t@ *tree,
                             VsgPRTree3@t@FarInteractionFunc far_func,
                             VsgPRTree3@t@InteractionFunc near_func,
                             VsgPRTree3@t@SemifarInteractionFunc semifar_func,
                             guint semifar_threshold,
                             gpointer user_data)
{
  MPI_Comm comm = tree->config.parallel_config.communicator;

  nfc->tree = tree;

  nfc->far_func = far_func;
  nfc->near_func = near_func;
  nfc->semifar_func = semifar_func;
  nfc->semifar_threshold = semifar_threshold;
  nfc->user_data = user_data;

  vsg_packed_msg_init (&nfc->recv, comm);
  nfc->recv_lock = FALSE;
  nfc->procs_msgs = NULL;
  nfc->procs_requests = NULL;

   if (comm != MPI_COMM_NULL)
    {
      gint i;

      MPI_Comm_rank (comm, &nfc->rk);
      MPI_Comm_size (comm, &nfc->sz);

      nfc->procs_msgs =
        g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL,
                               (GDestroyNotify) vsg_nf_proc_msg_free);

      nfc->procs_requests = g_malloc (nfc->sz * sizeof (MPI_Request));
      for (i=0; i<nfc->sz; i++) nfc->procs_requests[i] = MPI_REQUEST_NULL;
    }
  else
    {
      nfc->rk = 0;
      nfc->sz = 1;
    }

   nfc->tmp_node_data = NULL;
   nfc->tmp_point = NULL;
   nfc->tmp_region = NULL;

   nfc->forward_pending_nb = 0;
   nfc->backward_pending_nb = 0;

   nfc->pending_end_forward = nfc->sz-1;
   nfc->pending_backward_msgs = 0;

   nfc->all_fw_sends = 0; nfc->all_fw_recvs = 0;
   nfc->all_bw_sends = 0; nfc->all_bw_recvs = 0;

   nfc->shared_far_interaction_counter = 0;
}

void vsg_nf_config3@t@_tmp_alloc (VsgNFConfig3@t@ *nfc,
                                  VsgPRTree3@t@Config *config)
{
  VsgPRTreeParallelConfig *pc = &config->parallel_config;

  if (pc->node_data.alloc != NULL)
    {
      nfc->tmp_node_data = pc->node_data.alloc (FALSE,
                                                pc->node_data.alloc_data);
    }

  if (pc->point.alloc != NULL)
    {
      nfc->tmp_point = pc->point.alloc (FALSE, pc->point.alloc_data);
    }

  if (pc->region.alloc != NULL)
    {
      nfc->tmp_region = pc->region.alloc (FALSE, pc->region.alloc_data);
    }

}

void vsg_nf_config3@t@_tmp_free (VsgNFConfig3@t@ *nfc,
                                 VsgPRTree3@t@Config *config)
{
  VsgPRTreeParallelConfig *pc = &config->parallel_config;

  if (pc->node_data.destroy != NULL)
    {
      pc->node_data.destroy (nfc->tmp_node_data, FALSE,
                             pc->node_data.destroy_data);
    }

  nfc->tmp_node_data = NULL;

  if (pc->point.destroy != NULL)
    {
      pc->point.destroy (nfc->tmp_point, FALSE, pc->point.destroy_data);
      nfc->tmp_point = NULL;
    }

  if (pc->region.destroy != NULL)
    {
      pc->region.destroy (nfc->tmp_region, FALSE, pc->region.destroy_data);
      nfc->tmp_region = NULL;
    }
}

/*
 * Frees all memory allocated inside a VsgNFConfig3@t@. (doesn't free @nfc
 * itself).
 */
void vsg_nf_config3@t@_clean (VsgNFConfig3@t@ *nfc)
{
  vsg_packed_msg_drop_buffer (&nfc->recv);

  if (nfc->procs_msgs != NULL)
    {
      g_hash_table_destroy (nfc->procs_msgs);
      nfc->procs_msgs = NULL;
    }

  if (nfc->procs_requests != NULL)
    {
      g_free (nfc->procs_requests);
      nfc->procs_requests = NULL;
    }
}

/*
 * Searchs for the VsgNFProcMsg associated with a particular processor.
 * Allocates a new one if necessary.
 */
static VsgNFProcMsg * vsg_nf_config3@t@_proc_msgs_lookup (VsgNFConfig3@t@ *nfc,
                                                          gint proc)
{
  VsgNFProcMsg *nfpm = g_hash_table_lookup (nfc->procs_msgs,
                                            GINT_TO_POINTER (proc));

  if (nfpm == NULL)
    {
      nfpm = vsg_nf_proc_msg_new (nfc->recv.communicator);

      g_hash_table_insert (nfc->procs_msgs, GINT_TO_POINTER (proc), nfpm);
    }

  return nfpm;
}

static void _visit_destroy_point (VsgPoint3 pt, VsgParallelVTable *vtable)
{
  vtable->destroy (pt, FALSE, vtable->destroy_data);
}

static void _visit_destroy_region (VsgRegion3 rg, VsgParallelVTable *vtable)
{
  vtable->destroy (rg, FALSE, vtable->destroy_data);
}

/*
 * Allocates a floating VsgPRTree3@t@Node to hold a visiting node from another
 * processor.
 */
static VsgPRTree3@t@Node *_new_visiting_node (VsgPRTree3@t@ *tree,
                                              VsgPRTreeKey3@t@ *id,
                                              gint src)
{
  VsgPRTree3@t@Node *node;
  VsgVector3@t@ *lb = &tree->node->lbound;
  VsgVector3@t@ *ub = &tree->node->ubound;
  VsgVector3@t@ newlb, newub;

  /* compute the bounds of the visiting node */
  if (id->depth == 0)
    {
      vsg_vector3@t@_copy (lb, &newlb);
      vsg_vector3@t@_copy (ub, &newub);
    }
  else
    {
      VsgVector3@t@ delta;

      vsg_vector3@t@_sub (ub, lb, &delta);

      vsg_vector3@t@_scalp (&delta, 1. / (1 << id->depth), &delta);

      newlb.x = lb->x + id->x * delta.x;
      newlb.y = lb->y + id->y * delta.y;
      newlb.z = lb->z + id->z * delta.z;

      vsg_vector3@t@_add (&newlb, &delta, &newub);
    }

  /* allocate new node without node's user_data */
  node = vsg_prtree3@t@node_alloc_no_data (&newlb, &newub);

  // *** PRIVATE-REMOTE
  node->parallel_status.storage = VSG_PARALLEL_PRIVATE_REMOTE;
  node->parallel_status.proc = src;

  return node;
}

/*
 * Deallocates the VsgPRTree3@t@Node structure used by a visiting node.
 */
static void _visiting_node_free (VsgPRTree3@t@Node *node,
                                VsgPRTree3@t@Config *config)
{
  VsgPRTreeParallelConfig *pc = &config->parallel_config;

  if (node->user_data != NULL)
    {
      if (pc->node_data.destroy)
        pc->node_data.destroy (node->user_data, FALSE,
                               pc->node_data.destroy_data);

      node->user_data = NULL;
    }

  if (PRTREE3@T@NODE_ISLEAF (node))
    {
      g_slist_foreach (PRTREE3@T@NODE_LEAF (node).point,
                       (GFunc) _visit_destroy_point,
                       &pc->point);

      g_slist_free (PRTREE3@T@NODE_LEAF (node).point);
      PRTREE3@T@NODE_LEAF (node).point = NULL;
    }

  g_slist_foreach (node->region_list, (GFunc) _visit_destroy_region,
                   &pc->region);

  g_slist_free (node->region_list);
  node->region_list = NULL;

  vsg_prtree3@t@node_free (node, config);
}

static gint _pack_subtree (VsgPRTree3@t@Node *node, VsgPRTreeKey3@t@ *id,
                           NodePackData *npd)
{
  gint ret = 1;

  vsg_packed_msg_send_append (npd->pt_fpd.msg, id, 1,
                              VSG_MPI_TYPE_PRTREE_KEY3@T@);
  _node_pack (node, npd);

  if (PRTREE3@T@NODE_ISINT (node))
    {
      gint i;

      for (i=0; i<CN; i++)
        {
          VsgPRTreeKey3@t@ child_id;
          vsg_prtree_key3@t@_build_child (id, i, &child_id);

          ret += _pack_subtree (PRTREE3@T@NODE_CHILD (node, i), &child_id, npd);
        }
    }

  return ret;
}

static gint _pack_tree (VsgPRTree3@t@Node *node, VsgPRTreeKey3@t@ *id,
                        NodePackData *npd, guint8 semifar_threshold_height,
                        gboolean recursive)
{
  gint ret = 1;

  vsg_packed_msg_send_append (npd->pt_fpd.msg, id, 1,
                              VSG_MPI_TYPE_PRTREE_KEY3@T@);
  vsg_packed_msg_send_append (npd->pt_fpd.msg, &semifar_threshold_height, 1,
                              MPI_UNSIGNED_CHAR);
  _node_pack (node, npd);

  if (recursive && PRTREE3@T@NODE_ISINT (node))
    {
      gint i;

      for (i=0; i<CN; i++)
        {
          VsgPRTreeKey3@t@ child_id;
          vsg_prtree_key3@t@_build_child (id, i, &child_id);

          ret += _pack_subtree (PRTREE3@T@NODE_CHILD (node, i), &child_id, npd);
        }
    }

  return ret;
}

static void _do_send_forward_node (VsgNFConfig3@t@ *nfc,
                                   VsgNFProcMsg *nfpm,
                                   VsgPRTree3@t@Node *node,
                                   VsgPRTreeKey3@t@ *id,
                                   gint proc,
                                   gboolean pack_subtree,
                                   guint8 semifar_threshold_height)
{
  gint sent = 0;
  VsgPackedMsg *msg = &nfpm->send_pm;
  NodePackData npd =
    NPD_VISIT_FORWARD (&nfc->tree->config.parallel_config, msg);

  /* g_printerr ("%d : pack with semifar height=%u depth=%d\n", nfc->rk, semifar_threshold_height, id->depth); */

  msg->position = 0;

  sent += _pack_tree (node, id, &npd, semifar_threshold_height, pack_subtree);

  _nf_msg_isend (msg, proc, VISIT_FORWARD_TAG, &nfc->procs_requests[proc]);

  nfc->all_fw_sends ++;
  nfc->pending_backward_msgs += sent;
}

static void _send_pending_forward_node (VsgNFConfig3@t@ *nfc,
                                        VsgNFProcMsg *nfpm,
                                        gint proc)
{
  GSList *first = nfpm->forward_pending;
  WaitingVisitor *wv = (WaitingVisitor *) first->data;

  nfpm->forward_pending = g_slist_next (nfpm->forward_pending);
  g_slist_free_1 (first);

  _do_send_forward_node (nfc, nfpm, wv->node, &wv->id, proc, wv->fake_leaf, wv->semifar_threshold_height);

  _waiting_visitor_free (wv);

  nfc->forward_pending_nb --;
}

static void _pack_wv_and_destroy (VsgPackedMsg *msg, WaitingVisitor *wv,
                                  NodePackData *npd,
                                  VsgPRTree3@t@Config *config)
{
  if (wv == NULL) return;

  vsg_packed_msg_send_append (msg, &wv->id, 1, VSG_MPI_TYPE_PRTREE_KEY3@T@);
  _node_pack_and_destroy (wv->node, npd);
  vsg_prtree3@t@node_free (wv->node, config);

  _pack_wv_and_destroy (msg, wv->next, npd, config);

  wv->next = NULL;
  _waiting_visitor_free (wv);

}

/* static gint _bw_pending_max = 0; */
/* static gint _bw_pending_sum = 0; */
/* static gint _bw_pending_calls = 0; */

static void _do_send_backward_node (VsgNFConfig3@t@ *nfc,
                                    VsgNFProcMsg *nfpm,
                                    WaitingVisitor *wv,
                                    gint proc)
{
  VsgPackedMsg *msg = &nfpm->send_pm;
  NodePackData npd =
    NPD_VISIT_BACKWARD (&nfc->tree->config.parallel_config, msg);

/*   gint _bw_pending_length = g_slist_length (nfpm->backward_pending); */

/*   _bw_pending_calls ++; */
/*   _bw_pending_sum += _bw_pending_length; */
/*   _bw_pending_max = MAX (_bw_pending_max, _bw_pending_length); */

  msg->position = 0;

  if (nfpm->dropped_visitors > 0)
    {
      VsgPRTreeKey3@t@ k = vsg_prtree_key3@t@_root;

/*               g_printerr ("%d : sending dropped number %d to %d\n", */
/*                           nfc->rk, nfpm->dropped_visitors, wv->src); */

      /* issue a false key followed with number of dropped visitors */
      vsg_packed_msg_send_append (msg, &k, 1,
                                  VSG_MPI_TYPE_PRTREE_KEY3@T@);
      vsg_packed_msg_send_append (msg, &nfpm->dropped_visitors, 1,
                                  MPI_INT);
      nfpm->dropped_visitors = 0;
    }

  /* g_printerr ("%d : bw node 0x%lx 0x%lx 0x%lx\n", nfc->rk, wv->id.x, wv->id.y, wv->id.z); */
  _pack_wv_and_destroy (msg, wv, &npd, &nfc->tree->config);

  _nf_msg_isend (msg, proc, VISIT_BACKWARD_TAG, &nfc->procs_requests[proc]);

  nfc->all_bw_sends ++;
}

static void _send_pending_backward_node (VsgNFConfig3@t@ *nfc,
                                         VsgNFProcMsg *nfpm,
                                         gint proc)
{
  VsgPackedMsg *msg = &nfpm->send_pm;
  NodePackData npd =
    NPD_VISIT_BACKWARD (&nfc->tree->config.parallel_config, msg);
  gint maxsize = MAX (_packed_msg_max_size, vsg_packed_msg_header_size ()+1);
  gint sending = 0;

  msg->position = 0;

  if (nfpm->dropped_visitors > 0)
    {
      VsgPRTreeKey3@t@ k = vsg_prtree_key3@t@_root;

      /* issue a false key followed with number of dropped visitors */
      vsg_packed_msg_send_append (msg, &k, 1,
                                  VSG_MPI_TYPE_PRTREE_KEY3@T@);
      vsg_packed_msg_send_append (msg, &nfpm->dropped_visitors, 1,
                                  MPI_INT);
      nfpm->dropped_visitors = 0;
      sending ++;
    }

  /* check for backward messages */
  while (nfpm->backward_pending != NULL &&
         nfpm->send_pm.position < maxsize)
    {
      GSList *first = nfpm->backward_pending;
      WaitingVisitor *wv = (WaitingVisitor *) first->data;

      nfpm->backward_pending = g_slist_next (nfpm->backward_pending);
      g_slist_free_1 (first);

      _pack_wv_and_destroy (msg, wv, &npd, &nfc->tree->config);

      nfc->backward_pending_nb --;

      sending ++;
    }

  if (sending > 0)
    {
      _nf_msg_isend (msg, proc, VISIT_BACKWARD_TAG,
                     &nfc->procs_requests[proc]);

      nfc->all_bw_sends ++;
    }
}

static void _propose_node_forward (VsgNFConfig3@t@ *nfc,
                                   gint proc,
                                   VsgPRTree3@t@Node *node,
                                   VsgPRTreeKey3@t@ *id, gboolean fake_leaf,
                                   guint8 semifar_threshold_height)
{
  gint flag;
  VsgNFProcMsg *nfpm;

  nfpm = vsg_nf_config3@t@_proc_msgs_lookup (nfc, proc);

  MPI_Test (&nfc->procs_requests[proc], &flag, MPI_STATUS_IGNORE);

  if (flag && nfpm->backward_pending == NULL)
    {
      _do_send_forward_node (nfc, nfpm, node, id, proc, fake_leaf, semifar_threshold_height);
    }
  else
    {
      WaitingVisitor *wv = _waiting_visitor_new (node, id, fake_leaf, proc, semifar_threshold_height);

      nfpm->forward_pending = g_slist_append (nfpm->forward_pending, wv);

      if (flag)
        {
          _send_pending_backward_node (nfc, nfpm, proc);
        }

      nfc->forward_pending_nb ++;
    }

}

static void _propose_node_backward (VsgNFConfig3@t@ *nfc,
                                    gint proc,
                                    WaitingVisitor *wv)
{
  gint flag;
  VsgNFProcMsg *nfpm;

  nfpm = vsg_nf_config3@t@_proc_msgs_lookup (nfc, proc);

  MPI_Test (&nfc->procs_requests[proc], &flag, MPI_STATUS_IGNORE);

  if (flag != 0)
    {
      _do_send_backward_node (nfc, nfpm, wv, proc);

/*       g_printerr ("%d : propose bw direct to %d - ", nfc->rk, proc); */
/*       vsg_prtree_key3@t@_write (&wv->id, stderr); */
/*       g_printerr ("\n"); */
    }
  else
    {
      nfpm->backward_pending = g_slist_append (nfpm->backward_pending, wv);

/*       g_printerr ("%d : propose bw delay to %d - ", nfc->rk, proc); */
/*       vsg_prtree_key3@t@_write (&wv->id, stderr); */
/*       g_printerr ("\n"); */

      nfc->backward_pending_nb ++;
    }

}

/*
 * Stores visitor's node info and near/far functions for a
 * vsg_prtree3@t@_traverse operation.
 */
typedef struct _NIAndFuncs NIAndFuncs;
struct _NIAndFuncs {
  WaitingVisitor *ref;
  VsgPRTree3@t@NodeInfo ref_info;
  VsgNFConfig3@t@ *nfc;
  VsgPRTreeKey3@t@ *ref_ancestry_ids;
  guint8 semifar_threshold_height;
  gint8 done_flag;
};

static gboolean _check_ancestor_semifar (VsgPRTree3@t@NodeInfo *ref_info,
                                         VsgPRTree3@t@NodeInfo *node_info,
                                         guint8 semifar_threshold_height)
{
  @key_type@ dist;
  guint8 h;

  /*
   * dist<=2: call near_func, will handle semifar if needed
   */
  dist = vsg_prtree_key3@t@_deepest_distance (&ref_info->id, &node_info->id);
  if (dist <= 2) return FALSE;

  /* here, ref_info is always deeper than node_info */
  g_assert (ref_info->depth >= node_info->depth);
  h = ref_info->depth - node_info->depth;

  h = MIN (h, semifar_threshold_height);

  /* if dist > 2^h, semifar_func was already applied somewhere in the ancestry */
  return dist > 1 << h;
}

/*
 * computes near/far interactions for a visiting node during a tree traversal.
 */
static void _traverse_visiting_nf (VsgPRTree3@t@Node *node,
                                   VsgPRTree3@t@NodeInfo *node_info,
                                   NIAndFuncs *niaf)
{
  VsgPRTree3@t@NodeInfo *ref_info = &niaf->ref_info;
  guint8 node_depth;

  node_depth = node_info->id.depth;

  if (node_depth <= ref_info->id.depth)
    {
      VsgPRTreeKey3@t@ *ref_id = &niaf->ref_ancestry_ids[node_depth];
      if (vsg_prtree_key3@t@_is_neighbour (ref_id, &node_info->id))
        {
          if (node->point_count == 0) return;

          if (!ref_info->isleaf && niaf->semifar_threshold_height == 0)
            {
              if (PRTREE3@T@NODE_ISLEAF (node) &&
                  niaf->nfc->semifar_func != NULL)
                {
                  @key_type@ dist;
                  dist = vsg_prtree_key3@t@_deepest_distance (&ref_info->id, &node_info->id);

                  /* g_printerr ("%d : visitor near ref[%#@kmod@x %#@kmod@x %#@kmod@x %d] node[%#lx %#lx %#lx %d] dist=%@kmod@d " */
                  /*             "th_height=%d\n", */
                  /*             niaf->nfc->rk, */
                  /*             ref_info->id.x, ref_info->id.y, ref_info->id.z, ref_info->depth, */
                  /*             node_info->id.x, node_info->id.y, node_info->id.z, node_info->depth, */
                  /*             dist, niaf->semifar_threshold_height); */

                  if (dist == 2)
                    {
                      niaf->nfc->semifar_func (&niaf->ref_info, node_info, niaf->nfc->user_data);
                      niaf->done_flag |= 1;
                    }
                }

              return;
            }

          if (PRTREE3@T@NODE_ISLEAF (node) ||
              node_depth == ref_info->id.depth)
            {
              VsgPRTree3@t@NodeInfo info;
              WaitingVisitor *wv;

              /* skip interactions that were already taken in account with
               * a semifar_func somewhere in "ref" ancestry
               */
              if (niaf->nfc->semifar_func &&
                  _check_ancestor_semifar (ref_info, node_info,
                                           niaf->semifar_threshold_height))
                {
                  /* g_printerr ("%d : skipping semifar by ancestry ref[%#@kmod@x %#@kmod@x %#@kmod@x %d] node[%#lx %#lx %#lx %d] dist=%d " */
                  /*             "th_height=%d\n", */
                  /*             niaf->nfc->rk, */
                  /*             ref_info->id.x, ref_info->id.y, ref_info->id.z, ref_info->depth, */
                  /*             node_info->id.x, node_info->id.y, node_info->id.z, node_info->depth, */
                  /*             vsg_prtree_key3@t@_deepest_distance (&ref_info->id, &node_info->id), */
                  /*             niaf->semifar_threshold_height); */
                  return;
                }

              if (ref_info->point_count > 0)
                vsg_prtree3@t@node_recursive_near_func (niaf->ref->node,
                                                        ref_info, node,
                                                        node_info, niaf->nfc);

              /* compute  */
              wv = niaf->ref->next;
              while (wv != NULL)
                {
                  if (PRTREE3@T@NODE_ISLEAF (wv->node))
                    {
                      _vsg_prtree3@t@node_get_info (wv->node, &info, NULL, 0);
                      memcpy (&info.id, &wv->id, sizeof (VsgPRTreeKey3@t@));

                      info.depth = info.id.depth;

                      vsg_prtree3@t@node_recursive_near_func (wv->node, &info,
                                                              node, node_info, 
                                                              niaf->nfc);
                    }

                   wv = wv->next;
                }
              niaf->done_flag |= 1;
            }
        }
      else if (node_depth == ref_info->id.depth &&
               PRTREE3@T@NODE_IS_PRIVATE_LOCAL (node) &&
               node_info->point_count != 0)
        {
          niaf->nfc->far_func (ref_info, node_info, niaf->nfc->user_data);
          niaf->done_flag |= 1<<1;
        }
    }
}

static vsgrloc3 _selector_nf_visitor (VsgPRTree3@t@NodeInfo *ref_info,
                                      VsgPRTree3@t@NodeInfo *node_info,
                                      VsgPRTreeKey3@t@ *ref_ancestry_ids)
{
  static const vsgrloc3 ancestor_order_rlocs[] = {
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (0)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (1)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (2)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (3)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (4)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (5)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (6)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (7)-1),
  };
  VsgPRTreeKey3@t@ *node_key = &node_info->id;
  gint d = node_key->depth;

  if (d >= ref_info->id.depth)
    {
/*       if (ref_info->point_count == 0/\*  || d > ref_info->id.depth *\/) */
        return 0x0;

      return VSG_RLOC3_MASK;
    }

  if (vsg_prtree_key3@t@_is_neighbour (&ref_ancestry_ids[d], node_key))
    {
      if (vsg_prtree_key3@t@_equals (&ref_ancestry_ids[d], node_key))
        {
          vsgloc3 loc = (ref_ancestry_ids[d+1].x & 1) |
            ((ref_ancestry_ids[d+1].y & 1) << 1) |
            ((ref_ancestry_ids[d+1].z & 1) << 2);

          return ancestor_order_rlocs[loc];
        }
      return VSG_RLOC3_MASK;
    }

  return 0x0;
}

static void
_prtree3@t@node_traverse_visiting_nf (VsgPRTree3@t@Node *node,
                                      VsgPRTree3@t@NodeInfo *father_info,
                                      vsgloc3 child_number,
                                      NIAndFuncs *niaf,
                                      VsgPRTreeKey3@t@ *ref_ancestry_ids)
{
  VsgPRTree3@t@NodeInfo node_info;
  guint8 i;
  vsgrloc3 ipow;
  vsgrloc3 locmask;

  _vsg_prtree3@t@node_get_info (node, &node_info, father_info, child_number);

  locmask = _selector_nf_visitor (&niaf->ref_info, &node_info,
                                  ref_ancestry_ids);

  if (PRTREE3@T@NODE_ISINT (node))
    {
      for (i=0; i<CN; i++)
        {
          ipow = VSG_RLOC3_COMP (i);

          if (ipow & locmask)
            _prtree3@t@node_traverse_visiting_nf
              (PRTREE3@T@NODE_CHILD(node, i), &node_info, i, niaf,
               ref_ancestry_ids);
        }
    }

  _traverse_visiting_nf (node, &node_info, niaf);
}

/* static gint _visitors = 0; */
/* static gint _unused_visitors = 0; */

/*
 * operates a traversal of near/far interactions for a visiting node.
 */
static gboolean _compute_visiting_node (VsgNFConfig3@t@ *nfc,
                                        WaitingVisitor *wv)
{
  NIAndFuncs niaf;
  VsgPRTreeKey3@t@ ref_ancestry_ids[sizeof (@key_type@) * 8];
  gint i;

  niaf.ref = wv;
  niaf.nfc = nfc;
  niaf.semifar_threshold_height = wv->semifar_threshold_height;

  _vsg_prtree3@t@node_get_info (wv->node, &niaf.ref_info, NULL, 0);
  memcpy (&niaf.ref_info.id, &wv->id, sizeof (VsgPRTreeKey3@t@));

  niaf.ref_info.depth = niaf.ref_info.id.depth;

  /* no empty leaf node except fake leaves should be sent here, so an empty
     visitor must be a interior node */
  if (wv->node->point_count == 0 && wv->next == NULL)
    niaf.ref_info.isleaf = FALSE;

  vsg_prtree_key3@t@_copy (&ref_ancestry_ids[niaf.ref_info.id.depth],
                           &niaf.ref_info.id);
  for (i = niaf.ref_info.id.depth-1; i >= 0; i --)
    {
      vsg_prtree_key3@t@_get_father (&ref_ancestry_ids[i+1],
                                     &ref_ancestry_ids[i]);
    }

  niaf.ref_ancestry_ids = ref_ancestry_ids;

  niaf.done_flag = 0;

  _prtree3@t@node_traverse_visiting_nf (nfc->tree->node, NULL, 0, &niaf,
                                        ref_ancestry_ids);

  if (niaf.done_flag == 0)
    {
        WaitingVisitor *wv2 = wv;
/*       g_printerr ("%d : reject node [", nfc->rk); */
/*       vsg_prtree_key3@t@_write (&wv->id, stderr); */
/*       g_printerr ("]\n"); */

      VsgNFProcMsg *nfpm = vsg_nf_config3@t@_proc_msgs_lookup (nfc, wv->src);


      do {
        nfpm->dropped_visitors ++;
        _visiting_node_free (wv2->node, &nfc->tree->config);
        wv2 = wv2->next;
      } while (wv2);

      _waiting_visitor_free (wv);

      return FALSE;
    }
/*   else */
/*     { */
/*       g_printerr ("%d : accepted node [", nfc->rk); */
/*       vsg_prtree_key3@t@_write (&wv->id, stderr); */
/*       g_printerr ("]\n"); */
     
/*     } */

  return TRUE;
}

/*
 * checks for all completed VsgNFPocMsg requests and fills the
 * requests with pending msgs.
 */
gint vsg_prtree3@t@_nf_check_send (VsgNFConfig3@t@ *nfc)
{
  gint sent = 0;
  gint flag = FALSE;
  gint index = -1;

  MPI_Testany (nfc->sz, nfc->procs_requests, &index, &flag, MPI_STATUS_IGNORE);

  while (flag && index != MPI_UNDEFINED)
    {
      VsgNFProcMsg *nfpm = vsg_nf_config3@t@_proc_msgs_lookup (nfc, index);

      if (nfpm->backward_pending != NULL)
        {
          /* First, check for backward messages */
          _send_pending_backward_node (nfc, nfpm, index);
          sent ++;
        }
      else if (nfpm->forward_pending != NULL)
        {
          /* Fallback into forward message */
          _send_pending_forward_node (nfc, nfpm, index);
          sent ++;
        }

      MPI_Testany (nfc->sz, nfc->procs_requests, &index, &flag,
                   MPI_STATUS_IGNORE);

    }

  return sent;
}

typedef struct _FinalDroppedData FinalDroppedData;
struct _FinalDroppedData {
  VsgNFConfig3@t@ *nfc;
  gint remaining;
};

/*
 * ensures a specific VsgNFProcMsg has sent its count of dropped visitors
 */
static void _foreach_send_dropped_visitors (gpointer key, VsgNFProcMsg *nfpm,
                                            FinalDroppedData *fdd)
{
  if (nfpm->dropped_visitors > 0)
    {
      gint dst = GPOINTER_TO_INT (key);
      VsgPRTreeKey3@t@ k = vsg_prtree_key3@t@_root;
      gint flag;

      MPI_Test (&fdd->nfc->procs_requests[dst], &flag, MPI_STATUS_IGNORE);

      if (!flag)
        {
          fdd->remaining ++;
          return;
        }

      nfpm->send_pm.position = 0;

      /* issue a false key followed with number of dropped visitors */
      vsg_packed_msg_send_append (&nfpm->send_pm, &k, 1,
                                  VSG_MPI_TYPE_PRTREE_KEY3@T@);
      vsg_packed_msg_send_append (&nfpm->send_pm, &nfpm->dropped_visitors, 1,
                                  MPI_INT);
      nfpm->dropped_visitors = 0;

      _nf_msg_isend (&nfpm->send_pm, dst, VISIT_BACKWARD_TAG,
                     &fdd->nfc->procs_requests[dst]);
    }
}

static void _send_dropped_visitors (VsgNFConfig3@t@ *nfc)
{
  FinalDroppedData fdd = {nfc, 1};

  while (fdd.remaining > 0)
    {
      fdd.remaining = 0;
      g_hash_table_foreach (nfc->procs_msgs,
                            (GHFunc) _foreach_send_dropped_visitors,
                            &fdd);
    }
}

static gint _dropped_count = 0;
/*
 * Probes for incoming messages. When called with @blocking == TRUE, will wait
 * until any receive happens. The time spent waiting will be used in
 * pending computations (of visitors) or in sending pending messages.
 */
gboolean vsg_prtree3@t@_nf_check_receive (VsgNFConfig3@t@ *nfc, gint tag,
                                          gboolean blocking)
{
  gint flag = FALSE;
  VsgPRTree3@t@Config *config = &nfc->tree->config;
  VsgPRTreeParallelConfig * pc = &config->parallel_config;
  MPI_Status status;
  gint received = 0;

  if (nfc->recv_lock) return FALSE;

  MPI_Iprobe (MPI_ANY_SOURCE, tag, pc->communicator, &flag, &status);

  if (blocking)
    {
      while (!flag)
        {
          if ((nfc->forward_pending_nb + nfc->backward_pending_nb) > 0 &&
              vsg_prtree3@t@_nf_check_send (nfc) == 0)
            {
              /* fallback asleep for just a moment before rechecking */
              g_usleep (1);
            }

          MPI_Iprobe (MPI_ANY_SOURCE, tag, pc->communicator, &flag, &status);
        }
    }

  while (flag)
    {
      VsgPRTreeKey3@t@ id;
      VsgPRTree3@t@Node *node;
      WaitingVisitor *wv;

/*       g_printerr ("%d : check_recv begin from %d tag=%d\n", nfc->rk, */
/*                   status.MPI_SOURCE, status.MPI_TAG); */

      flag = FALSE;
      received ++;

      nfc->recv_lock = TRUE;
      vsg_packed_msg_recv_probed (&nfc->recv, &status);

/*       g_printerr ("%d : check_recv recv completed\n", nfc->rk); */

      switch (status.MPI_TAG) {
      case VISIT_FORWARD_TAG:
        {
          WaitingVisitor *root = NULL;
          WaitingVisitor *parent = NULL;
          NodePackData npd = NPD_VISIT_FORWARD (pc, &nfc->recv);
          guint8 semifar_threshold_height;

          do {
            vsg_packed_msg_recv_read (&nfc->recv, &id, 1,
                                      VSG_MPI_TYPE_PRTREE_KEY3@T@);

            if (root == NULL)
              vsg_packed_msg_recv_read (&nfc->recv, &semifar_threshold_height, 1,
                                        MPI_UNSIGNED_CHAR);
            else semifar_threshold_height = 0;

            node = _new_visiting_node (nfc->tree, &id, status.MPI_SOURCE);

            wv = _waiting_visitor_new (node, &id, FALSE, status.MPI_SOURCE,
                                       semifar_threshold_height);

            _node_unpack (node, &npd);

            /* g_printerr ("%d : unpack [%#@kmod@x %#@kmod@x %#@kmod@x %d] with semifar height=%u depth=%d\n", nfc->rk, id.x, id.y, id.z, id.depth, semifar_threshold_height, id.depth); */

            if (root == NULL) root = wv;
            if (parent == NULL) parent = wv;
            else
              {
                parent->next = wv;
                parent = wv;
              }

          } while (nfc->recv.position < nfc->recv.size);

          /* compute nf interactions with local tree */
          if (_compute_visiting_node (nfc, root))
              _propose_node_backward (nfc, status.MPI_SOURCE, root);
          else
            vsg_prtree3@t@_nf_check_send (nfc);

          nfc->all_fw_recvs ++;

        }
        break;
      case VISIT_BACKWARD_TAG:

        do {
          vsg_packed_msg_recv_read (&nfc->recv, &id, 1,
                                    VSG_MPI_TYPE_PRTREE_KEY3@T@);

          /* detect special key for dropped visitors */
          if (id.depth == 0)
            {
              gint i;
              vsg_packed_msg_recv_read (&nfc->recv, &i, 1, MPI_INT);
              nfc->pending_backward_msgs -= i;

              _dropped_count += i;
/*             g_printerr ("%d(%d) : bw dropped recv from %d - %d (%d left) ", */
/*                         nfc->rk, getpid (), status.MPI_SOURCE, i, */
/*                         nfc->pending_backward_msgs); */
/*             vsg_prtree_key3@t@_write (&id, stderr); */
/*             g_printerr ("\n"); */
/*             fflush (stderr); */

              /* if end of message reached, break */
              if (nfc->recv.position >= nfc->recv.size) break;

              /* else: unpack following node key */
              vsg_packed_msg_recv_read (&nfc->recv, &id, 1,
                                        VSG_MPI_TYPE_PRTREE_KEY3@T@);
            }

          node = vsg_prtree3@t@node_key_lookup (nfc->tree->node, id);

          /* g_printerr ("%d : bw node recv %#@kmod@x %#@kmod@x %#@kmod@x d=%d\n", nfc->rk, id.x, id.y, id.z, id.depth); */
          /* g_printerr ("%d : bw node recv %g %g %g\n", nfc->rk, node->center.x, node->center.y, node->center.z); */

/*         g_printerr ("%d(%d) : bw recv from %d - ", nfc->rk, getpid (), */
/*                     status.MPI_SOURCE); */
/*         vsg_prtree_key3@t@_write (&id, stderr); */
/*         g_printerr ("\n"); */
/*         fflush (stderr); */

        // *** PRIVATE-LOCAL
        g_assert (PRTREE3@T@NODE_IS_PRIVATE_LOCAL (node));
          {
            NodePackData npd = NPD_VISIT_BACKWARD (pc, &nfc->recv);
            _node_unpack_and_reduce (node, &npd, nfc);
          }

          nfc->pending_backward_msgs --;
/*             g_printerr ("bw recv(%d)\n", nfc->rk); */
/*         fflush (stderr); */

          nfc->all_bw_recvs ++;

        } while (nfc->recv.position < nfc->recv.size);

        /* check for other incoming messages */
        MPI_Iprobe (MPI_ANY_SOURCE, tag, pc->communicator, &flag, &status);

        break;
      case END_FW_TAG:
        nfc->pending_end_forward --;
        break;
      default:
        g_error ("Unrecognized Message TAG %d.", status.MPI_TAG);
        break;

      }

/*       g_printerr ("%d : check_recv end %d %d %d\n", nfc->rk, */
/*                   nfc->pending_backward_msgs, */
/*                   nfc->forward_pending_nb, */
/*                   nfc->backward_pending_nb); */
      nfc->recv_lock = FALSE;

    }

  return received != 0;
}

typedef struct _NodeRemoteData NodeRemoteData;
struct _NodeRemoteData
{
  VsgNFConfig3@t@ *nfc;
  VsgPRTree3@t@Node *ref_node;
  VsgPRTree3@t@NodeInfo *ref_info;
  VsgPRTreeKey3@t@ *ref_ancestry_ids;
  gboolean *procs;
  gboolean sent;
  gboolean fake_leaf;
};

static vsgrloc3 _selector_nf_remote (VsgPRTree3@t@NodeInfo *ref_info,
                                     VsgPRTree3@t@NodeInfo *node_info,
                                     VsgPRTreeKey3@t@ *ref_ancestry_ids)
{
  static const vsgrloc3 ancestor_order_rlocs[] = {
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (0)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (1)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (2)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (3)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (4)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (5)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (6)-1),
    VSG_RLOC3_MASK - (VSG_RLOC3_COMP (7)-1),
  };
  VsgPRTreeKey3@t@ *node_key = &node_info->id;
  gint d = node_key->depth;

  if (d >= ref_info->id.depth)
    {
      /* we can't skip non leaf nodes like in the following because
         all ref_info's subtree could be skipped (see parallel_check in
         vsgprrtee3@t@-extras.c:vsg_prtree3@t@node_near_far_traversal()) */
      /* if (!ref_info->isleaf) return 0x0; */

      return VSG_RLOC3_MASK;
    }

  if (vsg_prtree_key3@t@_is_neighbour (&ref_ancestry_ids[d], node_key))
    {
      if (VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_LOCAL (node_info)) return 0x0;

      if (vsg_prtree_key3@t@_equals (&ref_ancestry_ids[d], node_key))
        {
          vsgloc3 loc = (ref_ancestry_ids[d+1].x & 1) |
            ((ref_ancestry_ids[d+1].y & 1) << 1) |
            ((ref_ancestry_ids[d+1].z & 1) << 2);

          return ancestor_order_rlocs[loc];
        }

      return VSG_RLOC3_MASK;
    }

  return 0x0;
}

static inline @key_type@ _key_coord_dist (@key_type@ ref, @key_type@ node,
                                          @key_type@ clamped, guint8 height)
{
  if (node == ref)
    return 0;

  if (node > ref)
    return (1<<(height)) - clamped;

  return clamped;
}

/*
 * compute which ancestry level exceeds semifar_threshold.
 */
static guint8 _semifar_threshold_height (VsgPRTree3@t@NodeInfo *node_info, VsgNFConfig3@t@ *nfc)
{
  if (node_info == NULL) return 0;

  // *** SHARED-* (should be LOCAL only, though)
  if (VSG_PRTREE3@T@_NODE_INFO_IS_SHARED (node_info)) return 0;

  if (node_info->point_count >= nfc->semifar_threshold) return 0;

  return _semifar_threshold_height (node_info->father_info, nfc) + 1;
}

/*
 * used in vsg_prtree3@t@_traverse to register which processors a node is to
 * be sent to for a near/far interaction.
 */
static void _traverse_check_remote_neighbours (VsgPRTree3@t@Node *node,
                                               VsgPRTree3@t@NodeInfo *node_info,
                                               NodeRemoteData *data)
{
  // *** PRIVATE-REMOTE
  if (PRTREE3@T@NODE_IS_PRIVATE_REMOTE (node))
    {
      gint proc = PRTREE3@T@NODE_PROC (node);
      VsgPRTree3@t@NodeInfo *ref_info = data->ref_info;
      guint8 node_depth;

      if (data->procs[proc]) return;

      node_depth = node_info->id.depth;

      if (node_depth <= ref_info->id.depth)
        {
          VsgPRTreeKey3@t@ *ref_id = &data->ref_ancestry_ids[node_depth];

          if (! vsg_prtree_key3@t@_is_neighbour (ref_id, &node_info->id))
            {
              if (node_depth < ref_info->id.depth)
                return;
            }
          else
            {
              if (node_depth < ref_info->id.depth &&
                  PRTREE3@T@NODE_LEAF (node).remote_depth > 0)
                {
                  /* check for mindepth */
                  VsgPRTreeKey3@t@ truncated_ref;
                  guint8 mindepth;
                  guint8 shift;
                  @key_type@ dist;

                  /* use remote depth to avoid unwanted fw sends */
                  mindepth = node_info->depth +
                    PRTREE3@T@NODE_LEAF (node).remote_depth;

                  /* don't check deeper than node's depth */
                  mindepth = MIN (mindepth, data->ref_info->depth);

                  shift = ref_info->id.depth - mindepth;

                  vsg_prtree_key3@t@_truncate (&ref_info->id, shift, &truncated_ref);

                  dist = vsg_prtree_key3@t@_deepest_distance (&truncated_ref, &node_info->id);

                  if (dist > 2) return;
                }
            }
        }
      else
        if (ref_info->point_count == 0) return;

      _propose_node_forward (data->nfc, proc, data->ref_node,
                             &data->ref_info->id, data->fake_leaf,
                             _semifar_threshold_height (data->ref_info, data->nfc));

      /* g_printerr ("%d : semifar height: %u depth=%d\n", data->nfc->rk, */
      /*             _semifar_threshold_height (data->ref_info, data->nfc), */
      /*             data->ref_info->id.depth); */

      data->procs[proc] = TRUE;
      data->sent = TRUE;

      return;

    }
}
static void
_prtree3@t@node_traverse_check_parallel (VsgPRTree3@t@Node *node,
                                         VsgPRTree3@t@NodeInfo *father_info,
                                         vsgloc3 child_number,
                                         NodeRemoteData *nrd,
                                         VsgPRTreeKey3@t@ *ref_ancestry_ids)
{
  VsgPRTree3@t@NodeInfo node_info;
  guint8 i;
  vsgrloc3 ipow;

  _vsg_prtree3@t@node_get_info (node, &node_info, father_info, child_number);

  _traverse_check_remote_neighbours (node, &node_info, nrd);

  if (PRTREE3@T@NODE_ISINT (node))
    {
      vsgrloc3 locmask = _selector_nf_remote (nrd->ref_info, &node_info,
                                              ref_ancestry_ids);

      for (i=0; i<CN; i++)
        {
          ipow = VSG_RLOC3_COMP (i);

          if (ipow & locmask)
            _prtree3@t@node_traverse_check_parallel
              (PRTREE3@T@NODE_CHILD(node, i), &node_info, i, nrd,
               ref_ancestry_ids);
        }
    }
}

/*
 * checks wether some specified node is to be sent to distant processors in
 * order to complete a near/far traversal. Returns TRUE if node id
 * LOCAL and was sent to another processor.
 */
gboolean
vsg_prtree3@t@_node_check_parallel_near_far (VsgNFConfig3@t@ *nfc,
                                             VsgPRTree3@t@Node *node,
                                             VsgPRTree3@t@NodeInfo *info,
                                             gboolean do_traversal)
{
  static gint _virtual_shared_warning_done = 0;

  gboolean ret = do_traversal;
  if (nfc->sz < 2) return FALSE;

  vsg_prtree3@t@_nf_check_receive (nfc, MPI_ANY_TAG, FALSE);

  if (!do_traversal) return ret;

  // *** PRIVATE-LOCAL
  if (VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_LOCAL (info))
    {
      gint i;
      NodeRemoteData nrd;
      VsgPRTreeKey3@t@ ref_ancestry_ids[sizeof (@key_type@) * 8];

      nrd.nfc = nfc;
      nrd.ref_node = node;
      nrd.ref_info = info;

      vsg_prtree_key3@t@_copy (&ref_ancestry_ids[info->id.depth], &info->id);
      for (i = info->id.depth-1; i >= 0; i --)
        vsg_prtree_key3@t@_get_father (&ref_ancestry_ids[i+1],
                                       &ref_ancestry_ids[i]);

      nrd.ref_ancestry_ids = ref_ancestry_ids;

      nrd.procs = g_alloca (nfc->sz * sizeof (gboolean));
      memset (nrd.procs, 0, nfc->sz * sizeof (gboolean));
      nrd.sent = FALSE;
      nrd.fake_leaf = (!info->isleaf) &&
        PRTREE3@T@_NODE_INFO_NF_ISLEAF (info, &nfc->tree->config);

      _prtree3@t@node_traverse_check_parallel (nfc->tree->node, NULL, 0, &nrd,
                                               ref_ancestry_ids);

      ret = nrd.sent && ! nrd.fake_leaf;
    }
  // *** SHARED-* (should be LOCAL only, though)
  else if (VSG_PRTREE3@T@_NODE_INFO_IS_SHARED (info) &&
           nfc->tree->config.nf_isleaf (info,
                                        nfc->tree->config.nf_isleaf_data))
    {
      if (_virtual_shared_warning_done == 0)
        {
          g_warning ("%d : virtual-leaf shared node not handled in parallel "
                     "near/far traversal. Results will be unpredictable.",
                     nfc->rk);
          _virtual_shared_warning_done ++;
        }
    }

  vsg_prtree3@t@_nf_check_send (nfc);

  return ret;
}

typedef struct _NodeDataReduce NodeDataReduce;

struct _NodeDataReduce {
  VsgPackedMsg *msg;
  VsgParallelMigrateVTable *data_vtable;
  gpointer tmp_node_data;
};

/*
 * used in a traversal to unpack all shared nodes of a tree from a VsgPackedMsg
 * in a backward direction (that is: supposedly accumulating/reducing with the
 * output part of the near/far interaction).
 */
static void _unpack_shared (VsgPRTree3@t@Node *node,
                            VsgPRTree3@t@NodeInfo *node_info,
                            NodeDataReduce *ndr)
{
  // *** SHARED-*
  if (PRTREE3@T@NODE_IS_SHARED (node))
    {
/*       gint rk; */
/*       MPI_Comm_rank (MPI_COMM_WORLD, &rk); */

/*       g_printerr ("%d : unpacking shared ", rk); */
/*       vsg_prtree_key3@t@_write (&node_info->id, stderr); */
/*       g_printerr ("\n"); */
      ndr->data_vtable->unpack (ndr->tmp_node_data, ndr->msg,
                                ndr->data_vtable->unpack_data);

      ndr->data_vtable->reduce (ndr->tmp_node_data, node->user_data,
                                ndr->data_vtable->reduce_data);
    }
}

/*
 * used in a traversal to pack all shared nodes of a tree into a VsgPackedMsg
 * in a backward direction (that is: supposedly accumulating/reducing with the
 * output part of the near/far interaction).
 */
static void _pack_shared (VsgPRTree3@t@Node *node,
                          VsgPRTree3@t@NodeInfo *node_info,
                          NodeDataReduce *ndr)
{
  // *** SHARED-*
  if (PRTREE3@T@NODE_IS_SHARED (node))
    {
/*       gint rk; */
/*       MPI_Comm_rank (MPI_COMM_WORLD, &rk); */

/*       g_printerr ("%d : packing shared ", rk); */
/*       vsg_prtree_key3@t@_write (&node_info->id, stderr); */
/*       g_printerr ("\n"); */
      ndr->data_vtable->pack (node->user_data, ndr->msg,
                              ndr->data_vtable->pack_data);

    }
}

static void
_shared_nodes_allreduce_internal (VsgPRTree3@t@ *tree,
                                  VsgParallelMigrateVTable *data_vtable,
                                  gpointer tmp_node_data)
{
  gint rk, sz;
  VsgPackedMsg send_msg =
    VSG_PACKED_MSG_STATIC_INIT (tree->config.parallel_config.communicator);
  VsgPackedMsg recv_msg =
    VSG_PACKED_MSG_STATIC_INIT (tree->config.parallel_config.communicator);

  NodeDataReduce ndr_send = {&send_msg, data_vtable, tmp_node_data};
  NodeDataReduce ndr_recv = {&recv_msg, data_vtable, tmp_node_data};
  MPI_Request requests[2] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL};
  gint step = 0;
  gint maxoffset, offset;
  gint quo;
  gint mod, div, src, dst;

  MPI_Comm_rank (tree->config.parallel_config.communicator, &rk);
  MPI_Comm_size (tree->config.parallel_config.communicator, &sz);

  while ((1<<step) < sz)
    step ++;

  maxoffset = 1<<(step-1);

/*   g_printerr ("%d : allreduce steps=%d maxoffset=%d\n", rk, step, */
/*               maxoffset); */

  while (step > 0)
    {
      step --;
      offset = 1 << step;
      quo = offset << 1;
      mod = (rk + offset) % quo;
      div = rk / quo;

      dst = mod + div * quo;
      src = dst;
      if (src >= sz) src = src % maxoffset;

      if (dst < sz && dst != rk)
        {
          /* reinit msg for packing a new message */
          send_msg.position = 0;

          /* fill ndr_send with shared nodes */
          vsg_prtree3@t@_traverse_custom_internal (tree, G_PRE_ORDER,
                                                   (VsgRegion3@t@InternalLocDataFunc)
                                                   _selector_skip_local_nodes,
                                                   NULL, NULL,
                                                   (VsgPRTree3@t@InternalFunc)
                                                   _pack_shared,
                                                   &ndr_send);

          /* send to first destination */
/*           g_printerr ("%d : allreduce sending (step %d) to %d\n", rk, */
/*                       step, dst); */
          vsg_packed_msg_isend (&send_msg, dst, VISIT_SHARED_TAG, &requests[0]);

          requests[1] = MPI_REQUEST_NULL;

          /* try alternate destination */
          dst += maxoffset;
          if (dst < sz && dst !=rk)
            {
              int dstmod = (dst + offset) % quo;
              int dstdiv = dst / quo;
              int dstsrc = dstmod + dstdiv * quo;

              /* send to alternate destination only if it has no
                 communiocation this step */
              if (dstsrc >= sz)
                {
/*                   g_printerr ("%d : allreduce sending2 (step %d) to %d\n", */
/*                               rk, step, dst); */
                  vsg_packed_msg_isend (&send_msg, dst, VISIT_SHARED_TAG,
                                        &requests[1]);
                }
            }
        }

      if (src != rk)
        {
          /* receive from source */
/*           g_printerr ("%d : allreduce recv (step %d) from %d\n", rk, */
/*                       step, src); */
          vsg_packed_msg_recv (&recv_msg, src, VISIT_SHARED_TAG);

          /* add results to shared nodes */
          vsg_prtree3@t@_traverse_custom_internal (tree, G_PRE_ORDER,
                                                   (VsgRegion3@t@InternalLocDataFunc)
                                                   _selector_skip_local_nodes,
                                                   NULL, NULL,
                                                   (VsgPRTree3@t@InternalFunc)
                                                   _unpack_shared,
                                                   &ndr_recv);
        }
/*       else */
/*           g_printerr ("%d : allreduce no recv\n", rk); */

      MPI_Waitall (2, requests, MPI_STATUSES_IGNORE);
/*       g_printerr ("%d : allreduce (step %d) ok\n", rk, step); */
    }

  vsg_packed_msg_drop_buffer (&send_msg);
  vsg_packed_msg_drop_buffer (&recv_msg);
}

/**
 * vsg_prtree3@t@_shared_nodes_allreduce:
 * @tree: A #VsgPRTree3@t@.
 * @data_vtable: A #VsgParallelMigrateVTable specifying som reduction operator.
 *
 * copies the semantic of the MPI_Allreduce operation on user_data for all
 * shared nodes of a tree. Reduction operator is given by the user
 * through @data_vtable.
 *
 * NOTE: this is a collective communication function.
 *
 * WARNING: only available when VSG_HAVE_MPI is defined.
 */
void
vsg_prtree3@t@_shared_nodes_allreduce (VsgPRTree3@t@ *tree,
                                       VsgParallelMigrateVTable *data_vtable)
{
  VsgPRTreeParallelConfig *pconfig = &tree->config.parallel_config;
  gpointer tmp_node_data = NULL;

  if (pconfig->communicator == MPI_COMM_NULL) return;

  if (data_vtable->pack == NULL || data_vtable->unpack == NULL) return;

  if (pconfig->node_data.alloc != NULL)
    {
      tmp_node_data =
        pconfig->node_data.alloc (FALSE, pconfig->node_data.alloc_data);
    }

  _shared_nodes_allreduce_internal (tree, data_vtable, tmp_node_data);

  if (pconfig->node_data.destroy != NULL)
    {
      pconfig->node_data.destroy (tmp_node_data, FALSE,
                                  pconfig->node_data.destroy_data);
    }
}

/*
 * finishes all communication and computations involved in a parallel near/far
 * interaction.
 */
void vsg_prtree3@t@_nf_check_parallel_end (VsgNFConfig3@t@ *nfc)
{
  MPI_Comm comm = nfc->tree->config.parallel_config.communicator;
  gint i, dst;
  VsgParallelMigrateVTable *data_bw_vtable =
    &nfc->tree->config.parallel_config.node_data.visit_backward;
  VsgPackedMsg pm[nfc->sz];
  MPI_Request end_fw_reqs[nfc->sz];
  /* GTimer *timer = g_timer_new (); */

/*   g_printerr ("%d(%d) : parallel_end begin (fw pending wv=%d) (bw pending wv=%d)\n", */
/*               nfc->rk, getpid (), */
/*               nfc->forward_pending_nb, */
/*               nfc->backward_pending_nb); */

  while (nfc->forward_pending_nb > 0)
    {
      vsg_prtree3@t@_nf_check_receive (nfc, MPI_ANY_TAG, FALSE);
      vsg_prtree3@t@_nf_check_send (nfc);
    }

  end_fw_reqs[nfc->rk] = MPI_REQUEST_NULL;

  /* vsg_packed_msg_trace ("enter 2 [end fw send]"); */

  for (i=1; i<nfc->sz; i++)
    {
      dst = (nfc->rk+i) % nfc->sz;
      vsg_packed_msg_init (&pm[dst], comm);
      _nf_msg_isend (&pm[dst], dst, END_FW_TAG, &end_fw_reqs[dst]);
    }

  /* wait for all END_FW messages to be correctly sent */
  i=nfc->sz-1;
  while (i > 0)
    {
      gint index, flag;

      MPI_Testany (nfc->sz, end_fw_reqs, &index, &flag, MPI_STATUS_IGNORE);
      if (flag)
        {
          i --;
        }
      else
        {
          vsg_prtree3@t@_nf_check_send (nfc);
          /* fall asleep if no message is incoming */
          if (! vsg_prtree3@t@_nf_check_receive (nfc, MPI_ANY_TAG, FALSE))
            g_usleep (1);
        }
    }

  for (i=1; i<nfc->sz; i ++)
    {
      dst = (nfc->rk+i) % nfc->sz;
      vsg_packed_msg_drop_buffer (&pm[dst]);
    }

  /* vsg_packed_msg_trace ("leave 2 [end fw send]"); */
  /* vsg_packed_msg_trace ("enter 2 [end fw recv]"); */

/*   g_printerr ("%d : end fw sent (elapsed %f)\n", nfc->rk, */
/*               g_timer_elapsed (timer, NULL)); */

  /* check all remaining messages */
  while (nfc->pending_end_forward > 0)
    {
/*     g_printerr ("%d : check %d\n", nfc->rk,  nfc->end_forward_received); */
      vsg_prtree3@t@_nf_check_receive (nfc, MPI_ANY_TAG, TRUE);
    }

/*   g_printerr ("%d : end fw received (elapsed %f)\n", nfc->rk, */
/*               g_timer_elapsed (timer, NULL)); */

  /* vsg_packed_msg_trace ("leave 2 [end fw recv]"); */

  /* now, no forward visitor should be left incoming */

  /* vsg_packed_msg_trace ("enter 2 [end bw] # fwp=%d bwp=%d, pbw=%d", */
  /*                       nfc->forward_pending_nb, nfc->backward_pending_nb, */
  /*                       nfc->pending_backward_msgs); */

  /* do all remaining stuff */
  while ((nfc->forward_pending_nb + nfc->backward_pending_nb) > 0)
    {
/*       g_printerr ("%d : pending bw %d\n", */
/*                   nfc->rk, nfc->pending_backward_msgs); */
      vsg_prtree3@t@_nf_check_send (nfc);
      vsg_prtree3@t@_nf_check_receive (nfc, MPI_ANY_TAG, FALSE);
    }

  /* send information on all remaining dropped visitors */
  _send_dropped_visitors (nfc);

/*   g_printerr ("%d : all bw send ok (elapsed %f) (dropped %d)\n", nfc->rk, */
/*               g_timer_elapsed (timer, NULL), _dropped_count); */

  while (nfc->pending_backward_msgs > 0)
    {
      vsg_prtree3@t@_nf_check_receive (nfc, MPI_ANY_TAG, TRUE);
    }

  /* g_printerr ("%d : pending bw recv ok elapsed=%g seconds\n", nfc->rk, */
  /*             g_timer_elapsed (timer, NULL)); */

  MPI_Waitall (nfc->sz, nfc->procs_requests, MPI_STATUS_IGNORE); 

  /* vsg_packed_msg_trace ("leave 2 [end bw]"); */

  MPI_Barrier (comm);

  /* vsg_packed_msg_trace ("enter 2 [allreduce]"); */

/*   g_printerr ("%d : allreduce begin\n", nfc->rk); */

  if (data_bw_vtable->pack != NULL && data_bw_vtable->unpack != NULL)

    _shared_nodes_allreduce_internal (nfc->tree, data_bw_vtable,
                                      nfc->tmp_node_data);

  /* g_printerr ("%d : parallel_end done elapsed=%g seconds\n", nfc->rk, */
  /*             g_timer_elapsed (timer, NULL)); */

/*   g_printerr ("%d : unused visitors %d (%d total)\n", nfc->rk, */
/*               _unused_visitors, _visitors); */

/*   g_printerr ("%d : nodes msgs (fw send=%d recv=%d) (bw send=%d recv=%d)\n", */
/*               nfc->rk, nfc->all_fw_sends, nfc->all_fw_recvs, nfc->all_bw_sends, */
/*               nfc->all_bw_recvs); */

/*   g_printerr ("%d : pending bw stats: max=%d avg=%f nb=%d\n", nfc->rk, */
/*               _bw_pending_max, */
/*               _bw_pending_sum / (gdouble) _bw_pending_calls, */
/*               _bw_pending_calls); */

/*   g_timer_destroy (timer); */

  /* vsg_packed_msg_trace ("leave 2 [allreduce]"); */
}

static guint8 _prtree3@t@node_mindepth (const VsgPRTree3@t@Node *node,
                                        VsgPRTree3@t@NodeInfo *node_info,
                                        VsgPRTree3@t@Config *config)
{
  guint8 res = 8 * sizeof (@type@); /* max depth is the number of bits of
                                       @type@ */
  vsgloc3 i;

  /* empty nodes can be omitted here: return /infinite/ depth */
  if (node->point_count == 0)
    return res;

  /* if (PRTREE3@T@NODE_ISLEAF (node)) return 0; */
  if (PRTREE3@T@_NODE_INFO_NF_ISLEAF (node_info, config)) return 0;
  for (i=0; i<CN; i++)
    {
      guint8 tmp;
      VsgPRTree3@t@Node *child = PRTREE3@T@NODE_CHILD (node, i);
      VsgPRTree3@t@NodeInfo child_info;

      _vsg_prtree3@t@node_get_info (child, &child_info, node_info, i);

      tmp = _prtree3@t@node_mindepth (child, &child_info, config);

       if (tmp < res) res = tmp;
    }

  return (res == G_MAXUINT8) ? res : res + 1;
}

typedef struct _ArrayAndConfig ArrayAndConfig;
struct _ArrayAndConfig {
  GArray *array;
  VsgPRTree3@t@Config *config;
};

static void _remote_depths_array_build (VsgPRTree3@t@Node *node,
                                        VsgPRTree3@t@NodeInfo *node_info,
                                        ArrayAndConfig *aac)
{
  // *** == PRIVATE-*
  if (PRTREE3@T@NODE_IS_PRIVATE (node))
    {
      /* detect root of local (remote) subtrees */
      // *** father == SHARED-*
      if (node_info->father_info == NULL ||
          VSG_PRTREE3@T@_NODE_INFO_IS_SHARED (node_info->father_info))
        {
          guint8 i = 0;

          // *** *-LOCAL
          if (PRTREE3@T@NODE_IS_LOCAL (node))
            i = _prtree3@t@node_mindepth (node, node_info, aac->config);

          g_array_append_val (aac->array, i);
        }
    }
}



static void _remote_depths_store (VsgPRTree3@t@Node *node,
                                  VsgPRTree3@t@NodeInfo *node_info,
                                  guint8 ** depths)
{
  // *** == PRIVATE-*
  if (PRTREE3@T@NODE_IS_PRIVATE (node))
    {
      /* detect root of local (remote) subtrees */
      if (node_info->father_info == NULL ||
      // *** father == SHARED-*
          VSG_PRTREE3@T@_NODE_INFO_IS_SHARED (node_info->father_info))
        {
          // *** *-REMOTE
          if (PRTREE3@T@NODE_IS_REMOTE (node))
            PRTREE3@T@NODE_LEAF (node).remote_depth = **depths;

          /* go to the next depth entry */
          (*depths) ++;
        }
    }
}

void vsg_prtree3@t@_update_remote_depths (VsgPRTree3@t@ *tree)
{
  ArrayAndConfig aac;
  GArray *reduced;
  guint8 *depths;

  g_return_if_fail (tree != NULL);

  aac.array = g_array_sized_new (FALSE, FALSE, sizeof (guint8), 1024);
  aac.config = &tree->config;

  vsg_prtree3@t@_traverse_custom_internal (tree, G_PRE_ORDER, NULL, NULL, NULL,
                                           (VsgPRTree3@t@InternalFunc)
                                           _remote_depths_array_build,
                                           &aac);

  /* prepare reduced for storing the results of the Allreduce */
  reduced = g_array_sized_new (FALSE, TRUE, sizeof (guint8), aac.array->len);
  reduced = g_array_set_size (reduced, aac.array->len);

  MPI_Allreduce (aac.array->data, reduced->data, aac.array->len,
                 MPI_UNSIGNED_CHAR, MPI_MAX,
                 tree->config.parallel_config.communicator);

  g_array_free (aac.array, TRUE);

  depths = (guint8 *) reduced->data;

  vsg_prtree3@t@_traverse_custom_internal (tree, G_PRE_ORDER, NULL, NULL, NULL,
                                           (VsgPRTree3@t@InternalFunc)
                                           _remote_depths_store,
                                           &depths);

  g_array_free (reduced, TRUE);

  tree->config.remote_depth_dirty = FALSE;
}

typedef struct _LocalNFLeafCountData LocalNFLeafCountData;
struct _LocalNFLeafCountData {
  VsgPRTree3@t@Config *config;
  GArray *array;
};

static void _local_leaves_count (VsgPRTree3@t@NodeInfo *node_info,
                                 LocalNFLeafCountData *lnflcd)
{
  // *** PRIVATE-*
  if (VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE (node_info))
    {
      /* detect root of local (remote) subtrees */
      // *** SHARED-*
      if (node_info->father_info == NULL ||
          VSG_PRTREE3@T@_NODE_INFO_IS_SHARED (node_info->father_info))
        {
          gint i = 0;
          g_array_append_val (lnflcd->array, i);
        }

      /* increment local leaves */
      // *** *-LOCAL
      if (node_info->isleaf && VSG_PRTREE3@T@_NODE_INFO_IS_LOCAL (node_info) &&
          node_info->point_count != 0)
        g_array_index (lnflcd->array, gint, lnflcd->array->len-1) ++;
    }
}

static void _local_nf_leaves_count (VsgPRTree3@t@NodeInfo *node_info,
                                    LocalNFLeafCountData *lnflcd)
{
  // *** PRIVATE-*
  if (VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE (node_info))
    {
      /* detect root of local (remote) subtrees */
      // *** SHARED-*
      if (node_info->father_info == NULL ||
          VSG_PRTREE3@T@_NODE_INFO_IS_SHARED (node_info->father_info))
        {
          gint i = 0;
          g_array_append_val (lnflcd->array, i);
        }

      /* increment local leaves */
      // *** *-LOCAL
      if ((PRTREE3@T@_NODE_INFO_CALL_NF_ISLEAF(node_info, lnflcd->config) &&
           (node_info->father_info == NULL || ! PRTREE3@T@_NODE_INFO_CALL_NF_ISLEAF(node_info->father_info, lnflcd->config))) &&
          VSG_PRTREE3@T@_NODE_INFO_IS_LOCAL (node_info) &&
          node_info->point_count != 0)
        g_array_index (lnflcd->array, gint, lnflcd->array->len-1) ++;
    }
}

typedef struct _ContiguousDistData ContiguousDistData;
struct _ContiguousDistData {
  VsgPRTree3@t@Config *config;
  GArray *array;       /* array of number of leaves on each local subtree */
  gint total_leaves;   /* total number of leaves in the tree (sum of array) */
  gint q, r, m;        /* distribution variables */
  gint current_index;  /* number of already checked array entries */
  gint current_lcount; /* number of already checked leaves */
  VsgPRTreeKey3@t@ last_subtree_id; /* key of the last local subtree */
  VsgPRTreeKey3@t@ last_virtleaf_id; /* only for virtual leaf dist */
};

static inline gint contiguous_dist_data_dest (ContiguousDistData *cdd)
{
  if (cdd->current_lcount >= cdd->m && cdd->q > 0)
    return (cdd->current_lcount - cdd->r) / cdd->q;

  return cdd->current_lcount / (cdd->q + 1);

}

static gint _contiguous_dist (VsgPRTree3@t@NodeInfo *node_info,
                             ContiguousDistData *cdd)
{
  // *** PRIVATE-LOCAL
  if (VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_LOCAL (node_info))
    {
      gint ret = contiguous_dist_data_dest (cdd);

      if (node_info->point_count == 0)
        return ret;

      cdd->current_lcount ++;

      if (! vsg_prtree_key3@t@_is_ancestor (&cdd->last_subtree_id,
                                            &node_info->id))
        {
          VsgPRTree3@t@NodeInfo *ancestor = node_info;

          // *** PRIVATE-LOCAL
          while (ancestor->father_info &&
                 VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_LOCAL (ancestor->father_info))
            ancestor = ancestor->father_info;

          cdd->current_index ++;
          vsg_prtree_key3@t@_copy (&cdd->last_subtree_id, &ancestor->id);
        }

/*       g_printerr ("%d: sending node ", rk); */
/*       vsg_vector3@t@_write (&node_info->center, stderr); */
/*       g_printerr (" to %d\n", ret); */

      return ret;
    }

  // *** PRIVATE_REMOTE
  g_assert (VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_REMOTE (node_info));

  cdd->current_lcount += g_array_index (cdd->array, gint, cdd->current_index);
  cdd->current_index ++;

  return -1;
}

static inline VsgPRTreeKey3@t@ *
_search_virtual_leaf (VsgPRTree3@t@NodeInfo *node_info,
                      VsgPRTree3@t@Config *config)
{
  VsgPRTree3@t@NodeInfo *father_info = node_info->father_info;

  /* go back in ancestry while father is a virtual leaf */
  while (father_info != NULL &&
         PRTREE3@T@_NODE_INFO_CALL_NF_ISLEAF (father_info, config))
    {
      node_info = father_info;
      father_info = node_info->father_info;
    }

  /* return higher virtual leaf in the ancestry */
  return &node_info->id;
}

static const VsgPRTreeKey3@t@ _dummy_key = {0, 0, 0, 255};

static gint _contiguous_dist_virtual_leaf (VsgPRTree3@t@NodeInfo *node_info,
                                           ContiguousDistData *cdd)
{
  // *** PRIVATE-LOCAL
  if (VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_LOCAL (node_info))
    {
      gint ret;

      if (node_info->point_count == 0)
        return contiguous_dist_data_dest (cdd);

      if (vsg_prtree_key3@t@_equals (&cdd->last_virtleaf_id, &_dummy_key))
        {
          /* no leaf was already registered, find the higher virtual leaf */
          VsgPRTreeKey3@t@ *k = _search_virtual_leaf (node_info, cdd->config);
          vsg_prtree_key3@t@_copy (&cdd->last_virtleaf_id, k);

          return contiguous_dist_data_dest (cdd);
        }

      /* we are inside an already registered virtual leaf */
      if (vsg_prtree_key3@t@_is_ancestor (&cdd->last_virtleaf_id,
                                          &node_info->id))
        return contiguous_dist_data_dest (cdd);

      /* changing to a new virtual leaf: recompute new destination */
      cdd->current_lcount ++;
      ret = contiguous_dist_data_dest (cdd);
      /* register new virtual leaf */
      vsg_prtree_key3@t@_copy (&cdd->last_virtleaf_id,
                               _search_virtual_leaf (node_info, cdd->config));

      if (! vsg_prtree_key3@t@_is_ancestor (&cdd->last_subtree_id,
                                            &node_info->id))
        {
          VsgPRTree3@t@NodeInfo *ancestor = node_info;

          // *** PRIVATE-LOCAL
          while (ancestor->father_info &&
                 VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_LOCAL (ancestor->father_info))
            ancestor = ancestor->father_info;

          cdd->current_index ++;
          vsg_prtree_key3@t@_copy (&cdd->last_subtree_id, &ancestor->id);
        }

/*       g_printerr ("%d: sending node ", rk); */
/*       vsg_vector3@t@_write (&node_info->center, stderr); */
/*       g_printerr (" to %d\n", ret); */

      return ret;
    }

  // *** PRIVATE_REMOTE
  g_assert (VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_REMOTE (node_info));

  cdd->current_lcount += g_array_index (cdd->array, gint, cdd->current_index);
  cdd->current_index ++;

  return -1;
}

typedef gint (*ContiguousDistFunc) (VsgPRTree3@t@NodeInfo *node_info,
                                    ContiguousDistData *cdd);

static void
_distribute_contiguous_leaves_internal (VsgPRTree3@t@ *tree,
                                        VsgPRTree3@t@Func leaves_count,
                                        ContiguousDistFunc contiguous_dist)
{
  MPI_Comm comm;
  ContiguousDistData cdd;
  LocalNFLeafCountData lnflcd;
  GArray *reduced;
  gint i, sz;

  g_return_if_fail (tree != NULL);

  lnflcd.config = &tree->config;
  lnflcd.array = g_array_sized_new (FALSE, FALSE, sizeof (gint), 1024);
  comm = tree->config.parallel_config.communicator;
  
/*   MPI_Comm_rank (comm, &rk); */
  MPI_Comm_size (comm, &sz);

  /* accumulate number of local leaves in the local subtrees' roots */
  vsg_prtree3@t@_traverse (tree, G_PRE_ORDER,
                           leaves_count,
                           &lnflcd);

  /* prepare reduced for storing the results of the Allreduce */
  reduced = g_array_sized_new (FALSE, TRUE, sizeof (gint), lnflcd.array->len);
  reduced = g_array_set_size (reduced, lnflcd.array->len);

  MPI_Allreduce (lnflcd.array->data, reduced->data, lnflcd.array->len, MPI_INT,
                 MPI_MAX, comm);

/*   g_printerr ("%d: contiguous allreduce size=%d\n", rk, array->len); */

  g_array_free (lnflcd.array, TRUE);

  cdd.config = &tree->config;
  cdd.array = reduced;
  cdd.total_leaves = 0;
  for (i=0; i<reduced->len; i++)
    cdd.total_leaves += g_array_index (reduced, gint, i);

  if (cdd.total_leaves > 0)
    {
      cdd.q = cdd.total_leaves / sz;
      cdd.r = cdd.total_leaves % sz;
      cdd.m = (cdd.q+1) * cdd.r;
      cdd.current_lcount = 0;
      cdd.current_index = 0;
      cdd.last_subtree_id = _dummy_key;
      cdd.last_virtleaf_id = _dummy_key;

      vsg_prtree3@t@_distribute_nodes (tree,
                                       (VsgPRTree3@t@DistributionFunc) contiguous_dist,
                                       &cdd);

/*   g_printerr ("%d : reduced-len=%d current-index=%d\n", rk, reduced->len, cdd.current_index); */
    }

  g_array_free (reduced, TRUE);
}

/**
 * vsg_prtree3@t@_distribute_contiguous_leaves:
 * @tree: a #VsgPRTree3@t@.
 *
 * Performs parallel distribution of the nodes in @tree so as leaves
 * are sent to processors in order to have contiguous segments of (non
 * empty) leaves of roughly the same size in each
 * processor. "Contiguous segments" is here meant as if leaves were
 * numbered successively in an ordinary traversal of @tree. This means
 * that this distribution is subject to the current children ordering
 * of the tree.
 *
 * NOTE: this is a collective communication function.
 *
 * WARNING: only available when VSG_HAVE_MPI is defined.
 */
void vsg_prtree3@t@_distribute_contiguous_leaves (VsgPRTree3@t@ *tree)
{
  VsgPRTree3@t@Func leaves_count = (VsgPRTree3@t@Func) _local_leaves_count;
  ContiguousDistFunc contiguous_dist = _contiguous_dist;

  if (! vsg_prtree3@t@_nf_isleaf_is_default (tree))
    {
      leaves_count = (VsgPRTree3@t@Func) _local_nf_leaves_count;
      contiguous_dist = _contiguous_dist_virtual_leaf;
    }

  _distribute_contiguous_leaves_internal (tree, leaves_count, contiguous_dist);


}

typedef struct _ScatterData ScatterData;
struct _ScatterData {
  gint cptr;
  gint sz;
};

static gint scatter_dist (VsgPRTree3@t@NodeInfo *node_info, ScatterData *sd)
{
  // *** PRIVATE-LOCAL
  if (VSG_PRTREE3@T@_NODE_INFO_IS_PRIVATE_LOCAL (node_info))
    {
      gint dst = sd->cptr;

      sd->cptr ++;
      sd->cptr = sd->cptr % sd->sz;

      return dst;
    }

  return -1;
}

/**
 * vsg_prtree3@t@_distribute_scatter_leaves:
 * @tree: a #VsgPRTree3@t@.
 *
 * Performs parallel distribution of the nodes in @tree so as all
 * leaves will be scattered across all processors, in the order they
 * are encountered in a regular traversal (ie. first leaf for
 * processor 0, second leaf for processor 1, etc... and cycling when
 * the number of processors is reached).
 *
 * NOTE: this is a collective communication function.
 *
 * WARNING: only available when VSG_HAVE_MPI is defined.
 */
void vsg_prtree3@t@_distribute_scatter_leaves (VsgPRTree3@t@ *tree)
{
  MPI_Comm comm;
  ScatterData sd;

  g_return_if_fail (tree != NULL);

  comm = tree->config.parallel_config.communicator;

  sd.cptr = 0;
  MPI_Comm_size (comm, &sd.sz);

  vsg_prtree3@t@_distribute_nodes (tree,
                                   (VsgPRTree3@t@DistributionFunc)
                                   scatter_dist,
                                   &sd);
}

static gint concentrate_dist (VsgPRTree3@t@NodeInfo *node_info, gint *dst)
{
  return *dst;
}

/**
 * vsg_prtree3@t@_distribute_concentrate:
 * @tree: a #VsgPRTree3@t@.
 * @dst: destination processor number in the @tree's communicator.
 *
 * Performs parallel distribution of the nodes in @tree so as all
 * nodes will be sent to the processor @dst.
 *
 * NOTE: this is a collective communication function.
 *
 * WARNING: only available when VSG_HAVE_MPI is defined.
 */
void vsg_prtree3@t@_distribute_concentrate (VsgPRTree3@t@ *tree, gint dst)
{
  MPI_Comm comm;
  gint sz;

  g_return_if_fail (tree != NULL);

  comm = tree->config.parallel_config.communicator;
  MPI_Comm_size (comm, &sz);

  vsg_prtree3@t@_distribute_nodes (tree,
                                   (VsgPRTree3@t@DistributionFunc)
                                   concentrate_dist,
                                   &dst);

}
