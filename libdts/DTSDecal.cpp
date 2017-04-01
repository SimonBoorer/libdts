#include "DTSDecal.h"

#include "DTSShape.h"

namespace DTS
{

#define tsalloc TSShape::alloc_

void TSDecalMesh::Assemble(bool skip)
{
	if (TSShape::current_read_version_ < 20)
	{
		// read empty mesh...decals used to be derived from meshes
		tsalloc.CheckGuard();
		tsalloc.IMemBuffer32::GetPointer(15);
	}

	int32_t sz = tsalloc.IMemBuffer32::Get();
	int32_t* ptr32 = tsalloc.IMemBuffer32::CopyToShape(0); // get current shape address w/o doing anything
	for (int32_t i = 0; i < sz; i++)
	{
		tsalloc.IMemBuffer16::GetPointer(2);
		tsalloc.IMemBuffer32::GetPointer(1);
	}
	tsalloc.Align32();
	Vector::Set(primitives_, ptr32, sz);

	sz = tsalloc.IMemBuffer32::Get();
	int16_t* ptr16 = tsalloc.IMemBuffer16::GetPointer(sz);
	tsalloc.Align32();
	Vector::Set(indices_, ptr16, sz);

	if (TSShape::current_read_version_ < 20)
	{
		// read more empty mesh stuff...decals used to be derived from meshes
		tsalloc.IMemBuffer32::GetPointer(3);
		tsalloc.CheckGuard();
	}

	sz = tsalloc.IMemBuffer32::Get();
	ptr32 = tsalloc.IMemBuffer32::GetPointer(sz);
	Vector::Set(start_primitive_, ptr32, sz);

	ptr32 = tsalloc.IMemBuffer32::GetPointer(sz * 4);
	Vector::Set(texgen_S_, ptr32, start_primitive_.size());
	ptr32 = tsalloc.IMemBuffer32::GetPointer(sz * 4);
	Vector::Set(texgen_T_, ptr32, start_primitive_.size());

	material_index_ = tsalloc.IMemBuffer32::Get();

	tsalloc.CheckGuard();
}

void TSDecalMesh::Disassemble()
{
	tsalloc.OMemBuffer32::Set(primitives_.size());
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(primitives_)), primitives_.size());

	tsalloc.OMemBuffer32::Set(indices_.size());
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(indices_)), indices_.size());

	tsalloc.OMemBuffer32::Set(start_primitive_.size());
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(start_primitive_)), start_primitive_.size());

	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(texgen_S_)), texgen_S_.size() * 4);
	tsalloc.OMemBuffer32::CopyToBuffer(reinterpret_cast<int32_t*>(Vector::Address(texgen_T_)), texgen_T_.size() * 4);

	tsalloc.OMemBuffer32::Set(material_index_);

	tsalloc.SetGuard();
}

} // namespace DTS
