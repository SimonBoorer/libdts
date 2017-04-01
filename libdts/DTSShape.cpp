#include "DTSShape.h"

#include <fstream>

#include "DTSMaterialList.h"
#include "DTSStream.h"

namespace DTS
{

// most recent version -- this is the version we write
const int32_t TSShape::kVersion = 24;
// Version currently being read, only valid during read
int32_t TSShape::current_read_version_ = -1;
const int32_t TSShape::kMostRecentExporterVersion = 124;

TSShapeAlloc TSShape::alloc_;

#define tsalloc TSShape::alloc_

TSShape::TSShape() :
	material_list_(nullptr), read_version_(-1), shape_data_(nullptr), shape_data_size_(0)
{

}

TSShape::TSShape(const std::string& filename)
	: TSShape()
{
	LoadFromFile(filename);
}

TSShape::~TSShape()
{
	delete material_list_;

	int32_t i;

	// everything left over here is a legit mesh
	for (i = 0; i < meshes_.size(); i++)
	{
		if (!meshes_[i])
			continue;

		// Handle meshes that were either assembled with the shape or added later
		if ((reinterpret_cast<int8_t*>(meshes_[i]) >= shape_data_) && (reinterpret_cast<int8_t*>(meshes_[i]) < (shape_data_ + shape_data_size_)))
			meshes_[i]->~TSMesh();
		else
			delete meshes_[i];
	}

	if (shape_data_)
		delete[] shape_data_;
}

int32_t TSShape::FindName(const std::string& name) const
{
	for (int32_t i = 0; i < names_.size(); i++)
	{
		if (names_[i] == name)
			return i;
	}

	return -1;
}

int32_t TSShape::FindNode(int32_t name_index) const
{
	for (int32_t i = 0; i < nodes_.size(); i++)
		if (nodes_[i].name_index == name_index)
			return i;
	return -1;
}

int32_t TSShape::FindObject(int32_t name_index) const
{
	for (int32_t i = 0; i < objects_.size(); i++)
		if (objects_[i].name_index == name_index)
			return i;
	return -1;
}

int32_t TSShape::GetSubShapeForNode(int32_t node_index)
{
	for (int32_t i = 0; i < sub_shape_first_node_.size(); i++)
	{
		int32_t start = sub_shape_first_node_[i];
		int32_t end = start + sub_shape_num_nodes_[i];
		if ((node_index >= start) && (node_index < end))
			return i;;
	}
	return -1;
}

int32_t TSShape::GetSubShapeForObject(int32_t obj_index)
{
	for (int32_t i = 0; i < sub_shape_first_object_.size(); i++)
	{
		int32_t start = sub_shape_first_object_[i];
		int32_t end = start + sub_shape_num_objects_[i];
		if ((obj_index >= start) && (obj_index < end))
			return i;
	}
	return -1;
}

void TSShape::GetSubShapeDetails(int32_t sub_shape_index, std::vector<int32_t>& valid_details)
{
	valid_details.clear();
	for (int32_t i = 0; i < details_.size(); i++)
	{
		if ((details_[i].sub_shape_num == sub_shape_index) ||
			(details_[i].sub_shape_num < 0))
			valid_details.push_back(i);
	}
}

void TSShape::GetNodeWorldTransform(int32_t node_index, MatrixF* mat) const
{
	if (node_index == -1)
	{
		mat->Identity();
	}
	else
	{
		// Calculate the world transform of the given node
		default_rotations_[node_index].GetQuatF().SetMatrix(mat);
		mat->SetPosition(default_translations_[node_index]);

		int32_t parent_index = nodes_[node_index].parent_index;
		while (parent_index != -1)
		{
			MatrixF mat2(*mat);
			default_rotations_[parent_index].GetQuatF().SetMatrix(mat);
			mat->SetPosition(default_translations_[parent_index]);
			mat->Mul(mat2);

			parent_index = nodes_[parent_index].parent_index;
		}
	}
}

void TSShape::AssembleShape()
{
	int32_t i, j;

	int32_t num_nodes = tsalloc.IMemBuffer32::Get();
	int32_t num_objects = tsalloc.IMemBuffer32::Get();
	int32_t num_decals = tsalloc.IMemBuffer32::Get();
	int32_t num_sub_shapes = tsalloc.IMemBuffer32::Get();
	int32_t num_ifl_materials = tsalloc.IMemBuffer32::Get();
	int32_t num_node_rots;
	int32_t num_node_trans;
	int32_t num_node_uniform_scales;
	int32_t num_node_aligned_scales;
	int32_t num_node_arbitrary_scales;
	if (current_read_version_ < 22)
	{
		num_node_rots = num_node_trans = tsalloc.IMemBuffer32::Get() - num_nodes;
		num_node_uniform_scales = num_node_aligned_scales = num_node_arbitrary_scales = 0;
	}
	else
	{
		num_node_rots = tsalloc.IMemBuffer32::Get();
		num_node_trans = tsalloc.IMemBuffer32::Get();
		num_node_uniform_scales = tsalloc.IMemBuffer32::Get();
		num_node_aligned_scales = tsalloc.IMemBuffer32::Get();
		num_node_arbitrary_scales = tsalloc.IMemBuffer32::Get();
	}
	int32_t num_ground_frames = 0;
	if (current_read_version_ > 23)
		num_ground_frames = tsalloc.IMemBuffer32::Get();
	int32_t num_object_states = tsalloc.IMemBuffer32::Get();
	int32_t num_decal_states = tsalloc.IMemBuffer32::Get();
	int32_t num_triggers = tsalloc.IMemBuffer32::Get();
	int32_t num_details = tsalloc.IMemBuffer32::Get();
	int32_t num_meshes = tsalloc.IMemBuffer32::Get();
	int32_t num_skins = 0;
	if (current_read_version_ < 23)
		// in later versions, skins are kept with other meshes
		num_skins = tsalloc.IMemBuffer32::Get();
	int32_t num_names = tsalloc.IMemBuffer32::Get();

	smallest_visible_size_ = static_cast<float>(tsalloc.IMemBuffer32::Get());
	smallest_visible_dl_ = tsalloc.IMemBuffer32::Get();

	tsalloc.CheckGuard();

	// get bounds...
	tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&radius_), 1);
	tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&tube_radius_), 1);
	tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&center_), 3);
	tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&bounds_), 6);

	tsalloc.CheckGuard();

	// copy various vectors...
	int32_t* ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_nodes * 5);
	Vector::Set(nodes_, ptr32, num_nodes);

	tsalloc.CheckGuard();

	ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_objects * 6, true);
	if (!ptr32)
		ptr32 = tsalloc.IMemBuffer32::AllocShape(num_skins * 6); // pre v23 shapes store skins and meshes separately...no longer
	else
		tsalloc.IMemBuffer32::AllocShape(num_skins * 6);
	Vector::Set(objects_, ptr32, num_objects);

	tsalloc.CheckGuard();

	// DEPRECATED decals
	ptr32 = tsalloc.IMemBuffer32::GetPointer(num_decals * 5);

	tsalloc.CheckGuard();

	// DEPRECATED ifl materials
	ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_ifl_materials * 5);

	tsalloc.CheckGuard();

	ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_sub_shapes, true);
	Vector::Set(sub_shape_first_node_, ptr32, num_sub_shapes);
	ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_sub_shapes, true);
	Vector::Set(sub_shape_first_object_, ptr32, num_sub_shapes);
	// DEPRECATED subShapeFirstDecal
	ptr32 = tsalloc.IMemBuffer32::GetPointer(num_sub_shapes);

	tsalloc.CheckGuard();

	ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_sub_shapes);
	Vector::Set(sub_shape_num_nodes_, ptr32, num_sub_shapes);
	ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_sub_shapes);
	Vector::Set(sub_shape_num_objects_, ptr32, num_sub_shapes);
	// DEPRECATED subShapeNumDecals
	ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_sub_shapes);

	tsalloc.CheckGuard();

	// get default translation and rotation
	int16_t* ptr16 = tsalloc.IMemBuffer16::AllocShape(0);
	for (i = 0; i < num_nodes; i++)
		tsalloc.IMemBuffer16::CopyToShape(4);
	Vector::Set(default_rotations_, ptr16, num_nodes);
	tsalloc.Align32();
	ptr32 = tsalloc.IMemBuffer32::AllocShape(0);
	for (i = 0; i < num_nodes; i++)
	{
		tsalloc.IMemBuffer32::CopyToShape(3);
		tsalloc.IMemBuffer32::CopyToShape(sizeof(Point3F) - 12); // handle alignment issues w/ point3f
	}
	Vector::Set(default_translations_, ptr32, num_nodes);

	// get any node sequence data stored in shape
	node_translations_.resize(num_node_trans);
	for (i = 0; i < num_node_trans; i++)
		tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&node_translations_[i]), 3);
	node_rotations_.resize(num_node_rots);
	for (i = 0; i < num_node_rots; i++)
		tsalloc.IMemBuffer16::Get(reinterpret_cast<int16_t*>(&node_rotations_[i]), 4);
	tsalloc.Align32();

	tsalloc.CheckGuard();

	if (current_read_version_ > 21)
	{
		// more node sequence data...scale
		node_uniform_scales_.resize(num_node_uniform_scales);
		for (i = 0; i < num_node_uniform_scales; i++)
			tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&node_uniform_scales_[i]), 1);
		node_aligned_scales_.resize(num_node_aligned_scales);
		for (i = 0; i < num_node_aligned_scales; i++)
			tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&node_aligned_scales_[i]), 3);
		node_arbitrary_scale_factors_.resize(num_node_arbitrary_scales);
		for (i = 0; i < num_node_arbitrary_scales; i++)
			tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&node_arbitrary_scale_factors_[i]), 3);
		node_arbitrary_scale_rots_.resize(num_node_arbitrary_scales);
		for (i = 0; i < num_node_arbitrary_scales; i++)
			tsalloc.IMemBuffer16::Get(reinterpret_cast<int16_t*>(&node_arbitrary_scale_rots_[i]), 4);
		tsalloc.Align32();

		tsalloc.CheckGuard();
	}

	if (current_read_version_ > 23)
	{
		ground_translations_.resize(num_ground_frames);
		for (i = 0; i < num_ground_frames; i++)
			tsalloc.IMemBuffer32::Get(reinterpret_cast<int32_t*>(&ground_translations_[i]), 3);
		ground_rotations_.resize(num_ground_frames);
		for (i = 0; i < num_ground_frames; i++)
			tsalloc.IMemBuffer16::Get(reinterpret_cast<int16_t*>(&ground_rotations_[i]), 4);
		tsalloc.Align32();

		tsalloc.CheckGuard();
	}

	// object states
	ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_object_states * 3);
	Vector::Set(object_states_, ptr32, num_object_states);
	tsalloc.IMemBuffer32::AllocShape(num_skins * 3); // provide buffer after object_states_ for older shapes

	tsalloc.CheckGuard();

	// DEPRECATED decal states
	ptr32 = tsalloc.IMemBuffer32::GetPointer(num_decal_states);

	tsalloc.CheckGuard();

	// frame triggers
	ptr32 = tsalloc.IMemBuffer32::GetPointer(num_triggers * 2);
	triggers_.resize(num_triggers);
	memcpy(Vector::Address(triggers_), ptr32, sizeof(int32_t) * num_triggers * 2);

	tsalloc.CheckGuard();

	// details
	ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_details * 7, true);
	Vector::Set(details_, ptr32, num_details);

	// Some DTS exporters (MAX - I'm looking at you!) write garbage into the
	// averageError and maxError values which stops LOD from working correctly.
	// Try to detect and fix it
	for (uint32_t i = 0; i < details_.size(); i++)
	{
		if ((details_[i].average_error == 0) || (details_[i].average_error > 10000) ||
			(details_[i].max_error == 0) || (details_[i].max_error > 10000))
		{
			details_[i].average_error = details_[i].max_error = -1.0f;
		}
	}

	// We don't trust the value of mSmallestVisibleDL loaded from the dts
	// since some legacy meshes seem to have the wrong value. Recalculate it
	// now that we have the details loaded.
	UpdateSmallestVisibleDL();

	tsalloc.CheckGuard();

	// about to read in the meshes...first must allocate some scratch space
	int32_t scratch_size = std::max(num_skins, num_meshes);
	TSMesh::verts_list_.resize(scratch_size);
	TSMesh::tverts_list_.resize(scratch_size);
	TSMesh::norms_list_.resize(scratch_size);
	TSMesh::encoded_norms_list_.resize(scratch_size);
	TSMesh::data_copied_.resize(scratch_size);
	TSSkinMesh::init_transform_list_.resize(scratch_size);
	TSSkinMesh::vertex_index_list_.resize(scratch_size);
	TSSkinMesh::bone_index_list_.resize(scratch_size);
	TSSkinMesh::weight_list_.resize(scratch_size);
	TSSkinMesh::node_index_list_.resize(scratch_size);
	for (i = 0; i < num_meshes; i++)
	{
		TSMesh::verts_list_[i] = nullptr;
		TSMesh::tverts_list_[i] = nullptr;
		TSMesh::norms_list_[i] = nullptr;
		TSMesh::encoded_norms_list_[i] = nullptr;
		TSMesh::data_copied_[i] = false;
		TSSkinMesh::init_transform_list_[i] = nullptr;
		TSSkinMesh::vertex_index_list_[i] = nullptr;
		TSSkinMesh::bone_index_list_[i] = nullptr;
		TSSkinMesh::weight_list_[i] = nullptr;
		TSSkinMesh::node_index_list_[i] = nullptr;
	}

	// read in the meshes (sans skins)...straightforward read one at a time
	TSMesh **ptrmesh = reinterpret_cast<TSMesh**>(tsalloc.IMemBuffer32::AllocShape((num_meshes + num_skins * num_details) * (sizeof(TSMesh*) / 4)));
	for (i = 0; i < num_meshes; i++)
	{
		bool skip = false;
		int32_t mesh_type = tsalloc.IMemBuffer32::Get();
		if (mesh_type == TSMesh::kDecalMeshType)
			// decal mesh deprecated
			skip = true;
		TSMesh* mesh = TSMesh::AssembleMesh(mesh_type, skip);
		if (ptrmesh)
		{
			ptrmesh[i] = skip ? nullptr : mesh;
		}

		// fill in location of verts, tverts, and normals for detail levels
		if (mesh && mesh_type != TSMesh::kDecalMeshType)
		{
			TSMesh::verts_list_[i] = Vector::Address(mesh->verts_);
			TSMesh::tverts_list_[i] = Vector::Address(mesh->tverts_);
			TSMesh::norms_list_[i] = Vector::Address(mesh->norms_);
			TSMesh::encoded_norms_list_[i] = Vector::Address(mesh->encoded_norms_);
			TSMesh::data_copied_[i] = !skip; // as long as we didn't skip this mesh, the data should be in shape now
			if (mesh_type == TSMesh::kSkinMeshType)
			{
				TSSkinMesh* skin = static_cast<TSSkinMesh*>(mesh);
				TSMesh::verts_list_[i] = Vector::Address(skin->initial_verts_);
				TSMesh::norms_list_[i] = Vector::Address(skin->initial_norms_);
				TSSkinMesh::init_transform_list_[i] = Vector::Address(skin->initial_transforms_);
				TSSkinMesh::vertex_index_list_[i] = Vector::Address(skin->vertex_index_);
				TSSkinMesh::bone_index_list_[i] = Vector::Address(skin->bone_index_);
				TSSkinMesh::weight_list_[i] = Vector::Address(skin->weight_);
				TSSkinMesh::node_index_list_[i] = Vector::Address(skin->node_index_);
			}
		}
	}
	Vector::Set(meshes_, ptrmesh, num_meshes);

	tsalloc.CheckGuard();

	// names
	char* name_buffer_start = (char*)tsalloc.IMemBuffer8::GetPointer(0);
	char* name = name_buffer_start;
	int32_t name_buffer_size = 0;
	names_.resize(num_names);
	for (i = 0; i < num_names; i++)
	{
		for (j = 0; name[j]; j++)
			;

		names_[i] = name;
		name_buffer_size += j + 1;
		name += j + 1;
	}

	tsalloc.IMemBuffer8::GetPointer(name_buffer_size);
	tsalloc.Align32();

	tsalloc.CheckGuard();

	if (current_read_version_ < 23)
	{
		// get detail information about skins...
		int32_t* det_first_skin = tsalloc.IMemBuffer32::GetPointer(num_details);
		int32_t* detail_num_skins = tsalloc.IMemBuffer32::GetPointer(num_details);

		tsalloc.CheckGuard();

		// skins
		ptr32 = tsalloc.IMemBuffer32::AllocShape(num_skins);
		for (i = 0; i < num_skins; i++)
		{
			bool skip = false;
			TSSkinMesh* skin = reinterpret_cast<TSSkinMesh*>(TSMesh::AssembleMesh(TSMesh::kSkinMeshType, skip));
			if (Vector::Address(meshes_))
			{
				// add pointer to skin in shapes list of meshes
				// we reserved room for this above...
				Vector::Set(meshes_, Vector::Address(meshes_), meshes_.size() + 1);
				meshes_[meshes_.size() - 1] = skip ? NULL : skin;
			}
		}

		tsalloc.CheckGuard();
	}
}

