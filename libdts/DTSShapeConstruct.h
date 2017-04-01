#ifndef DTS_SHAPECONSTRUCT_H_
#define DTS_SHAPECONSTRUCT_H_

#include "DTSShape.h"

namespace DTS
{

class TSShapeConstructor
{
public:
	enum CollisionDetailType
	{
		kBox,
		kSphere,
		kCapsule,
		k10_DOP_X,
		k10_DOP_Y,
		k10_DOP_Z,
		k18_DOP,
		k26_DOP
	};

	TSShapeConstructor(TSShape* shape);

	static const std::string& GetCapsuleShapePath() { return capsule_shape_path_; }
	static const std::string& GetCubeShapePath() { return cube_shape_path_; }
	static const std::string& GetSphereShapePath() { return sphere_shape_path_; }

	// Nodes
	bool AddNode(const std::string& name, const std::string& parent_name, Point3F pos = Point3F::kZero, QuatF rot = QuatF::kIdentity, bool is_world = false);
	bool SetNodeTransform(const std::string& name, Point3F pos, QuatF rot, bool is_world = false);

	// Detail Levels
	bool AddCollisionDetail(int32_t size, CollisionDetailType type, std::string target);

	TSShape* shape_; // Edited shape; NULL while not loaded;

private:
	bool GetNodeIndexNoRoot(TSShape::Node*& node, const std::string& name);

	// Paths to shapes used by MeshFit
	static std::string capsule_shape_path_;
	static std::string cube_shape_path_;
	static std::string sphere_shape_path_;
};

} // namespace DTS

#endif // DTS_SHAPECONSTRUCT_H_
