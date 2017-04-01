#include "DTSShapeAlloc.h"

#include <cassert>

namespace DTS
{

void ITSShapeAlloc::SetRead(int32_t* buff32, int16_t* buff16, int8_t* buff8, bool clear)
{
	IMemBuffer32::SetRead(buff32);
	IMemBuffer16::SetRead(buff16);
	IMemBuffer8::SetRead(buff8);

	if (clear)
	{
		dest_ = nullptr;
		size_ = 0;
	}

	SetSkipMode(false);
}

void ITSShapeAlloc::DoAlloc()
{
	dest_ = new int8_t[size_];
	size_ = 0;
}

void ITSShapeAlloc::Align32()
{
	int32_t aligned = size_ + 3 & (~0x3);
	IMemBuffer8::AllocShape(aligned - size_);
}

void ITSShapeAlloc::CheckGuard()
{
	bool check32 = IMemBuffer32::CheckGuard();
	bool check16 = IMemBuffer16::CheckGuard();
	bool check8 = IMemBuffer8::CheckGuard();
	assert(check32);
	assert(check16);
	assert(check8);
}

void OTSShapeAlloc::SetWrite()
{
	OMemBuffer32::SetWrite();
	OMemBuffer16::SetWrite();
	OMemBuffer8::SetWrite();
}

void OTSShapeAlloc::SetGuard()
{
	OMemBuffer32::SetGuard();
	OMemBuffer16::SetGuard();
	OMemBuffer8::SetGuard();
}

} // namespace DTS
