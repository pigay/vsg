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

#ifndef __VSG_PRTREE3_COMMON_H__
#define __VSG_PRTREE3_COMMON_H__

G_BEGIN_DECLS;

/* point objects localization */

/**
 * VSG_LOC3_BSW:
 *
 * VsgPRTree3? node point (#VsgPoint3) localization constant:
 * Bottom South West. #vsgloc3 with binary value: %000
 */
#define VSG_LOC3_BSW ( (vsgloc3) 0x00 )

/**
 * VSG_LOC3_BSE:
 *
 * VsgPRTree3? node point (#VsgPoint3) localization constant:
 * Bottom South East. #vsgloc3 with binary value: %001
 */

#define VSG_LOC3_BSE ( (vsgloc3) 0x01 )

/**
 * VSG_LOC3_BNW:
 *
 * VsgPRTree3? node point (#VsgPoint3) localization constant:
 * Bottom North West. #vsgloc3 with binary value: %010
 */
#define VSG_LOC3_BNW ( (vsgloc3) 0x02 )

/**
 * VSG_LOC3_BNE:
 *
 * VsgPRTree3? node point (#VsgPoint3) localization constant:
 * Bottom North East. #vsgloc3 with binary value: %011
 */
#define VSG_LOC3_BNE ( (vsgloc3) 0x03 )

/**
 * VSG_LOC3_USW:
 *
 * VsgPRTree3? node point (#VsgPoint3) localization constant:
 * Up South West. #vsgloc3 with binary value: %100
 */
#define VSG_LOC3_USW ( (vsgloc3) 0x04 )

/**
 * VSG_LOC3_USE:
 *
 * VsgPRTree3? node point (#VsgPoint3) localization constant:
 * Up South East. #vsgloc3 with binary value: %101
 */

#define VSG_LOC3_USE ( (vsgloc3) 0x05 )

/**
 * VSG_LOC3_UNW:
 *
 * VsgPRTree3? node point (#VsgPoint3) localization constant:
 * Up North West. #vsgloc3 with binary value: %110
 */
#define VSG_LOC3_UNW ( (vsgloc3) 0x06 )

/**
 * VSG_LOC3_UNE:
 *
 * VsgPRTree3? node point (#VsgPoint3) localization constant:
 * Up North East. #vsgloc3 with binary value: %111
 */
#define VSG_LOC3_UNE ( (vsgloc3) 0x07 )

/**
 * VSG_LOC3_MASK:
 *
 * VsgPRTree3? node point (#VsgPoint3) localization mask.
 */
#define VSG_LOC3_MASK ( (vsgloc3) 0x07 )

/**
 * VSG_LOC3_AXIS:
 * @n: 0 == x axis, 1 == y axis, 2 == z axis
 * 
 * Computes the bit mask corresponding to its argument axis.
 */
#define VSG_LOC3_AXIS(n) ( (vsgloc3) 1 << (n) )

/**
 * VSG_LOC3_X:
 *
 * X axis in an #vsgloc3.
 */
#define VSG_LOC3_X VSG_LOC3_AXIS (0)

/**
 * VSG_LOC3_Y:
 *
 * Y axis in an #vsgloc3.
 */
#define VSG_LOC3_Y VSG_LOC3_AXIS (1)

/**
 * VSG_LOC3_Z:
 *
 * Z axis in an #vsgloc3.
 */
#define VSG_LOC3_Z VSG_LOC3_AXIS (2)


/* region objects localization */

/**
 * VSG_RLOC3_COMP:
 * @i: a #vsgloc3
 *
 * Computes the #vsgrloc3 associated with @i.
 */
#define VSG_RLOC3_COMP(i) ((vsgrloc3) 1 << (i))

/**
 * VSG_RLOC3_MASK:
 *
 * VsgPRTree3? node region (#VsgRegion3) localization mask.
 */
#define VSG_RLOC3_MASK ((vsgrloc3) 0xFF)

/**
 * VsgPoint3:
 *
 * Pointer type of vertices stored by VsgPRTree3?. They serve as localization
 * for the tree (see #vsgloc3 and VsgPoint3?LocFunc).
 */

typedef gpointer VsgPoint3;

/**
 * vsgloc3:
 *
 * 3D localization bit mask for #VsgPoint3.
 * @see: #VSG_LOC3_SW, #VSG_LOC3_SE, #VSG_LOC3_NW, #VSG_LOC3_NE
 */
typedef guint8 vsgloc3;

/**
 * VsgRegion3:
 *
 * Pointer type of objects stored by VsgPRTree3? that have a dimension. think
 * of sphere or triangle as opposed to #VsgPoint3 vertices.
 */
typedef gpointer VsgRegion3;

/**
 * vsgrloc3:
 *
 * 3D localization bit mask for #VsgRegion3.
 */
typedef guint8 vsgrloc3;

typedef gboolean (*VsgRegion3CheckFunc) (const VsgRegion3 region,
                                         gpointer user_data);

/**
 * VsgRegion3Point3LocFunc:
 * @region:
 * @point:
 *
 * Function type related to VsgRegion3*LocFunc but that takes a #VsgPoint3
 * instead of a #VsgVector3* for second argument.
 *
 * Returns: localization bitmask.
 */
typedef vsgrloc3 (*VsgRegion3Point3LocFunc) (const VsgRegion3 region,
                                             const VsgPoint3 point);


G_END_DECLS;

#endif /* __VSG_PRTREE3_COMMON_H__ */
