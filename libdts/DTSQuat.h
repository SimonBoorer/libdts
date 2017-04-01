#ifndef DTS_QUAT_H_
#define DTS_QUAT_H_

#include <cstdint>

namespace DTS
{

class MatrixF;

// unit quaternion class
class QuatF
{
public:
	const static QuatF kIdentity;

	QuatF() {} // no init constructor
	QuatF(float _x, float _y, float _z, float _w);
	QuatF(const MatrixF &m);

	QuatF& Set(float _x, float _y, float _z, float _w);
	QuatF& Set(const MatrixF& m);

	MatrixF* SetMatrix(MatrixF * mat) const;
	QuatF& Normalize();
	QuatF& Identity();

	float x, y, z, w;
};

inline QuatF::QuatF(float _x, float _y, float _z, float _w)
{
	Set(_x, _y, _z, _w);
}

inline QuatF::QuatF(const MatrixF &m)
{
	Set(m);
}

inline QuatF& QuatF::Set(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
	return *this;
}

inline QuatF& QuatF::Identity()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
	return *this;
}

// compressed quaternion class
class Quat16
{
public:
	static const float kMaxVal;

	QuatF GetQuatF() const;
	void Set(const QuatF& q);

	int16_t x, y, z, w;
};

} // namespace DTS

#endif // DTS_QUAT_H_
