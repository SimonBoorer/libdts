#ifndef DTS_POINT3_H_
#define DTS_POINT3_H_

#include <algorithm>
#include <cassert>

namespace DTS
{

// 3D floating-point point.
class Point3F
{
public:
	const static Point3F kZero;

	Point3F() {}

	Point3F(float x, float y, float z)
		: x(x), y(y), z(z) {}

	void SetMin(const Point3F& test);
	void SetMax(const Point3F& test);

	operator float*() { return &x; }

	void Normalize();
	void Convolve(const Point3F& c);

	// Arithmetic w/ other points
	Point3F operator+(const Point3F& add) const;
	Point3F operator-(const Point3F& sub) const;
	Point3F& operator+=(const Point3F& add);
	Point3F& operator-=(const Point3F& sub);

	// Arithmetic w/ scalars
	Point3F operator*(float mul) const;
	Point3F operator/(float div) const;

	float x;
	float y;
	float z;
};

using VectorF = Point3F;

inline void Point3F::SetMin(const Point3F& test)
{
	x = std::min(x, test.x);
	y = std::min(y, test.y);
	z = std::min(z, test.z);
}

inline void Point3F::SetMax(const Point3F& test)
{
	x = std::max(x, test.x);
	y = std::max(y, test.y);
	z = std::max(z, test.z);
}

inline void Point3F::Normalize()
{
	float squared = x * x + y * y + z * z;
	if (squared != 0.0f)
	{
		float factor = 1.0f / sqrt(squared);
		x *= factor;
		y *= factor;
		z *= factor;
	}
	else
	{
		x = 0.0f;
		y = 0.0f;
		z = 1.0f;
	}
}

inline void Point3F::Convolve(const Point3F& c)
{
	x *= c.x;
	y *= c.y;
	z *= c.z;
}

inline Point3F Point3F::operator+(const Point3F& add) const
{
	return Point3F(x + add.x, y + add.y, z + add.z);
}

inline Point3F Point3F::operator-(const Point3F& sub) const
{
	return Point3F(x - sub.x, y - sub.y, z - sub.z);
}

inline Point3F& Point3F::operator+=(const Point3F& add)
{
	x += add.x;
	y += add.y;
	z += add.z;

	return *this;
}

inline Point3F& Point3F::operator-=(const Point3F& sub)
{
	x -= sub.x;
	y -= sub.y;
	z -= sub.z;

	return *this;
}

inline Point3F Point3F::operator*(float mul) const
{
	return Point3F(x * mul, y * mul, z * mul);
}

inline Point3F Point3F::operator/(float div) const
{
	assert(div != 0.0f);

	float inv = 1.0f / div;

	return Point3F(x * inv, y * inv, z * inv);
}

inline Point3F operator*(float mul, const Point3F& multiplicand)
{
	return multiplicand * mul;
}

namespace Math
{

inline float Dot(const Point3F& p1, const Point3F& p2)
{
	return (p1.x * p2.x + p1.y * p2.y + p1.z * p2.z);
}

inline void Cross(const Point3F& a, const Point3F& b, Point3F *res)
{
	res->x = (a.y * b.z) - (a.z * b.y);
	res->y = (a.z * b.x) - (a.x * b.z);
	res->z = (a.x * b.y) - (a.y * b.x);
}

inline Point3F Cross(const Point3F& a, const Point3F& b)
{
	Point3F r;
	Cross(a, b, &r);
	return r;
}

} // namespace Math

} // namespace DTS

#endif DTS_POINT3_H_
