#ifndef DTS_STREAM_H_
#define DTS_STREAM_H_

#include <iostream>
#include <cstdint>
#include <cassert>
#include <cstring>

#include "DTSEndian.h"

namespace DTS
{

// Base stream class for streaming data across a specific media
class IStream
{
public:
	IStream(std::istream& is) :
		is_(is) {}

	bool Good()
	{
		return is_.good();
	}

	template <typename T>
	bool Read(T* out_read)
	{
		T temp;
		bool success = Read(&temp, sizeof(T));
		*out_read = ConvertLEndianToHost(temp);
		return success;
	}
	
	bool Read(bool* out_read)
	{
		uint8_t translate;
		bool success = Read(&translate);
		if (success == false)
			return false;

		*out_read = translate != 0;
		return true;
	}

	bool Read(void* buffer, int32_t num_bytes)
	{
		// exit on pre-existing errors
		if (!is_.good())
			return false;

		// if a request of non-zero length was made
		if (0 != num_bytes)
		{
			is_.read(reinterpret_cast<char *>(buffer), num_bytes);
			return is_.good();
		}

		return true;
	}

	void ReadString(char buf[256])
	{
		uint8_t len;
		Read(&len);
		Read(buf, static_cast<int32_t>(len));
		buf[len] = 0;
	}

private:
	std::istream& is_;
};

class OStream
{
public:
	OStream(std::ostream& os) :
		os_(os) {}

	bool Good()
	{
		return os_.good();
	}

	template <typename T>
	bool Write(T in_write)
	{
		T temp = ConvertHostToLEndian(in_write);
		return Write(&temp, sizeof(T));
	}

	bool Write(bool in_write)
	{
		uint8_t translate = in_write ? uint8_t(1) : uint8_t(0);
		return Write(translate);
	}

	bool Write(const void* buffer, int32_t num_bytes)
	{
		// exit on pre-existing errors
		if (!os_.good())
			return false;

		// if a request of non-zero length was made
		if (0 != num_bytes)
		{
			os_.write(reinterpret_cast<const char *>(buffer), num_bytes);
			return os_.good();
		}

		return true;
	}

	void WriteString(const char* string, int32_t max_len = 255)
	{
		int32_t len = strlen(string);
		if (len > max_len)
			len = max_len;

		Write(static_cast<uint8_t>(len));
		if (len)
			Write(string, len);
	}

private:
	std::ostream& os_;
};

} // namespace DTS

#endif // DTS_STREAM_H_
