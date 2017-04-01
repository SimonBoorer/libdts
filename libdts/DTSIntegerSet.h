#ifndef DTS_INTEGERSET_H_
#define DTS_INTEGERSET_H_

#include <cstdint>

namespace DTS
{

class IStream;
class OStream;

// The standard mathmatical set, where there are no duplicates.  However,
// this set uses bits instead of numbers.
class TSIntegerSet
{
public:
	static const int32_t kMaxSetDWords = 64;
	static const int32_t kMaxSetSize = kMaxSetDWords * 32;

	TSIntegerSet();

	// Set this bit to true
	void Set(int32_t index);

	// Sets all bits to false
	void ClearAll(int32_t upto = kMaxSetSize);

	void Insert(int32_t index, bool value);

	int32_t End() const;

	bool LoadFromStream(IStream& is);
	bool WriteToStream(OStream& os);

private:
	// The bits!
	uint32_t bits_[kMaxSetDWords];
};

} // namespace DTS

#endif // DTS_INTERGERSET_H_