bool TSShape::LoadFromFile(const std::string& filename)
{
	std::ifstream ifs(filename, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
	{
		return false;
	}

	return LoadFromStream(ifs);
}

bool TSShape::LoadFromStream(std::istream &is)
{
	IStream stream(is);

	// read version - read handles endian-flip
	stream.Read(&current_read_version_);
	exporter_version_ = current_read_version_ >> 16;
	current_read_version_ &= 0xFF;
	if (current_read_version_ > kVersion)
	{
		// error -- don't support future versions yet :>
		return false;
	}
	if (current_read_version_ < 19)
	{
		return false;
	}
	read_version_ = current_read_version_;

	int32_t* mem_buffer32;
	int16_t* mem_buffer16;
	int8_t* mem_buffer8;
	int32_t count32, count16, count8;

	int32_t i;
	uint32_t size_mem_buffer, startU16, startU8;

	// in dwords. - read handles endian-flip
	stream.Read(&size_mem_buffer);
	stream.Read(&startU16);
	stream.Read(&startU8);

	int32_t* tmp = new int32_t[size_mem_buffer];
	stream.Read(reinterpret_cast<uint8_t *>(tmp), sizeof(int32_t) * size_mem_buffer);
	mem_buffer32 = tmp;
	mem_buffer16 = reinterpret_cast<int16_t *>(tmp + startU16);
	mem_buffer8 = reinterpret_cast<int8_t *>(tmp + startU8);

	count32 = startU16;
	count16 = startU8 - startU16;
	count8 = size_mem_buffer - startU8;

	// read sequences
	int32_t num_sequences;
	stream.Read(&num_sequences);
	sequences_.resize(num_sequences);
	for (i = 0; i < num_sequences; i++)
	{
		sequences_[i].LoadFromStream(stream);
	}

	// read material list
	delete material_list_; // just in case...
	material_list_ = new TSMaterialList;
	material_list_->LoadFromStream(stream);

	// since we read in the buffers, we need to endian-flip their entire contents...
	FixEndian(mem_buffer32, mem_buffer16, mem_buffer8, count32, count16, count8);

	tsalloc.SetRead(mem_buffer32, mem_buffer16, mem_buffer8, true);
	AssembleShape(); // determine size of buffer needed
	shape_data_size_ = tsalloc.GetSize();
	tsalloc.DoAlloc();
	shape_data_ = tsalloc.ITSShapeAlloc::GetBuffer();
	tsalloc.SetRead(mem_buffer32, mem_buffer16, mem_buffer8, false);
	AssembleShape(); // copy to buffer
	assert(tsalloc.GetSize() == shape_data_size_);

	delete[] mem_buffer32; // this covers all the buffers

	return is.good();
}

void TSShape::DisassembleShape()
{
	int32_t i;

	// set counts...
	int32_t num_nodes = tsalloc.OMemBuffer32::Set(nodes_.size());
	int32_t num_objects = tsalloc.OMemBuffer32::Set(objects_.size());
	tsalloc.OMemBuffer32::Set(0); // DEPRECATED decals
	int32_t num_sub_shapes = tsalloc.OMemBuffer32::Set(sub_shape_first_node_.size());
	tsalloc.OMemBuffer32::Set(0); // DEPRECATED ifl materials
	int32_t num_node_rotations = tsalloc.OMemBuffer32::Set(node_rotations_.size());
	int32_t num_node_translations = tsalloc.OMemBuffer32::Set(node_translations_.size());
	int32_t num_node_uniform_scales = tsalloc.OMemBuffer32::Set(node_uniform_scales_.size());
	int32_t num_node_aligned_scales = tsalloc.OMemBuffer32::Set(node_aligned_scales_.size());
	int32_t num_node_arbitrary_scales = tsalloc.OMemBuffer32::Set(node_arbitrary_scale_factors_.size());
	int32_t num_ground_frames = tsalloc.OMemBuffer32::Set(ground_translations_.size());
	int32_t num_object_states = tsalloc.OMemBuffer32::Set(object_states_.size());
	tsalloc.OMemBuffer32::Set(0); // DEPRECATED decals
	int32_t num_triggers = tsalloc.OMemBuffer32::Set(triggers_.size());
	int32_t num_details = tsalloc.OMemBuffer32::Set(details_.size());
	int32_t num_meshes = tsalloc.OMemBuffer32::Set(meshes_.size());
	int32_t num_names = tsalloc.OMemBuffer32::Set(names_.size());
	tsalloc.OMemBuffer32::Set(static_cast<int32_t>(smallest_visible_size_));
	tsalloc.OMemBuffer32::Set(smallest_visible_dl_);

	tsalloc.SetGuard();

	// get bounds...
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(&radius_), 1);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(&tube_radius_), 1);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(&center_), 3);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(&bounds_), 6);

	tsalloc.SetGuard();

	// copy various vectors...
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(nodes_)), num_nodes * 5);
	tsalloc.SetGuard();
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(objects_)), num_objects * 6);
	tsalloc.SetGuard();
	// DEPRECATED: no copy decals
	tsalloc.SetGuard();
	tsalloc.OMemBuffer32::CopyToBuffer(0, 0); // DEPRECATED: ifl materials!
	tsalloc.SetGuard();
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(sub_shape_first_node_)), num_sub_shapes);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(sub_shape_first_object_)), num_sub_shapes);
	tsalloc.OMemBuffer32::CopyToBuffer(0, num_sub_shapes); // DEPRECATED: no copy sub_shape_first_decal_
	tsalloc.SetGuard();
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(sub_shape_num_nodes_)), num_sub_shapes);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(sub_shape_num_objects_)), num_sub_shapes);
	tsalloc.OMemBuffer32::CopyToBuffer(0, num_sub_shapes); // DEPRECATED: no copy sub_shape_num_decals_
	tsalloc.SetGuard();

	// default transforms...
	tsalloc.OMemBuffer16::CopyToBuffer(reinterpret_cast<int16_t*>(Vector::Address(default_rotations_)), num_nodes * 4);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(default_translations_)), num_nodes * 3);

	// animated transforms...
	tsalloc.OMemBuffer16::CopyToBuffer(reinterpret_cast<int16_t*>(Vector::Address(node_rotations_)), num_node_rotations * 4);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(node_translations_)), num_node_translations * 3);

	tsalloc.SetGuard();

	// ...with scale
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(node_uniform_scales_)), num_node_uniform_scales);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(node_aligned_scales_)), num_node_aligned_scales * 3);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(node_arbitrary_scale_factors_)), num_node_arbitrary_scales * 3);
	tsalloc.OMemBuffer16::CopyToBuffer(reinterpret_cast<int16_t*>(Vector::Address(node_arbitrary_scale_rots_)), num_node_arbitrary_scales * 4);

	tsalloc.SetGuard();

	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(ground_translations_)), num_ground_frames * 3);
	tsalloc.OMemBuffer16::CopyToBuffer(reinterpret_cast<int16_t*>(Vector::Address(ground_rotations_)), num_ground_frames * 4);

	tsalloc.SetGuard();

	// object states..
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(object_states_)), num_object_states * 3);
	tsalloc.SetGuard();

	// decal states...
	// DEPRECATED (num_decal_states = 0)
	tsalloc.SetGuard();

	// frame triggers
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(triggers_)), num_triggers * 2);
	tsalloc.SetGuard();

	// details
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(details_)), num_details * 7);
	tsalloc.SetGuard();

	// read in the meshes (sans skins)...
	bool* is_mesh = new bool[num_meshes]; // funny business because decals are pretend meshes (legacy issue)
	for (i = 0; i < num_meshes; i++)
		is_mesh[i] = false;
	for (i = 0; i < objects_.size(); i++)
	{
		for (int32_t j = 0; j < objects_[i].num_meshes; j++)
			// even if an empty mesh, it's a mesh...
			is_mesh[objects_[i].start_mesh_index + j] = true;
	}
	for (i = 0; i < num_meshes; i++)
	{
		TSMesh* mesh = NULL;
		// decal mesh deprecated
		if (is_mesh[i])
			mesh = meshes_[i];
		tsalloc.OMemBuffer32::Set((mesh && mesh->GetMeshType() != TSMesh::kDecalMeshType) ? mesh->GetMeshType() : TSMesh::kNullMeshType);
		if (mesh)
			mesh->Disassemble();
	}
	delete[] is_mesh;
	tsalloc.SetGuard();

	// names
	for (i = 0; i < num_names; i++)
		tsalloc.OMemBuffer8::CopyToBuffer(reinterpret_cast<const int8_t*>(names_[i].c_str()), names_[i].length() + 1);

	tsalloc.SetGuard();
}

