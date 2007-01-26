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

#ifndef __VSG_PRTREE2_COMMON_H__
#define __VSG_PRTREE2_COMMON_H__

G_BEGIN_DECLS;

/* point objects localization */

/**
 * VSG_LOC2_SW:
 *
 * VsgPRTree2? node point (#VsgPoint2) localization constant:
 * South West. #vsgloc2 with binary value: %00
 */
#define VSG_LOC2_SW ( (vsgloc2) 0x00 )

/**
 * VSG_LOC2_SE:
 *
 * VsgPRTree2? node point (#VsgPoint2) localization constant:
 * South East. #vsgloc2 with binary value: %01
 */

#define VSG_LOC2_SE ( (vsgloc2) 0x01 )

/**
 * VSG_LOC2_NW:
 *
 * VsgPRTree2? node point (#VsgPoint2) localization constant:
 * North West. #vsgloc2 with binary value: %10
 */
#define VSG_LOC2_NW ( (vsgloc2) 0x02 )

/**
 * VSG_LOC2_NE:
 *
 * VsgPRTree2? node point (#VsgPoint2) localization constant:
 * North East. #vsgloc2 with binary value: %11
 */
#define VSG_LOC2_NE ( (vsgloc2) 0x03 )

/**
 * VSG_LOC2_MASK:
 *
 * VsgPRTree2? node point (#VsgPoint2) localization mask.
 */
#define VSG_LOC2_MASK ( (vsgloc2) 0x03 )

/**
 * VSG_LOC2_AXIS:
 * @n: 0 == x axis, 1 == y axis
 * 
 * Computes the bit mask corresponding to its argument axis.
 */
#define VSG_LOC2_AXIS(n) ( (vsgloc2) 1 << (n) )

/**
 * VSG_LOC2_X:
 *
 * X axis in an #vsgloc2.
 */
#define VSG_LOC2_X VSG_LOC2_AXIS (0)

/**
 * VSG_LOC2_Y:
 *
 * Y axis in an #vsgloc2.
 */
#define VSG_LOC2_Y VSG_LOC2_AXIS (1)


/* region objects localization */

/**
 * VSG_RLOC2_COMP:
 * @i: a #vsgloc2
 *
 * Computes the #vsgrloc2 associated with @i.
 */
#define VSG_RLOC2_COMP(i) ((vsgrloc2) 1 << (i))

/**
 * VSG_RLOC2_MASK:
 *
 * VsgPRTree2? node region (#VsgRegion2) localization mask.
 */
#define VSG_RLOC2_MASK ((vsgrloc2) 0x0F)

/**
 * VsgPoint2:
 *
 * Pointer type of vertices stored by VsgPRTree2?. They serve as localization
 * for the tree (see #vsgloc2 and VsgPoint2?LocFunc).
 */
typedef gpointer VsgPoint2;

/**
 * vsgloc2:
 *
 * 2D localization bit mask for #VsgPoint2.
 * @see: #VSG_LOC2_SW, #VSG_LOC2_SE, #VSG_LOC2_NW, #VSG_LOC2_NE
 */
typedef guint8 vsgloc2;

/**
 * VsgRegion2:
 *
 * Pointer type of objects stored by VsgPRTree2? that have a dimension. think
 * of sphere or triangle as opposed to #VsgPoint2 vertices.
 */
typedef gpointer VsgRegion2;

/**
 * vsgrloc2:
 *
 * 2D localization bit mask for #VsgRegion2.
 */
typedef guint8 vsgrloc2;

typedef gboolean (*VsgRegion2CheckFunc) (const VsgRegion2 region,
                                         gpointer user_data);

/**
 * VsgRegion2Point2LocFunc:
 * @region:
 * @point:
 *
 * Function type related to VsgRegion2*LocFunc but that takes a #VsgPoint2
 * instead of a #VsgVector2* for second argument.
 *
 * Returns localization bitmask.
 */
typedef vsgrloc2 (*VsgRegion2Point2LocFunc) (const VsgRegion2 region,
                                             const VsgPoint2 point);


G_END_DECLS;

#endif /* __VSG_PRTREE2_COMMON_H__ */
