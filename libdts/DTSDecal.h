#ifndef DTS_DECAL_H_
#define DTS_DECAL_H_

#include "DTSMesh.h"

namespace DTS
{

// Decals!  The lovely detailing thingies, e.g. bullet hole marks.
// DEPRECATED: This class is here for compatibility with old files only.
// Performs no actual rendering.
class TSDecalMesh
{
public:
	void Assemble(bool skip);
	void Disassemble();

	// Topology
	std::vector<TSDrawPrimitive> primitives_;
	std::vector<uint16_t> indices_;

	// Render Data
	// indexed by decal frame...
	std::vector<int32_t> start_primitive_;
	std::vector<Point4F> texgen_S_;
	std::vector<Point4F> texgen_T_;

	// We only allow 1 material per decal...
	int32_t material_index_;
};

} // namespace DTS

#endif // DTS_DECAL_H_
