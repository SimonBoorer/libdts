#include "DTSShape.h"

#include "DTSString.h"

namespace DTS
{

int32_t TSShape::AddName(const std::string& name)
{
	// Check for empty names
	if (name.empty())
		return -1;

	// Return the index of the new name (add if it is unique)
	int32_t index = FindName(name);
	if (index >= 0)
		return index;

	names_.push_back(name);
	return names_.size() - 1;
}

void TSShape::UpdateSmallestVisibleDL()
{
	// Update smallest visible detail
	smallest_visible_dl_ = -1;
	smallest_visible_size_ = FLT_MAX;
	float max_size = 0.0f;
	for (int32_t i = 0; i < details_.size(); i++)
	{
		max_size = std::max(max_size, details_[i].size);

		if ((details_[i].size >= 0) && (details_[i].size < smallest_visible_size_))
		{
			smallest_visible_dl_ = i;
			smallest_visible_size_ = details_[i].size;
		}
	}
}

int32_t TSShape::AddDetail(const std::string& dname, int32_t size, int32_t sub_shape_num)
{
	int32_t name_index = AddName(dname + std::to_string(size));

	// Check if this detail size has already been added
	int32_t index;
	for (index = 0; index < details_.size(); index++)
	{
		if ((details_[index].size == size) &&
			(details_[index].sub_shape_num == sub_shape_num) &&
			(details_[index].name_index == name_index))
			return index;
		if (details_[index].size < size)
			break;
	}

	// Create a new detail level at the right index, so array
	// remains sorted by detail size (from largest to smallest)
	Vector::Insert(details_, index);
	TSShape::Detail &detail = details_[index];

	// Clear the detail to ensure no garbage values
	// are left in any vars we don't set.
	memset(&detail, 0, sizeof(Detail));

	// Setup the detail.
	detail.name_index = name_index;
	detail.size = size;
	detail.sub_shape_num = sub_shape_num;
	detail.object_detail_num = 0;
	detail.average_error = -1;
	detail.max_error = -1;
	detail.poly_count = 0;

	// Fixup objectDetailNum in other detail levels
	for (int32_t i = index + 1; i < details_.size(); i++)
	{
		if ((details_[i].sub_shape_num >= 0) &&
			((sub_shape_num == -1) || (details_[i].sub_shape_num == sub_shape_num)))
			details_[i].object_detail_num++;
	}

	// Update smallest visible detail
	UpdateSmallestVisibleDL();

	return index;
}

bool TSShape::SetNodeTransform(const std::string& name, const Point3F& pos, const QuatF& rot)
{
	// Find the node to be transformed
	int32_t node_index = FindNode(name);
	if (node_index < 0)
	{
		return false;
	}

	// Update initial node position and rotation
	default_translations_[node_index] = pos;
	default_rotations_[node_index].Set(rot);

	return true;
}

bool TSShape::AddNode(const std::string& name, const std::string& parent_name, const Point3F& pos, const QuatF& rot)
{
	// Check that adding this node would not exceed the maximum count
	if (nodes_.size() >= TSIntegerSet::kMaxSetSize)
	{
		return false;
	}

	// Check that there is not already a node with this name
	if (FindNode(name) >= 0)
	{
		return false;
	}

	// Find the parent node (OK for name to be empty => node is at root level)
	int32_t parent_index = -1;
	if (strcmp(parent_name.c_str(), ""))
	{
		parent_index = FindNode(parent_name);
		if (parent_index < 0)
		{
			return false;
		}
	}

	// Insert node at the end of the subshape
	int32_t sub_shape_index = (parent_index >= 0) ? GetSubShapeForNode(parent_index) : 0;
	int32_t node_index = sub_shape_num_nodes_[sub_shape_index];

	// Adjust subshape node indices
	sub_shape_num_nodes_[sub_shape_index]++;
	for (int32_t i = sub_shape_index + 1; i < sub_shape_first_node_.size(); i++)
		sub_shape_first_node_[i]++;

	// Update animation sequences
	for (int32_t i_seq = 0; i_seq < sequences_.size(); i_seq++)
	{
		// Update animation matters arrays (new node is not animated)
		TSShape::Sequence& seq = sequences_[i_seq];
		seq.translation_matters_.Insert(node_index, false);
		seq.rotation_matters_.Insert(node_index, false);
		seq.scale_matters_.Insert(node_index, false);
	}

	// Insert the new node
	TSShape::Node node;
	node.name_index = AddName(name);
	node.parent_index = parent_index;
	node.first_child = -1;
	node.first_object = -1;
	node.next_sibling = -1;
	Vector::Insert(nodes_, node_index, node);

	// Insert node default translation and rotation
	Quat16 rot16;
	rot16.Set(rot);
	Vector::Insert(default_translations_, node_index, pos);
	Vector::Insert(default_rotations_, node_index, rot16);

	// Fixup node indices
	for (int32_t i = 0; i < nodes_.size(); i++)
	{
		if (nodes_[i].parent_index >= node_index)
			nodes_[i].parent_index++;
	}
	for (int32_t i = 0; i < objects_.size(); i++)
	{
		if (objects_[i].node_index >= node_index)
			objects_[i].node_index++;
	}
	for (int32_t i = 0; i < meshes_.size(); i++)
	{
		if (meshes_[i] && (meshes_[i]->GetMeshType() == TSMesh::kSkinMeshType))
		{
			TSSkinMesh* skin = dynamic_cast<TSSkinMesh*>(meshes_[i]);
			for (int32_t j = 0; j < skin->node_index_.size(); j++)
			{
				if (skin->node_index_[j] >= node_index)
					skin->node_index_[j]++;
			}
		}
	}

	return true;
}

int32_t TSShape::AddObject(const std::string& objName, int32_t sub_shape_index)
{
	int32_t obj_index = sub_shape_num_objects_[sub_shape_index];

	// Add object to subshape
	sub_shape_num_objects_[sub_shape_index]++;
	for (int32_t i = sub_shape_index + 1; i < sub_shape_first_object_.size(); i++)
		sub_shape_first_object_[i]++;

	TSShape::Object obj;
	obj.name_index = AddName(objName);
	obj.node_index = 0;
	obj.num_meshes = 0;
	obj.start_mesh_index = (obj_index == 0) ? 0 : objects_[obj_index - 1].start_mesh_index + objects_[obj_index - 1].num_meshes;
	obj.first_decal = 0;
	obj.next_sibling = 0;
	Vector::Insert(objects_, obj_index, obj);

	// Add default object state
	TSShape::ObjectState state;
	state.frame_index = 0;
	state.mat_frame_index = 0;
	state.vis = 1.0f;
	Vector::Insert(object_states_, obj_index, state);

	// Fixup sequences
	for (int32_t i = 0; i < sequences_.size(); i++)
		sequences_[i].base_object_state_++;

	return obj_index;
}

void TSShape::AddMeshToObject(int32_t obj_index, int32_t mesh_index, TSMesh* mesh)
{
	TSShape::Object& obj = objects_[obj_index];

	// Pad with nullptrs if required
	int32_t old_num_meshes = obj.num_meshes;
	if (mesh)
	{
		for (int32_t i = obj.num_meshes; i < mesh_index; i++)
		{
			Vector::Insert(meshes_, obj.start_mesh_index + i, static_cast<TSMesh*>(nullptr));
			obj.num_meshes++;
		}
	}

	// Insert the new mesh
	Vector::Insert(meshes_, obj.start_mesh_index + mesh_index, mesh);
	obj.num_meshes++;

	// Skinned meshes are not attached to any node
	if (mesh && (mesh->GetMeshType() == TSMesh::kSkinMeshType))
		obj.node_index = -1;

	// Fixup mesh indices for other objects
	for (int32_t i = 0; i < objects_.size(); i++)
	{
		if ((i != obj_index) && (objects_[i].start_mesh_index >= obj.start_mesh_index))
			objects_[i].start_mesh_index += (obj.num_meshes - old_num_meshes);
	}
}


bool TSShape::SetObjectNode(const std::string& obj_name, const std::string& node_name)
{
	// Find the object and node
	int32_t obj_index = FindObject(obj_name);
	if (obj_index < 0)
	{
		return false;
	}

	int32_t node_index;
	if (node_name.empty())
		node_index = -1;
	else
	{
		node_index = FindNode(node_name);
		if (node_index < 0)
		{
			return false;
		}
	}

	objects_[obj_index].node_index = node_index;

	return true;
}

TSMesh* TSShape::CopyMesh(const TSMesh* src_mesh) const
{
	TSMesh * mesh = nullptr;
	if (src_mesh && (src_mesh->GetMeshType() == TSMesh::kSkinMeshType))
	{
		TSSkinMesh* skin = new TSSkinMesh;

		// Copy skin elements
		const TSSkinMesh *src_skin = dynamic_cast<const TSSkinMesh*>(src_mesh);
		skin->weight_ = src_skin->weight_;
		skin->vertex_index_ = src_skin->vertex_index_;
		skin->bone_index_ = src_skin->bone_index_;

		skin->node_index_ = src_skin->node_index_;
		skin->initial_transforms_ = src_skin->initial_transforms_;
		skin->initial_verts_ = src_skin->initial_verts_;
		skin->initial_norms_ = src_skin->initial_norms_;

		mesh = static_cast<TSMesh*>(skin);
	}
	else
	{
		mesh = new TSMesh;
	}

	if (!src_mesh)
		return mesh; // return an empty mesh

	// Copy mesh elements
	mesh->indices_ = src_mesh->indices_;
	mesh->primitives_ = src_mesh->primitives_;
	mesh->num_frames_ = src_mesh->num_frames_;
	mesh->num_mat_frames_ = src_mesh->num_mat_frames_;
	mesh->verts_per_frame_ = src_mesh->verts_per_frame_;
	mesh->SetFlags(src_mesh->GetFlags());

	// Copy vertex data in an *unpacked* form
	mesh->CopySourceVertexDataFrom(src_mesh);

	Vector::Set(mesh->encoded_norms_, NULL, 0);

	mesh->ComputeBounds();

	return mesh;
}

bool TSShape::AddMesh(TSMesh* mesh, const std::string& mesh_name)
{
	// Determine the object name and detail size from the mesh name
	int32_t detail_size = 999;
	std::string obj_name(String::GetTrailingNumber(mesh_name.c_str(), detail_size));

	// Find the destinatio+n object (create one if it does not exist)
	int32_t obj_index = FindObject(obj_name);
	if (obj_index < 0)
		obj_index = AddObject(obj_name, 0);
	assert(obj_index >= 0 && obj_index < objects_.size());

	// Determine the subshape this object belongs to
	int32_t sub_shape_index = GetSubShapeForObject(obj_index);
	assert(sub_shape_index < sub_shape_first_object_.size());

	// Get the existing detail levels for the subshape
	std::vector<int32_t> valid_details;
	GetSubShapeDetails(sub_shape_index, valid_details);

	// Determine where to add the new mesh, and whether this is a new detail
	int32_t det_index;
	bool new_detail = true;
	for (det_index = 0; det_index < valid_details.size(); det_index++)
	{
		const TSShape::Detail& det = details_[valid_details[det_index]];
		if (detail_size >= det.size)
		{
			new_detail = (det.size != detail_size);
			break;
		}
	}

	// Insert the new detail level if required
	if (new_detail)
	{
		// Determine a name for the detail level
		const char* detail_name;
		if (String::StartsWith(obj_name, "Col"))
			detail_name = "collision";
		else if (String::StartsWith(obj_name, "loscol"))
			detail_name = "los";
		else
			detail_name = "detail";

		int32_t index = AddDetail(detail_name, detail_size, sub_shape_index);
		details_[index].object_detail_num = det_index;
	}

	// Adding a new mesh or detail level is a bit tricky, since each
	// object potentially stores a different number of meshes, including
	// nullptr meshes for higher detail levels where required.
	// For example, the following table shows 3 objects. Note how NULLs
	// must be inserted for detail levels higher than the first valid
	// mesh, but details after the the last valid mesh are left empty.
	//
	// Detail   |  Object1  |  Object2  |  Object3
	// ---------+-----------+-----------+---------
	// 128      |  128      |  NULL     |  NULL
	// 64       |           |  NULL     |  64
	// 32       |           |  32       |  NULL
	// 2        |           |           |  2

	// Add meshes as required for each object
	for (int32_t i = 0; i < sub_shape_num_objects_[sub_shape_index]; i++)
	{
		int32_t index = sub_shape_first_object_[sub_shape_index] + i;
		const TSShape::Object& obj = objects_[index];

		if (index == obj_index)
		{
			// The target object: replace the existing mesh (if any) or add a new one
			// if required.
			if (!new_detail && (det_index < obj.num_meshes))
			{
				if (meshes_[obj.start_mesh_index + det_index])
					meshes_[obj.start_mesh_index + det_index]->~TSMesh();
				meshes_[obj.start_mesh_index + det_index] = mesh;
			}
			else
				AddMeshToObject(index, det_index, mesh);
		}
		else
		{
			// Other objects: add a NULL mesh only if inserting before a valid mesh
			if (new_detail && (det_index < obj.num_meshes))
				AddMeshToObject(index, det_index, nullptr);
		}
	}

	return true;
}

} // namespace DTS
