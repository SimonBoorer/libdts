#include "DTSMaterialList.h"

#include "DTSStream.h"
#include "DTSShape.h"

namespace DTS
{

bool MaterialList::LoadFromStream(IStream& is)
{
	// check the stream version
	uint8_t version;
	if (is.Read(&version) && version != kBinaryFileVersion)
		return false;

	// how many materials?
	uint32_t count;
	if (!is.Read(&count))
		return false;

	// pre-size the vectors for efficiency
	material_names_.reserve(count);

	// read in the materials
	for (uint32_t i = 0; i < count; i++)
	{
		// Load the bitmap name
		char buffer[256];
		is.ReadString(buffer);
		if (!buffer[0])
		{
			assert(0);
			return false;
		}

		// Material paths are a legacy of Tribes tools,
		// strip them off...
		char *name = &buffer[strlen(buffer)];
		while (name != buffer && name[-1] != '/' && name[-1] != '\\')
			name--;

		// Add it to the list
		material_names_.push_back(name);
	}

	return is.Good();
}

bool MaterialList::WriteToStream(OStream& os)
{
	os.Write(kBinaryFileVersion);					// version
	os.Write(material_names_.size());				// material count

	uint32_t i;
	for (i = 0; i < material_names_.size(); i++)	// material names
		os.WriteString(material_names_[i].c_str());

	return os.Good();
}

bool TSMaterialList::LoadFromStream(IStream& is)
{
	if (!MaterialList::LoadFromStream(is))
		return false;

	Allocate(Size());

	uint32_t i;
	for (i = 0; i < Size(); i++)
		is.Read(&flags_[i]);
	for (i = 0; i < Size(); i++)
		is.Read(&reflectance_maps_[i]);
	for (i = 0; i < Size(); i++)
		is.Read(&bump_maps_[i]);
	for (i = 0; i < Size(); i++)
		is.Read(&detail_maps_[i]);

	if (TSShape::current_read_version_ == 25)
	{
		uint32_t dummy = 0;

		for (i = 0; i < Size(); i++)
			is.Read(&dummy);
	}

	for (i = 0; i < Size(); i++)
		is.Read(&detail_scales_[i]);

	if (TSShape::current_read_version_ > 20)
	{
		for (i = 0; i < Size(); i++)
			is.Read(&reflection_amounts_[i]);
	}
	else
	{
		for (i = 0; i < Size(); i++)
			reflection_amounts_[i] = 1.0f;
	}

	return is.Good();
}

bool TSMaterialList::WriteToStream(OStream& os)
{
	if (!MaterialList::WriteToStream(os))
		return false;

	uint32_t i;
	for (i = 0; i < Size(); i++)
		os.Write(flags_[i]);

	for (i = 0; i < Size(); i++)
		os.Write(reflectance_maps_[i]);

	for (i = 0; i < Size(); i++)
		os.Write(bump_maps_[i]);

	for (i = 0; i < Size(); i++)
		os.Write(detail_maps_[i]);

	for (i = 0; i < Size(); i++)
		os.Write(detail_scales_[i]);

	for (i = 0; i < Size(); i++)
		os.Write(reflection_amounts_[i]);

	return os.Good();
}

void TSMaterialList::Allocate(uint32_t sz)
{
	flags_.resize(sz);
	reflectance_maps_.resize(sz);
	bump_maps_.resize(sz);
	detail_maps_.resize(sz);
	detail_scales_.resize(sz);
	reflection_amounts_.resize(sz);
}

} // namespace DTS
