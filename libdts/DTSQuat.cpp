#include "DTSQuat.h"

#include <cmath>

#include "DTSMatrix.h"

namespace DTS
{

const QuatF QuatF::kIdentity(0.0f, 0.0f, 0.0f, 1.0f);

const float Quat16::kMaxVal = 0x7fff;

constexpr int Idx(int r, int c)
{
	return r * 4 + c;
}

QuatF& QuatF::Set(const MatrixF& mat)
{
	float const *m = mat;

	float trace = m[0] + m[5] + m[10];
	if (trace > 0.0f)
	{
		float s = sqrt(trace + float(1));
		w = s * 0.5f;
		s = 0.5f / s;
		x = (m[Idx(1, 2)] - m[Idx(2, 1)]) * s;
		y = (m[Idx(2, 0)] - m[Idx(0, 2)]) * s;
		z = (m[Idx(0, 1)] - m[Idx(1, 0)]) * s;
	}
	else
	{
		float* q = &x;
		uint32_t i = 0;
		if (m[Idx(1, 1)] > m[Idx(0, 0)]) i = 1;
		if (m[Idx(2, 2)] > m[Idx(i, i)]) i = 2;
		uint32_t j = (i + 1) % 3;
		uint32_t k = (j + 1) % 3;

		float s = sqrt((m[Idx(i, i)] - (m[Idx(j, j)] + m[Idx(k, k)])) + 1.0f);
		q[i] = s * 0.5f;
		s = 0.5f / s;
		q[j] = (m[Idx(i, j)] + m[Idx(j, i)]) * s;
		q[k] = (m[Idx(i, k)] + m[Idx(k, i)]) * s;
		w = (m[Idx(j, k)] - m[Idx(k, j)]) * s;
	}

	Normalize();

	return *this;
}

MatrixF* QuatF::SetMatrix(MatrixF* mat) const
{
	if (x * x + y * y + z * z < 10E-20f) // IsIdentity() -- substituted code a little more stringent but a lot faster
		mat->Identity();
	else
	{
		float* m = *mat;
		float xs = x * 2.0f;
		float ys = y * 2.0f;
		float zs = z * 2.0f;
		float wx = w * xs;
		float wy = w * ys;
		float wz = w * zs;
		float xx = x * xs;
		float xy = x * ys;
		float xz = x * zs;
		float yy = y * ys;
		float yz = y * zs;
		float zz = z * zs;
		m[Idx(0, 0)] = 1.0f - (yy + zz);
		m[Idx(1, 0)] = xy - wz;
		m[Idx(2, 0)] = xz + wy;
		m[Idx(3, 0)] = 0.0f;
		m[Idx(0, 1)] = xy + wz;
		m[Idx(1, 1)] = 1.0f - (xx + zz);
		m[Idx(2, 1)] = yz - wx;
		m[Idx(3, 1)] = 0.0f;
		m[Idx(0, 2)] = xz - wy;
		m[Idx(1, 2)] = yz + wx;
		m[Idx(2, 2)] = 1.0f - (xx + yy);
		m[Idx(3, 2)] = 0.0f;

		m[Idx(0, 3)] = 0.0f;
		m[Idx(1, 3)] = 0.0f;
		m[Idx(2, 3)] = 0.0f;
		m[Idx(3, 3)] = 1.0f;
	}
	return mat;
}

QuatF &QuatF::Normalize()
{
	float l = sqrt(x * x + y * y + z * z + w * w);
	if (l == 0.0f)
		Identity();
	else
	{
		x /= l;
		y /= l;
		z /= l;
		w /= l;
	}
	return *this;
}

QuatF Quat16::GetQuatF() const
{
	return QuatF(static_cast<float>(x) / kMaxVal,
		static_cast<float>(y) / kMaxVal,
		static_cast<float>(z) / kMaxVal,
		static_cast<float>(w) / kMaxVal);
}

void Quat16::Set(const QuatF& q)
{
	x = static_cast<int16_t>(q.x * kMaxVal);
	y = static_cast<int16_t>(q.y * kMaxVal);
	z = static_cast<int16_t>(q.z * kMaxVal);
	w = static_cast<int16_t>(q.w * kMaxVal);
}

} // namespace DTS