void TSShape::FixEndian(int32_t* buff32, int16_t* buff16, int8_t*, int32_t count32, int32_t count16, int32_t)
{
	// if endian-ness isn't the same, need to flip the buffer contents.
	if (!IsLittleEndian())
	{
		for (int32_t i = 0; i < count32; i++)
			buff32[i] = ConvertLEndianToHost(buff32[i]);
		for (int32_t i = 0; i < count16; i++)
			buff16[i] = ConvertLEndianToHost(buff16[i]);
	}
}

bool TSShape::WriteToFile(const std::string& filename)
{
	std::ofstream ofs(filename, std::ios::out | std::ios::binary);
	if (!ofs.is_open())
	{
		return false;
	}

	return WriteToStream(ofs);
}

bool TSShape::WriteToStream(std::ostream &os)
{
	OStream stream(os);

	// write version
	stream.Write(kVersion | (kMostRecentExporterVersion << 16));

	tsalloc.SetWrite();
	DisassembleShape();

	int32_t* buffer32 = tsalloc.OMemBuffer32::GetBuffer();
	int16_t* buffer16 = tsalloc.OMemBuffer16::GetBuffer();
	int8_t* buffer8 = tsalloc.OMemBuffer8::GetBuffer();

	int32_t size32 = tsalloc.OMemBuffer32::GetBufferSize();
	int32_t size16 = tsalloc.OMemBuffer16::GetBufferSize();
	int32_t size8 = tsalloc.OMemBuffer8::GetBufferSize();

	// convert sizes to dwords...
	if (size16 & 1)
		size16 += 2;
	size16 >>= 1;
	if (size8 & 3)
		size8 += 4;
	size8 >>= 2;

	int32_t size_mem_buffer, start16, start8;
	size_mem_buffer = size32 + size16 + size8;
	start16 = size32;
	start8 = start16 + size16;

	// in dwords -- write will properly endian-flip.
	stream.Write(size_mem_buffer);
	stream.Write(start16);
	stream.Write(start8);

	// endian-flip the entire write buffers.
	FixEndian(buffer32, buffer16, buffer8, size32, size16, size8);

	// now write buffers
	stream.Write(buffer32, size32 * 4);
	stream.Write(buffer16, size16 * 4);
	stream.Write(buffer8, size8 * 4);

	// write sequences - write will properly endian-flip.
	stream.Write(sequences_.size());
	for (int32_t i = 0; i < sequences_.size(); i++)
		sequences_[i].WriteToStream(stream);

	// write material list - write will properly endian-flip.
	material_list_->WriteToStream(stream);

	delete[] buffer32;
	delete[] buffer16;
	delete[] buffer8;

	return os.good();
}

} // namespace DTS
