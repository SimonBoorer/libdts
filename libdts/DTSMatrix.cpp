#include "DTSMatrix.h"

namespace DTS
{

MatrixF& MatrixF::Inverse()
{
	// using Cramers Rule find the Inverse
	// Minv = (1/det(M)) * adjoint(M)
	float det = Determinant();
	assert(det != 0.0f);

	float inv_det = 1.0f / det;
	float temp[16];

	temp[0] = (m_[5] * m_[10] - m_[6] * m_[9]) * inv_det;
	temp[1] = (m_[9] * m_[2] - m_[10] * m_[1]) * inv_det;
	temp[2] = (m_[1] * m_[6] - m_[2] * m_[5]) * inv_det;

	temp[4] = (m_[6] * m_[8] - m_[4] * m_[10])* inv_det;
	temp[5] = (m_[10] * m_[0] - m_[8] * m_[2]) * inv_det;
	temp[6] = (m_[2] * m_[4] - m_[0] * m_[6]) * inv_det;

	temp[8] = (m_[4] * m_[9] - m_[5] * m_[8]) * inv_det;
	temp[9] = (m_[8] * m_[1] - m_[9] * m_[0]) * inv_det;
	temp[10] = (m_[0] * m_[5] - m_[1] * m_[4]) * inv_det;

	m_[0] = temp[0];
	m_[1] = temp[1];
	m_[2] = temp[2];

	m_[4] = temp[4];
	m_[5] = temp[5];
	m_[6] = temp[6];

	m_[8] = temp[8];
	m_[9] = temp[9];
	m_[10] = temp[10];

	// invert the translation
	VectorF p;
	p.x = -m_[3];
	p.y = -m_[7];
	p.z = -m_[11];

	MulV(&p.x, &temp[4]);
	m_[3] = temp[4];
	m_[7] = temp[5];
	m_[11] = temp[6];
	return (*this);
}

float MatrixF::Determinant()
{
	return m_[0] * (m_[5] * m_[10] - m_[6] * m_[9]) +
		m_[4] * (m_[2] * m_[9] - m_[1] * m_[10]) +
		m_[8] * (m_[1] * m_[6] - m_[2] * m_[5]);
}

MatrixF& MatrixF::Transpose()
{
	std::swap(m_[1], m_[4]);
	std::swap(m_[2], m_[8]);
	std::swap(m_[3], m_[12]);
	std::swap(m_[6], m_[9]);
	std::swap(m_[7], m_[13]);
	std::swap(m_[11], m_[14]);
	return (*this);
}

MatrixF& MatrixF::Mul(const MatrixF &a, const MatrixF &b)
{
	assert((&a != this) && (&b != this));

	m_[0] = a.m_[0] * b.m_[0] + a.m_[1] * b.m_[4] + a.m_[2] * b.m_[8] + a.m_[3] * b.m_[12];
	m_[1] = a.m_[0] * b.m_[1] + a.m_[1] * b.m_[5] + a.m_[2] * b.m_[9] + a.m_[3] * b.m_[13];
	m_[2] = a.m_[0] * b.m_[2] + a.m_[1] * b.m_[6] + a.m_[2] * b.m_[10] + a.m_[3] * b.m_[14];
	m_[3] = a.m_[0] * b.m_[3] + a.m_[1] * b.m_[7] + a.m_[2] * b.m_[11] + a.m_[3] * b.m_[15];

	m_[4] = a.m_[4] * b.m_[0] + a.m_[5] * b.m_[4] + a.m_[6] * b.m_[8] + a.m_[7] * b.m_[12];
	m_[5] = a.m_[4] * b.m_[1] + a.m_[5] * b.m_[5] + a.m_[6] * b.m_[9] + a.m_[7] * b.m_[13];
	m_[6] = a.m_[4] * b.m_[2] + a.m_[5] * b.m_[6] + a.m_[6] * b.m_[10] + a.m_[7] * b.m_[14];
	m_[7] = a.m_[4] * b.m_[3] + a.m_[5] * b.m_[7] + a.m_[6] * b.m_[11] + a.m_[7] * b.m_[15];

	m_[8] = a.m_[8] * b.m_[0] + a.m_[9] * b.m_[4] + a.m_[10] * b.m_[8] + a.m_[11] * b.m_[12];
	m_[9] = a.m_[8] * b.m_[1] + a.m_[9] * b.m_[5] + a.m_[10] * b.m_[9] + a.m_[11] * b.m_[13];
	m_[10] = a.m_[8] * b.m_[2] + a.m_[9] * b.m_[6] + a.m_[10] * b.m_[10] + a.m_[11] * b.m_[14];
	m_[11] = a.m_[8] * b.m_[3] + a.m_[9] * b.m_[7] + a.m_[10] * b.m_[11] + a.m_[11] * b.m_[15];

	m_[12] = a.m_[12] * b.m_[0] + a.m_[13] * b.m_[4] + a.m_[14] * b.m_[8] + a.m_[15] * b.m_[12];
	m_[13] = a.m_[12] * b.m_[1] + a.m_[13] * b.m_[5] + a.m_[14] * b.m_[9] + a.m_[15] * b.m_[13];
	m_[14] = a.m_[12] * b.m_[2] + a.m_[13] * b.m_[6] + a.m_[14] * b.m_[10] + a.m_[15] * b.m_[14];
	m_[15] = a.m_[12] * b.m_[3] + a.m_[13] * b.m_[7] + a.m_[14] * b.m_[11] + a.m_[15] * b.m_[15];
	return (*this);
}


void MatrixF::MulP(const float* v, float* vresult) const
{
	vresult[0] = m_[0] * v[0] + m_[1] * v[1] + m_[2] * v[2] + m_[3];
	vresult[1] = m_[4] * v[0] + m_[5] * v[1] + m_[6] * v[2] + m_[7];
	vresult[2] = m_[8] * v[0] + m_[9] * v[1] + m_[10] * v[2] + m_[11];
}

void MatrixF::MulV(const float* v, float* vresult) const
{
	vresult[0] = m_[0] * v[0] + m_[1] * v[1] + m_[2] * v[2];
	vresult[1] = m_[4] * v[0] + m_[5] * v[1] + m_[6] * v[2];
	vresult[2] = m_[8] * v[0] + m_[9] * v[1] + m_[10] * v[2];
}

} // namespace DTS

