#ifndef DTS_VECTOR_H_
#define DTS_VECTOR_H_

#include <vector>

namespace DTS
{

namespace Vector
{

template<class T>
T* Address(std::vector<T>& vector)
{
	if (vector.empty())
		return nullptr;

	return vector.data();
}

template<class T>
void Insert(std::vector<T>& vector, std::size_t index)
{
	assert(index <= vector.size());
	vector.insert(vector.begin() + index, T());
}

template<class T>
void Insert(std::vector<T>& vector, std::size_t index, const T& x)
{
	assert(index <= vector.size());
	vector.insert(vector.begin() + index, x);
}

template<class T>
void Merge(std::vector<T>& vector, const T* addr, std::size_t count)
{
	const std::size_t old_size = vector.size();
	const std::size_t new_size = vector.size() + count;
	vector.resize(new_size);

	T* dest = vector.data() + old_size;
	while (dest < vector.data() + new_size)
		new (dest++) T(*addr++);
}

template<class T>
void Set(std::vector<T>& vector, void* addr, std::size_t sz)
{
	if (!addr)
		sz = 0;

	vector.resize(sz);

	if (addr && sz > 0)
		memcpy(vector.data(), addr, sz * sizeof(T));
}

} // namespace Vector

} // namespace DTS

#endif // DTS_VECTOR_H_
