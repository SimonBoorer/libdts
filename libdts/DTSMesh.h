#ifndef DTS_MESH_H_
#define DTS_MESH_H_

#include "DTSMath.h"
#include "DTSVector.h"

namespace DTS
{

struct TSDrawPrimitive
{
	enum
	{
		kTriangles = 0 << 30,		// bits 30 and 31 index element type
		kStrip = 1 << 30,			// bits 30 and 31 index element type
		kFan = 2 << 30,				// bits 30 and 31 index element type
		kIndexed = 0x20000000,		// use glDrawElements if indexed, glDrawArrays o.w.
		kNoMaterial = 0x10000000,	// set if no material (i.e., texture missing)
		kMaterialMask = ~(kStrip | kFan | kTriangles | kIndexed | kNoMaterial),
		kTypeMask = kStrip | kFan | kTriangles
	};

	int32_t start;
	int32_t num_elements;
	int32_t mat_index; // holds material index & element type (see above enum)
};

class TSMesh
{
public:
	enum
	{
		// types...
		kStandardMeshType = 0,
		kSkinMeshType = 1,
		kDecalMeshType = 2,
		kSortedMeshType = 3,
		kNullMeshType = 4,
		kTypeMask = kStandardMeshType | kSkinMeshType | kDecalMeshType | kSortedMeshType | kNullMeshType,

		// flags (stored with mesh_type_)...
		kBillboard = 0x80000000, kHasDetailTexture = 0x40000000,
		kBillboardZAxis = 0x20000000, kUseEncodedNormals = 0x10000000,
		kHasColor = 0x8000000, kHasTVert2 = 0x4000000,
		kFlagMask = kBillboard | kBillboardZAxis | kHasDetailTexture | kUseEncodedNormals | kHasColor | kHasTVert2
	};

	static const Point3F kU8ToNormalTable[];

	TSMesh();
	virtual ~TSMesh() {}

	uint32_t GetMeshType() const { return mesh_type_ & kTypeMask; }
	void SetFlags(uint32_t flag) { mesh_type_ |= flag; }
	uint32_t GetFlags(uint32_t flag = 0xFFFFFFFF) const { return mesh_type_ & flag; }

	virtual void CopySourceVertexDataFrom(const TSMesh* src_mesh);

	// Bounding Methods
	void ComputeBounds();
	void ComputeBounds(const MatrixF& transform, Box3F& bounds, int32_t frame, Point3F* center, float* radius);
	void ComputeBounds(const Point3F* v, int32_t numVerts, int32_t stride, const MatrixF& transform, Box3F& bounds, Point3F* center, float* radius);
	const Box3F& GetBounds() const { return bounds_; }
	const Point3F& GetCenter() const { return center_; }
	float GetRadius() const { return radius_; }

	static uint8_t EncodeNormal(const Point3F& normal);
	static const Point3F& DecodeNormal(uint8_t ncode) { return kU8ToNormalTable[ncode]; }

	// persist methods...
	virtual void Assemble(bool skip);
	static TSMesh* TSMesh::AssembleMesh(uint32_t mesh_type, bool skip);
	virtual void Disassemble();

	// methods used during assembly to share vertex and other info
	// between meshes (and for skipping detail levels on load)
	template<typename T>
	T* GetSharedData(int32_t parent_mesh, int32_t size, T** source, bool skip)
	{
		#define tsalloc TSShape::alloc_
		T* ptr;
		if (parent_mesh < 0)
			ptr = skip ? tsalloc.IMemBuffer<T>::GetPointer(size) : tsalloc.IMemBuffer<T>::CopyToShape(size);
		else
		{
			ptr = source[parent_mesh];
			// if we skipped the previous mesh (and we're not skipping this one) then
			// we still need to copy points into the shape...
			if (!data_copied_[parent_mesh] && !skip)
			{
				T* tmp = ptr;
				ptr = tsalloc.IMemBuffer<T>::AllocShape(size);
				if (ptr && tmp)
					memcpy(ptr, tmp, size * sizeof(int32_t));
			}
		}
		return ptr;
		#undef tsalloc
	}

	int32_t parent_mesh_; // index into shapes mesh list
	int32_t num_frames_;
	int32_t num_mat_frames_;
	int32_t verts_per_frame_;

	// Vertex data
	std::vector<Point3F> verts_;
	std::vector<Point3F> norms_;
	std::vector<Point2F> tverts_;

	std::vector<TSDrawPrimitive> primitives_;
	std::vector<uint8_t> encoded_norms_;
	std::vector<uint32_t> indices_;

	// Assembly Variables
	static std::vector<Point3F*>	verts_list_;
	static std::vector<Point3F*>	norms_list_;
	static std::vector<uint8_t*>	encoded_norms_list_;
	static std::vector<Point2F*>	tverts_list_;

	static std::vector<bool>		data_copied_;

protected:
	uint32_t mesh_type_;
	Box3F bounds_;
	Point3F center_;
	float radius_;
};

class TSSkinMesh : public TSMesh
{
public:
	TSSkinMesh();

	virtual void CopySourceVertexDataFrom(const TSMesh* src_mesh);

	// persist methods...
	void Assemble(bool skip);
	void Disassemble();

	// vectors that define the vertex, weight, bone tuples
	std::vector<float> weight_;
	std::vector<int32_t> bone_index_;
	std::vector<int32_t> vertex_index_;

	// vectors indexed by bone number
	std::vector<int32_t> node_index_;
	std::vector<MatrixF> initial_transforms_;

	// initial values of verts and normals
	// these get transformed into initial bone space,
	// from there into world space relative to current bone
	// pos, and then weighted by bone weights...
	std::vector<Point3F> initial_verts_;
	std::vector<Point3F> initial_norms_;

	// variables used during assembly (for skipping mesh detail levels
	// on load and for sharing verts between meshes)
	static std::vector<MatrixF*>	init_transform_list_;
	static std::vector<int32_t*>	vertex_index_list_;
	static std::vector<int32_t*>	bone_index_list_;
	static std::vector<float*>		weight_list_;
	static std::vector<int32_t*>	node_index_list_;
};

} // namespace DTS

#endif // DTS_MESH_H_
