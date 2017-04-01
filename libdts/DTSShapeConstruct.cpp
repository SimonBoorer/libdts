#include "DTSShapeConstruct.h"

#include <string>

#include "DTSMeshFit.h"

namespace DTS
{

std::string TSShapeConstructor::capsule_shape_path_("unit_capsule.dts");
std::string TSShapeConstructor::cube_shape_path_("unit_cube.dts");
std::string TSShapeConstructor::sphere_shape_path_("unit_sphere.dts");

TSShapeConstructor::TSShapeConstructor(TSShape* shape)
	: shape_(shape)
{

}

bool TSShapeConstructor::AddNode(const std::string& name, const std::string& parent_name, Point3F pos, QuatF rot, bool is_world)
{
	if (is_world)
	{
		// World transform

		// Get the node's parent (if any)
		int32_t parent_index = shape_->FindNode(parent_name);
		if (parent_index != -1)
		{
			MatrixF mat;
			shape_->GetNodeWorldTransform(parent_index, &mat);

			// Pre-multiply by inverse of parent's world transform to get
			// local node transform
			mat.Inverse();
			MatrixF mat2(mat);
			rot.SetMatrix(&mat);
			mat.SetPosition(pos);
			mat.Mul(mat2);

			rot.Set(mat);
			pos = mat.GetPosition();
		}
	}

	if (!shape_->AddNode(name, parent_name, pos, rot))
		return false;

	return true;
}

// Do a node lookup, root node ("") is not allowed
bool TSShapeConstructor::GetNodeIndexNoRoot(TSShape::Node*& node, const std::string& name)
{
	int32_t index = shape_->FindNode(name);
	if (index < 0)
	{
		return false;
	}

	node = &(shape_->nodes_[index]);
	return true;
}

bool TSShapeConstructor::SetNodeTransform(const std::string& name, Point3F pos, QuatF rot, bool is_world)
{
	TSShape::Node *node;
	if (!GetNodeIndexNoRoot(node, name))
		return false;

	if (is_world)
	{
		// World transform

		// Get the node's parent (if any)
		if (node->parent_index != -1)
		{
			MatrixF mat;
			shape_->GetNodeWorldTransform(node->parent_index, &mat);

			// Pre-multiply by inverse of parent's world transform to get
			// local node transform
			mat.Inverse();
			MatrixF mat2(mat);
			rot.SetMatrix(&mat);
			mat.SetPosition(pos);
			mat.Mul(mat2);

			rot.Set(mat);
			pos = mat.GetPosition();
		}
	}

	if (!shape_->SetNodeTransform(name, pos, rot))
		return false;

	return true;
}

bool TSShapeConstructor::AddCollisionDetail(int32_t size, CollisionDetailType type, std::string target)
{
	MeshFit fit(shape_);
	fit.InitSourceGeometry("bounds");
	if (!fit.IsReady())
	{
		return false;
	}

	switch (type)
	{
	case kBox:		fit.FitOBB();		break;	
	case kSphere:	fit.FitSphere();	break;
	case kCapsule:	fit.FitCapsule();	break;
	case k10_DOP_X:	fit.Fit10_DOP_X();	break;
	case k10_DOP_Y:	fit.Fit10_DOP_Y();	break;
	case k10_DOP_Z:	fit.Fit10_DOP_Z();	break;
	case k18_DOP:	fit.Fit18_DOP();	break;
	case k26_DOP:	fit.Fit26_DOP();	break;
	default:		return false;
	}

	// Now add the fitted meshes to the shape:
	// - primitives (box, sphere, capsule) need their own node (with appropriate
	//   transform set) so that we can use the mesh bounds to compute the real
	//   collision primitive at load time without having to examine the geometry.
	// - convex meshes may be added at the default node, with identity transform
	// - since all meshes are in the same detail level, they all get a unique
	//   object name

	const std::string col_node_name("Col" + std::to_string(size));

	// Add the default node with identity transform
	int32_t node_index = shape_->FindNode(col_node_name);
	if (node_index == -1)
	{
		AddNode(col_node_name, "");
	}
	else
	{
		MatrixF mat;
		shape_->GetNodeWorldTransform(node_index, &mat);
		if (!mat.IsIdentity())
			SetNodeTransform(col_node_name, Point3F::kZero, QuatF::kIdentity);
	}

	// Add the meshes to the shape
	for (int32_t i = 0; i < fit.GetMeshCount(); i++)
	{
		MeshFit::Mesh* mesh = fit.GetMesh(i);

		// Determine a unique name for this mesh
		std::string obj_name;
		switch (mesh->type)
		{
		case MeshFit::kBox:		obj_name = "ColBox";		break;
		case MeshFit::kSphere:	obj_name = "ColSphere";		break;
		case MeshFit::kCapsule:	obj_name = "ColCapsule";	break;
		default:				obj_name = "ColConvex";		break;
		}

		for (int32_t suffix = i; suffix != 0; suffix /= 26)
			obj_name += ('A' + (suffix % 26));
		std::string mesh_name = obj_name + std::to_string(size);

		shape_->AddMesh(mesh->tsmesh, mesh_name);

		// Add a node for this object if needed (non-identity transform)
		if (mesh->transform.IsIdentity())
		{
			shape_->SetObjectNode(obj_name, col_node_name);
		}
		else
		{
			Point3F pos(mesh->transform.GetPosition());
			QuatF rot(mesh->transform);

			AddNode(mesh_name, col_node_name, pos, rot);
			shape_->SetObjectNode(obj_name, mesh_name);
		}
	}

	return true;
}

} // namespace DTS
