#ifndef DTS_MATERIALLIST_H_
#define DTS_MATERIALLIST_H_

#include "DTSVector.h"

namespace DTS
{

class IStream;
class OStream;

class MaterialList
{
public:
	uint32_t Size() const { return static_cast<uint32_t>(material_names_.size()); }

	bool LoadFromStream(IStream& is);
	bool WriteToStream(OStream& os);

private:
	static const int8_t kBinaryFileVersion = 1;

	std::vector<std::string> material_names_;
};

// Specialized material list for 3space objects.
class TSMaterialList : public MaterialList
{
public:
	enum
	{
		kS_Wrap = 0x0001,
		kT_Wrap = 0x0002,
		kTranslucent = 0x0004,
		kAdditive = 0x0008,
		kSubtractive = 0x0010,
		kSelfIlluminating = 0x0020,
		kNeverEnvMap = 0x0040,
		kNoMipMap = 0x0080,
		kMipMap_ZeroBorder = 0x0100,
		kAuxiliaryMap = 0x8000000 | 0x10000000 | 0x20000000 | 0x40000000 | 0x80000000 // DEPRECATED
	};

	bool LoadFromStream(IStream& is);
	bool WriteToStream(OStream& os);

private:
	void Allocate(uint32_t sz);

	std::vector<uint32_t> flags_;
	std::vector<uint32_t> reflectance_maps_;
	std::vector<uint32_t> bump_maps_;
	std::vector<uint32_t> detail_maps_;
	std::vector<float> detail_scales_;
	std::vector<float> reflection_amounts_;
};

} // namespace DTS

#endif // DTS_MATERIALLIST_H_
