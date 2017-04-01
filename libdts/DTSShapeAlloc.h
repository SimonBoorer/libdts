#ifndef DTS_SHAPEALLOC_H_
#define DTS_SHAPEALLOC_H_

#include <cstdint>

namespace DTS
{

// Alloc structure used in the reading/writing of shapes.
class IMemBufferBase
{
public:
	IMemBufferBase() :
		dest_(nullptr), size_(0), mult_(0) {}
	virtual ~IMemBufferBase() {}

protected:
	int8_t* dest_;
	int32_t size_;
	int32_t mult_;  // mult incoming sizes by this (when 0, then dest_ doesn't grow --> skip mode)
};

template <typename T>
class IMemBuffer : public virtual IMemBufferBase
{
public:
	IMemBuffer() :
		mem_buffer_(nullptr), mem_guard_(0), save_guard_(0) {}

	void SetRead(T* buff)
	{
		mem_buffer_ = buff;
		mem_guard_ = 0;
		save_guard_ = 0;
	}

	// reads one or more entries of type from input buffer(doesn't affect output buffer)
	T Get()
	{
		return *mem_buffer_++;
	}

	void Get(T* dest, int32_t num)
	{
		memcpy(dest, mem_buffer_, sizeof(T) * num);
		mem_buffer_ += num;
	}

	// copies entries of type from input buffer to output buffer
	T* CopyToShape(int32_t num, bool return_something = false)
	{
		T* ret = (!return_something || dest_) ? reinterpret_cast<T*>(dest_) : mem_buffer_;
		if (dest_)
		{
			memcpy(dest_, reinterpret_cast<int8_t*>(mem_buffer_),
				mult_ * num * sizeof(T));
			dest_ += mult_ * num * sizeof(T);
		}
		mem_buffer_ += num;
		size_ += sizeof(T) * mult_ * num;
		return ret;
	}

	// gets pointer to next entries of type in input buffer (no effect on input buffer)
	T* GetPointer(int32_t num)
	{
		T* ret = reinterpret_cast<T*>(mem_buffer_);
		mem_buffer_ += num;
		return ret;
	}

	// creates room for entries of type in output buffer (no effect on input buffer)
	T* AllocShape(int32_t num)
	{
		T* ret = reinterpret_cast<T*>(dest_);
		if (dest_)
			dest_ += mult_ * num * sizeof(T);
		size_ += sizeof(T) * mult_ * num;
		return ret;
	}

	bool CheckGuard()
	{
		save_guard_ = Get();
		bool ret = (save_guard_ == mem_guard_);
		mem_guard_++;
		return ret;
	}

private:
	T* mem_buffer_;
	T mem_guard_;
	T save_guard_;
};

using IMemBuffer32 = IMemBuffer<int32_t>;
using IMemBuffer16 = IMemBuffer<int16_t>;
using IMemBuffer8 = IMemBuffer<int8_t>;

class ITSShapeAlloc : public IMemBuffer32, public IMemBuffer16, public IMemBuffer8
{
public:
	void SetRead(int32_t* buff32, int16_t* buff16, int8_t* buff8, bool clear);

	void DoAlloc();
	void Align32(); // align on dword boundary
	int8_t* GetBuffer() { return dest_; }
	int32_t GetSize() { return size_; }
	void SetSkipMode(bool skip) { mult_ = skip ? 0 : 1; }

	void CheckGuard();
};

template <typename T>
class OMemBuffer
{
public:
	static const int32_t kPageSize = 1024;	// PageSize must be multiple of 4 so that we can always
											// "over-read" up to next dword

	OMemBuffer::OMemBuffer()
		: mem_buffer_(nullptr), size_(0), full_size_(0), mem_guard_(0) {}

	void SetWrite()
	{
		mem_buffer_ = nullptr;
		size_ = full_size_ = 0;
		mem_guard_ = 0;
	}

	T* GetBuffer()
	{
		return mem_buffer_;
	}

	int32_t GetBufferSize()
	{
		return size_;
	}

	void SetGuard()
	{
		Set(mem_guard_);
		mem_guard_++;
	}

	T* Extend(int32_t add)
	{
		if (size_ + add > full_size_)
		{
			int32_t num_pages = 1 + (full_size_ + add) / kPageSize;
			full_size_ = num_pages * kPageSize;
			T* temp = new T[full_size_];
			memcpy(temp, mem_buffer_, size_ * sizeof(T));
			delete[] mem_buffer_;
			mem_buffer_ = temp;
		}

		T* ret = mem_buffer_ + size_;
		size_ += add;
		return ret;
	}

	//adds one entry to buffer
	T Set(T entry)
	{
		*Extend(1) = entry;
		return entry;
	}

	// adds count entries to buffer
	void CopyToBuffer(const T* entries, int32_t count)
	{
		if (entries)
			memcpy(Extend(count), entries, count * sizeof(T));
		else
			memset(Extend(count), 0, count * sizeof(T));
	}

private:
	T* mem_buffer_;
	int32_t size_;
	int32_t full_size_;
	T mem_guard_;
};

using OMemBuffer32 = OMemBuffer<int32_t>;
using OMemBuffer16 = OMemBuffer<int16_t>;
using OMemBuffer8 = OMemBuffer<int8_t>;

class OTSShapeAlloc : public OMemBuffer32, public OMemBuffer16, public OMemBuffer8
{
public:
	void SetWrite();

	void SetGuard();
};

class TSShapeAlloc : public ITSShapeAlloc, public OTSShapeAlloc
{

};

} // namespace DTS

#include <istream>

#endif // DTS_SHAPEALLOC_H_
