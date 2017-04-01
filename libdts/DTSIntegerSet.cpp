#include "DTSIntegerSet.h"

#include "DTSStream.h"

namespace DTS
{

constexpr int32_t SetUpto(int32_t upto)
{
	return (((1 << (upto & 31)) - 1) * 2 + 1); // careful not to shift more than 31 times
}

TSIntegerSet::TSIntegerSet()
{
	ClearAll();
}

inline void TSIntegerSet::Set(int32_t index)
{
	assert(index >= 0 && index < kMaxSetSize);

	bits_[index >> 5] |= 1 << (index & 31);
}

void TSIntegerSet::ClearAll(int32_t upto)
{
	assert(upto <= kMaxSetSize);

	memset(bits_, 0, (upto >> 5) * 4);
	if (upto & 31)
		bits_[upto >> 5] &= ~SetUpto(upto);
}

void TSIntegerSet::Insert(int32_t index, bool value)
{
	assert(index < kMaxSetSize);

	// shift bits in words after the insertion point
	uint32_t end_word = (End() >> 5) + 1;
	if (end_word >= kMaxSetDWords)
		end_word = kMaxSetDWords - 1;

	for (int32_t i = end_word; i > (index >> 5); i--)
	{
		bits_[i] = bits_[i] << 1;
		if (bits_[i - 1] & 0x80000000)
			bits_[i] |= 0x1;
	}

	// shift to create space in target word
	uint32_t low_mask = (1 << (index & 0x1f)) - 1;				// bits below the insert point
	uint32_t high_mask = ~(low_mask | (1 << (index & 0x1f)));	// bits above the insert point

	int32_t word = index >> 5;
	bits_[word] = ((bits_[word] << 1) & high_mask) | (bits_[word] & low_mask);

	// insert new value
	if (value)
		Set(index);
}

int32_t TSIntegerSet::End() const
{
	for (int32_t i = kMaxSetDWords - 1; i >= 0; i--)
	{
		// search for set bit one dword at a time
		uint32_t dword = bits_[i];
		if (bits_[i])
		{
			// got dword, now search one byte at a time
			int32_t j = 31;
			uint32_t mask = 0xFF000000;
			do
			{
				if (dword & mask)
				{
					// got byte, now one bit at a time
					uint32_t bit = mask & ~(mask >> 1); // grabs the highest bit
					do
					{
						if (dword & bit)
							return (i << 5) + j + 1;
						j--;
						bit >>= 1;
					} while (1);
				}
				mask >>= 8;
				j -= 8;
			} while (1);
		}
	}

	return 0;
}

bool TSIntegerSet::LoadFromStream(IStream& is)
{
	ClearAll();

	int32_t num_ints;
	is.Read(&num_ints); // don't care about this

	int32_t sz;
	is.Read(&sz);
	assert(sz <= kMaxSetDWords);

	for (int32_t i = 0; i < sz; i++) // now mirrors the write code...
		is.Read(&bits_[i]);

	return is.Good();
}

bool TSIntegerSet::WriteToStream(OStream& os)
{
	os.Write(static_cast<int32_t>(0)); // don't do this anymore, keep in to avoid versioning
	int32_t i, sz = 0;
	for (i = 0; i < kMaxSetDWords; i++)
		if (bits_[i] != 0)
			sz = i + 1;
	os.Write(sz);
	for (i = 0; i < sz; i++)
		os.Write(bits_[i]);

	return os.Good();
}

} // namespace DTS
