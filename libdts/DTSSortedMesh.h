#ifndef DTS_SORTEDMESH_H_
#define DTS_SORTEDMESH_H_

#include "DTSMesh.h"

namespace DTS
{

// TSSortedMesh is for meshes that need sorting (obviously).  Such meshes
// are usually partially or completely composed of translucent/parent polygons.
class TSSortedMesh : public TSMesh
{
public:
	// This is a group of primitives that belong "together" in the rendering sequence.
	// For example, if a player model had a helmet with a translucent visor, the visor
	// would be a Cluster.
	struct Cluster
	{
		int32_t start_primitive;
		int32_t end_primitive;
		Point3F normal;
		float k;
		int32_t front_cluster;	// go to this cluster if in front of plane, if front_cluster<0, no cluster
		int32_t back_cluster;	// go to this cluster if in back of plane, if back_cluster<0, no cluster
								// if front_cluster==back_cluster, no plane to test against...
	};

	TSSortedMesh()
	{
		mesh_type_ = kSortedMeshType;
	}

	// persist methods...
	void Assemble(bool skip);
	void Disassemble();

	std::vector<Cluster> clusters_;			// All of the clusters of primitives to be drawn
	std::vector<int32_t> start_cluster_;	// indexed by frame number
	std::vector<int32_t> first_verts_;		// indexed by frame number
	std::vector<int32_t> num_verts_;		// indexed by frame number
	std::vector<int32_t> first_tverts_;		// indexed by frame number or mat_frame number, depending on which one animates (never both)

	bool always_write_depth_;				// sometimes, we want to write the depth value to the frame buffer even when object is translucent
};

} // namespace DTS

#endif // DTS_SORTEDMESH_H_
