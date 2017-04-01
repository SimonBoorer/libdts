#ifndef DTS_ENDIAN_H_
#define DTS_ENDIAN_H_

#include <cstdint>

namespace DTS
{

// Endian conversions
static inline bool IsLittleEndian()
{
	const std::uint32_t i = 0x1;
	return reinterpret_cast<const char&>(i) == 1;
}

inline uint8_t EndianSwap(const uint8_t in_swap)
{
	return in_swap;
}

inline int8_t EndianSwap(const int8_t in_swap)
{
	return in_swap;
}

// Convert the byte ordering on the uint16_t to and from big/little endian format.
inline uint16_t EndianSwap(const uint16_t in_swap)
{
	return uint16_t(((in_swap >> 8) & 0x00ff) |
		((in_swap << 8) & 0xff00));
}

inline int16_t EndianSwap(const int16_t in_swap)
{
	return static_cast<int16_t>(EndianSwap(static_cast<uint16_t>(in_swap)));
}

// Convert the byte ordering on the uint32_t to and from big/little endian format.
inline uint32_t EndianSwap(const uint32_t in_swap)
{
	return uint32_t(((in_swap >> 24) & 0x000000ff) |
		((in_swap >> 8) & 0x0000ff00) |
		((in_swap << 8) & 0x00ff0000) |
		((in_swap << 24) & 0xff000000));
}

inline int32_t EndianSwap(const int32_t in_swap)
{
	return static_cast<int32_t>(EndianSwap(static_cast<uint32_t>(in_swap)));
}

inline float EndianSwap(const float in_swap)
{
	return static_cast<float>(EndianSwap(static_cast<uint32_t>(in_swap)));
}

template <typename T>
inline T ConvertHostToLEndian(T i)
{
	if (IsLittleEndian())
		return i;

	return EndianSwap(i);
}

template <typename T>
inline T ConvertLEndianToHost(T i)
{
	if (IsLittleEndian())
		return i;

	return EndianSwap(i);
}

} // namespace DTS

#endif // DTS_ENDIAN_H_
