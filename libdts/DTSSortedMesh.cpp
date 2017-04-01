#include "DTSSortedMesh.h"
#include "DTSShape.h"
#include "DTSVector.h"

namespace DTS
{

#define tsalloc TSShape::alloc_

void TSSortedMesh::Assemble(bool skip)
{
	TSMesh::Assemble(skip);

	int32_t num_clusters = tsalloc.IMemBuffer32::Get();
	int32_t* ptr32 = tsalloc.IMemBuffer32::CopyToShape(num_clusters * 8);
	Vector::Set(clusters_, ptr32, num_clusters);

	int32_t sz = tsalloc.IMemBuffer32::Get();
	ptr32 = tsalloc.IMemBuffer32::CopyToShape(sz);
	Vector::Set(clusters_, ptr32, sz);

	sz = tsalloc.IMemBuffer32::Get();
	ptr32 = tsalloc.IMemBuffer32::CopyToShape(sz);
	Vector::Set(first_verts_, ptr32, sz);

	sz = tsalloc.IMemBuffer32::Get();
	ptr32 = tsalloc.IMemBuffer32::CopyToShape(sz);
	Vector::Set(num_verts_, ptr32, sz);

	sz = tsalloc.IMemBuffer32::Get();
	ptr32 = tsalloc.IMemBuffer32::CopyToShape(sz);
	Vector::Set(first_tverts_, ptr32, sz);

	always_write_depth_ = tsalloc.IMemBuffer32::Get() != 0;

	tsalloc.CheckGuard();
}

void TSSortedMesh::Disassemble()
{
	TSMesh::Disassemble();

	tsalloc.OMemBuffer32::Set(clusters_.size());
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(clusters_)), clusters_.size() * 8);

	tsalloc.OMemBuffer32::Set(start_cluster_.size());
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(start_cluster_)), start_cluster_.size());

	tsalloc.OMemBuffer32::Set(first_verts_.size());
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(first_verts_)), first_verts_.size());

	tsalloc.OMemBuffer32::Set(num_verts_.size());
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(num_verts_)), num_verts_.size());

	tsalloc.OMemBuffer32::Set(first_tverts_.size());
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(first_tverts_)), first_tverts_.size());

	tsalloc.OMemBuffer32::Set(always_write_depth_ ? 1 : 0);

	tsalloc.SetGuard();
}

} // namespace DTS
