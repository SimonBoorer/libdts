#ifndef DTS_BOX_H_
#define DTS_BOX_H_

#include "DTSPoint3.h"

namespace DTS
{

// Axis-aligned bounding box (AABB).
struct Box3F
{
	Point3F min_extents; // Minimum extents of box
	Point3F max_extents; // Maximum extents of box
};

} // namespace DTS

#endif // DTS_BOX_H_
